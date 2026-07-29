#include "xdglookup.h"

XdgLookup::Result XdgLookup::lookupIcon(const QString &iconName,
                                         int size,
                                         int scale,
                                         const QString &theme,
                                         const QStringList &themeHierarchy)
{
    Q_UNUSED(iconName)
    Q_UNUSED(size)
    Q_UNUSED(scale)
    Q_UNUSED(theme)
    Q_UNUSED(themeHierarchy)
    return {};
}

QStringList XdgLookup::resolveThemeHierarchy(const QString &theme)
{
    Q_UNUSED(theme)
    return {};
}

QStringList XdgLookup::getIconPaths()
{
    return {};
}

QStringList XdgLookup::xdgDataDirectories()
{
    return {};
}

QString XdgLookup::findBestInDir(const QString &dir,
                                  const QString &iconName,
                                  int size,
                                  int scale)
{
    Q_UNUSED(dir)
    Q_UNUSED(iconName)
    Q_UNUSED(size)
    Q_UNUSED(scale)
    return {};
}
