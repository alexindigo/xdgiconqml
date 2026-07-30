#include "xdgindexparse.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include <algorithm>

XdgIndexParse::ThemeMeta XdgIndexParse::parseIndexFile(const QString &themeRoot) {
    ThemeMeta meta;

    const QString indexPath = themeRoot + QStringLiteral("/index.theme");
    QFile file(indexPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return meta;

    meta.themeName = QFileInfo(themeRoot).fileName();

    struct DirEntry {
        QString name;
        int size = 0;
        int maxSize = 0;
        int minSize = 0;
        int threshold = 2;
        int scale = 1;
        XdgIconType type = XdgIconType::Threshold;
        XdgIconContext context = XdgIconContext::Unknown;
    };

    QStringList allDirNames;
    QHash<QString, DirEntry> dirMap;
    QString currentSection;
    QString line;

    auto splitList = [](const QString &value, QChar sep = QLatin1Char(',')) -> QStringList {
        if (value.isEmpty())
            return {};
        QStringList result;
        for (const QString &part : value.split(sep, Qt::SkipEmptyParts)) {
            QString trimmed = part.trimmed();
            if (!trimmed.isEmpty())
                result.append(trimmed);
        }
        return result;
    };

    QTextStream in(&file);
    while (in.readLineInto(&line)) {
        while (!line.isEmpty() &&
               (line.endsWith(QLatin1Char('\r')) || line.endsWith(QLatin1Char(' '))))
            line.chop(1);

        if (line.isEmpty() || line.startsWith(QLatin1Char('#')))
            continue;

        if (line.startsWith(QLatin1Char('[')) && line.endsWith(QLatin1Char(']'))) {
            currentSection = line.mid(1, line.size() - 2);
            continue;
        }

        int eq = line.indexOf(QLatin1Char('='));
        if (eq < 0)
            continue;

        QString key = line.left(eq).trimmed();
        QString value = line.mid(eq + 1).trimmed();

        if (currentSection == QLatin1String("Icon Theme")) {
            if (key == QLatin1String("Directories")) {
                for (const auto &name : splitList(value)) {
                    allDirNames.append(name);
                    dirMap[name].name = name;
                }
            } else if (key == QLatin1String("ScaledDirectories")) {
                meta.scaledDirectories = splitList(value);
                for (const auto &name : meta.scaledDirectories) {
                    if (!dirMap.contains(name)) {
                        allDirNames.append(name);
                        dirMap[name].name = name;
                        dirMap[name].scale = 2;
                    }
                }
            } else if (key == QLatin1String("Inherits")) {
                meta.inherits = splitList(value);
            } else if (key == QLatin1String("Hidden")) {
                meta.hidden = (value.compare(QLatin1String("true"), Qt::CaseInsensitive) == 0);
            }
        } else if (dirMap.contains(currentSection)) {
            DirEntry &entry = dirMap[currentSection];
            if (key == QLatin1String("Size")) {
                bool ok = false;
                int s = value.toInt(&ok);
                if (ok)
                    entry.size = s;
            } else if (key == QLatin1String("Type")) {
                entry.type = parseType(value);
            } else if (key == QLatin1String("MaxSize")) {
                bool ok = false;
                int s = value.toInt(&ok);
                if (ok)
                    entry.maxSize = s;
            } else if (key == QLatin1String("MinSize")) {
                bool ok = false;
                int s = value.toInt(&ok);
                if (ok)
                    entry.minSize = s;
            } else if (key == QLatin1String("Threshold")) {
                bool ok = false;
                int t = value.toInt(&ok);
                if (ok)
                    entry.threshold = t;
            } else if (key == QLatin1String("Scale")) {
                bool ok = false;
                int s = value.toInt(&ok);
                if (ok)
                    entry.scale = s;
            } else if (key == QLatin1String("Context")) {
                entry.context = parseContext(value);
            }
        }
    }

    file.close();

    for (const QString &name : allDirNames) {
        DirEntry &entry = dirMap[name];

        if (entry.size == 0)
            entry.size = sizeFromDirName(name);

        if (entry.minSize == 0)
            entry.minSize = entry.size;
        if (entry.maxSize == 0)
            entry.maxSize = entry.size;

        switch (entry.type) {
        case XdgIconType::Fixed:
            entry.maxSize = entry.size;
            entry.minSize = entry.size;
            break;
        case XdgIconType::Scalable:
            entry.threshold = 0;
            break;
        default:
            break;
        }

        if (entry.maxSize < entry.minSize)
            entry.maxSize = entry.minSize;

        meta.directories.append(name);
    }

    std::stable_sort(allDirNames.begin(), allDirNames.end(),
                     [&](const QString &a, const QString &b) {
                         const auto &da = dirMap[a];
                         const auto &db = dirMap[b];
                         if (da.scale != db.scale)
                             return da.scale < db.scale;
                         if (da.type != db.type) {
                             if (da.type == XdgIconType::Scalable)
                                 return true;
                             if (db.type == XdgIconType::Scalable)
                                 return false;
                         }
                         if (da.maxSize != db.maxSize)
                             return da.maxSize > db.maxSize;
                         return da.size > db.size;
                     });

    meta.iconDirs.reserve(allDirNames.size());
    for (const QString &name : allDirNames) {
        const auto &e = dirMap[name];
        XdgIconDir dir;
        dir.subdir = name;
        dir.size = e.size;
        dir.maxSize = e.maxSize;
        dir.minSize = e.minSize;
        dir.threshold = e.threshold;
        dir.scale = e.scale;
        dir.type = e.type;
        dir.context = e.context;
        meta.iconDirs.append(dir);
    }

    return meta;
}

QVector<XdgIconDir> XdgIndexParse::fallbackIconDirs(const QString &themeRoot) {
    QVector<XdgIconDir> dirs;
    QDir root(themeRoot);
    if (!root.exists())
        return dirs;

    // Walk all subdirectories. Add any directory that either:
    // - has a size pattern in its path (e.g. contains "48x48" or "scalable")
    // - is a leaf (no further subdirectories) — likely a context dir like "apps"
    QStringList stack;
    for (const auto &top : root.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
        stack.append(top);

    while (!stack.isEmpty()) {
        QString rel = stack.takeFirst();
        QDir d(root.absoluteFilePath(rel));
        const auto subs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        int sz = sizeFromDirName(rel);
        bool hasSize = (sz > 0 || rel.contains(QStringLiteral("scalable")));

        if (hasSize || subs.isEmpty()) {
            // If no size, derive from path segments (e.g. "hicolor/512x512/apps"
            // → size 512 from the "512x512" segment).
            if (!hasSize) {
                for (const auto &seg : rel.split(QLatin1Char('/'))) {
                    int s = sizeFromDirName(seg);
                    if (s > 0 || seg.contains(QStringLiteral("scalable"))) {
                        sz = s;
                        hasSize = true;
                        break;
                    }
                }
            }

            XdgIconDir dir;
            dir.subdir = rel;
            dir.size = hasSize ? sz : 0;
            dir.type = rel.contains(QStringLiteral("scalable")) ? XdgIconType::Scalable
                                                                : XdgIconType::Threshold;
            dir.maxSize = dir.size;
            dir.minSize = dir.size;
            dir.threshold = 2;
            dir.scale = 1;
            dirs.append(dir);
        }

        for (const auto &sub : subs)
            stack.append(rel + QLatin1Char('/') + sub);
    }

    return dirs;
}

int XdgIndexParse::sizeFromDirName(const QString &dirName) {
    if (dirName.contains(QStringLiteral("scalable")))
        return 0;

    int num = 0;
    bool found = false;
    for (const QChar &c : dirName) {
        if (c >= QLatin1Char('0') && c <= QLatin1Char('9')) {
            num = num * 10 + (c.unicode() - '0');
            found = true;
        } else if (found) {
            break;
        }
    }
    return num;
}

XdgIconType XdgIndexParse::parseType(const QString &typeStr) {
    if (typeStr.compare(QLatin1String("Fixed"), Qt::CaseInsensitive) == 0)
        return XdgIconType::Fixed;
    if (typeStr.compare(QLatin1String("Scalable"), Qt::CaseInsensitive) == 0)
        return XdgIconType::Scalable;
    if (typeStr.compare(QLatin1String("Threshold"), Qt::CaseInsensitive) == 0)
        return XdgIconType::Threshold;
    return XdgIconType::Threshold;
}

XdgIconContext XdgIndexParse::parseContext(const QString &contextStr) {
    if (contextStr.isEmpty())
        return XdgIconContext::Unknown;

    static const QHash<QString, XdgIconContext> map = {
        {QStringLiteral("Actions"), XdgIconContext::Actions},
        {QStringLiteral("Applications"), XdgIconContext::Apps},
        {QStringLiteral("Categories"), XdgIconContext::Categories},
        {QStringLiteral("Devices"), XdgIconContext::Devices},
        {QStringLiteral("Emblems"), XdgIconContext::Emblems},
        {QStringLiteral("Emotes"), XdgIconContext::Emotes},
        {QStringLiteral("International"), XdgIconContext::International},
        {QStringLiteral("MimeTypes"), XdgIconContext::Mimetypes},
        {QStringLiteral("Places"), XdgIconContext::Places},
        {QStringLiteral("Status"), XdgIconContext::Status},
        {QStringLiteral("Stock"), XdgIconContext::Stock},
    };

    auto it = map.find(contextStr);
    if (it != map.end())
        return it.value();

    return XdgIconContext::Unknown;
}

bool XdgIndexParse::isDirectorySection(const QString &section, const QStringList &knownDirs) {
    return knownDirs.contains(section);
}
