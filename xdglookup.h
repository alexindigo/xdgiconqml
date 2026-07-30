#ifndef XDGLOOKUP_H
#define XDGLOOKUP_H

#include <QString>
#include <QStringList>
#include <QVector>

#include "xdgtypes.h"

class XdgLookup {
public:
    struct Result {
        QString path;
        bool found = false;
    };

    static QStringList xdgIconPaths();

    static Result lookupIcon(const QString &iconName, int size, int scale,
                             const QStringList &searchPaths, const QStringList &themeChain);

    static bool dirMatchesIcon(const XdgIconDir &dir, int targetSize, int targetScale);

    static int sizeDistance(const XdgIconDir &dir, int targetSize, int targetScale);

private:
    static QStringList iconExtensions();
    static Result findInTheme(const QString &themeRoot, const QString &iconName, int size,
                              int scale);
    static Result findAnySizeInTheme(const QString &themeRoot, const QString &iconName, int size,
                                     int scale, int *bestDist = nullptr,
                                     Result *bestResult = nullptr);
    static QString findLooseIcon(const QString &iconName, const QStringList &searchPaths);
};

#endif // XDGLOOKUP_H
