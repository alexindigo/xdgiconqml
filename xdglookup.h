#ifndef XDGLOOKUP_H
#define XDGLOOKUP_H

#include <QString>
#include <QStringList>
#include <QVector>

#include "xdgtypes.h"

class XdgLookup
{
public:
    struct Result {
        QString path;
        bool found = false;
    };

    static Result lookupIcon(const QString &iconName,
                             int size,
                             int scale,
                             const QString &theme,
                             const QStringList &themeHierarchy);

    static QStringList resolveThemeHierarchy(const QString &theme);
    static QStringList getIconPaths();

private:
    static QStringList xdgDataDirectories();
    static QString findBestInDir(const QString &dir,
                                 const QString &iconName,
                                 int size,
                                 int scale);
};

#endif // XDGLOOKUP_H
