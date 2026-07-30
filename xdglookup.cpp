#include "xdglookup.h"
#include "xdgindexparse.h"

#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

#include <algorithm>

QStringList XdgLookup::xdgIconPaths() {
    QStringList paths;

    auto pushUnique = [&](const QString &path) {
        if (!path.isEmpty() && !paths.contains(path))
            paths.append(path);
    };

    pushUnique(QDir::homePath() + QStringLiteral("/.local/share/icons"));

    const QStringList dataDirs =
        QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);

    for (const QString &dir : dataDirs)
        pushUnique(dir + QStringLiteral("/icons"));

    pushUnique(QStringLiteral("/run/current-system/sw/share/icons"));
    pushUnique(QStringLiteral("/var/lib/flatpak/exports/share/icons"));

    pushUnique(QDir::homePath() + QStringLiteral("/.local/share/flatpak/exports/share/icons"));

    return paths;
}

XdgLookup::Result XdgLookup::lookupIcon(const QString &iconName, int size, int scale,
                                        const QStringList &searchPaths,
                                        const QStringList &themeChain) {
    if (iconName.isEmpty())
        return {};

    if (iconName.startsWith(QLatin1Char('/'))) {
        if (QFileInfo::exists(iconName))
            return {iconName, true};
        return {};
    }

    // Pass 1: exact size match across theme chain
    for (const QString &theme : themeChain) {
        for (const QString &base : searchPaths) {
            QString themeRoot = base + QLatin1Char('/') + theme;
            if (!QFileInfo::exists(themeRoot))
                continue;

            Result r = findInTheme(themeRoot, iconName, size, scale);
            if (r.found)
                return r;
        }
    }

    // Pass 2: closest size match within each theme, first theme to yield
    // anything wins (spec: "As soon as there is an icon of any size that
    // matches in a theme, the search is stopped.").
    for (const QString &theme : themeChain) {
        Result bestInTheme;
        int bestDist = std::numeric_limits<int>::max();

        for (const QString &base : searchPaths) {
            QString themeRoot = base + QLatin1Char('/') + theme;
            if (!QFileInfo::exists(themeRoot))
                continue;

            Result r =
                findAnySizeInTheme(themeRoot, iconName, size, scale, &bestDist, &bestInTheme);
            Q_UNUSED(r);
        }

        if (bestInTheme.found)
            return bestInTheme;
    }

    // Pass 3: loose-file fallback
    QString loose = findLooseIcon(iconName, searchPaths);
    if (!loose.isEmpty())
        return {loose, true};

    return {};
}

bool XdgLookup::dirMatchesIcon(const XdgIconDir &dir, int targetSize, int targetScale) {
    if (dir.scale != targetScale)
        return false;

    switch (dir.type) {
    case XdgIconType::Fixed:
        return dir.size == targetSize;
    case XdgIconType::Scalable:
        return targetSize >= dir.minSize && targetSize <= dir.maxSize;
    case XdgIconType::Threshold:
        return std::abs(targetSize - dir.size) <= dir.threshold;
    default:
        return false;
    }
}

int XdgLookup::sizeDistance(const XdgIconDir &dir, int targetSize, int targetScale) {
    if (dir.scale != targetScale)
        return std::numeric_limits<int>::max();

    switch (dir.type) {
    case XdgIconType::Scalable:
        return 0;
    case XdgIconType::Fixed:
        return std::abs(dir.size - targetSize);
    case XdgIconType::Threshold:
        return std::abs(dir.size - targetSize);
    default:
        return std::numeric_limits<int>::max();
    }
}

// -- private --

static QStringList extensionsForDir(const XdgIconDir &dir) {
    if (dir.type == XdgIconType::Scalable)
        return {QStringLiteral(".svg"), QStringLiteral(".svgz"), QStringLiteral(".png"),
                QStringLiteral(".xpm")};
    return {QStringLiteral(".png"), QStringLiteral(".svg"), QStringLiteral(".svgz"),
            QStringLiteral(".xpm")};
}

QStringList XdgLookup::iconExtensions() {
    return {QStringLiteral(".png"), QStringLiteral(".svg"), QStringLiteral(".svgz"),
            QStringLiteral(".xpm")};
}

XdgLookup::Result XdgLookup::findInTheme(const QString &themeRoot, const QString &iconName,
                                         int size, int scale) {
    auto meta = XdgIndexParse::parseIndexFile(themeRoot);
    QVector<XdgIconDir> dirs = meta.iconDirs;

    if (dirs.isEmpty())
        dirs = XdgIndexParse::fallbackIconDirs(themeRoot);

    for (const auto &dir : dirs) {
        if (!dirMatchesIcon(dir, size, scale))
            continue;

        for (const QString &ext : iconExtensions()) {
            QString path =
                themeRoot + QLatin1Char('/') + dir.subdir + QLatin1Char('/') + iconName + ext;
            if (QFileInfo::exists(path))
                return {path, true};
        }
    }

    return {};
}

XdgLookup::Result XdgLookup::findAnySizeInTheme(const QString &themeRoot, const QString &iconName,
                                                int size, int scale, int *bestDist,
                                                XdgLookup::Result *bestResult) {
    auto meta = XdgIndexParse::parseIndexFile(themeRoot);
    QVector<XdgIconDir> dirs = meta.iconDirs;

    if (dirs.isEmpty())
        dirs = XdgIndexParse::fallbackIconDirs(themeRoot);

    for (const auto &dir : dirs) {
        int d = sizeDistance(dir, size, scale);
        if (bestDist && d >= *bestDist)
            continue;

        for (const QString &ext : extensionsForDir(dir)) {
            QString path =
                themeRoot + QLatin1Char('/') + dir.subdir + QLatin1Char('/') + iconName + ext;
            if (QFileInfo::exists(path)) {
                if (bestDist && bestResult) {
                    *bestDist = d;
                    *bestResult = {path, true};
                } else {
                    return {path, true};
                }
                break;
            }
        }
    }

    return {};
}

QString XdgLookup::findLooseIcon(const QString &iconName, const QStringList &searchPaths) {
    static const QStringList exts = iconExtensions();

    for (const QString &base : searchPaths) {
        for (const QString &ext : exts) {
            QString path = base + QLatin1Char('/') + iconName + ext;
            if (QFileInfo::exists(path))
                return path;
        }
    }

    return {};
}
