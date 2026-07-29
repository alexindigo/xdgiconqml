#ifndef XDGINDEXPARSE_H
#define XDGINDEXPARSE_H

#include <QString>
#include <QStringList>
#include <QVector>

#include "xdgtypes.h"

class XdgIndexParse
{
public:
    struct ThemeMeta {
        QString themeName;
        QStringList directories;
        QStringList scaledDirectories;
        QStringList inherits;
        bool hidden = false;
        QVector<XdgIconDir> iconDirs;
    };

    static ThemeMeta parseIndexFile(const QString &themeRoot);
    static QVector<XdgIconDir> fallbackIconDirs(const QString &themeRoot);
    static int sizeFromDirName(const QString &dirName);

private:
    static XdgIconType parseType(const QString &typeStr);
    static XdgIconContext parseContext(const QString &contextStr);
    static bool isDirectorySection(const QString &section, const QStringList &knownDirs);
};

#endif // XDGINDEXPARSE_H
