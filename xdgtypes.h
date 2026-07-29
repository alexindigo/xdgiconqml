#ifndef XDGTYPES_H
#define XDGTYPES_H

#include <QObject>
#include <QString>
#include <QSize>

enum class XdgIconType {
    Unknown,
    Fixed,
    Scalable,
    Threshold,
};

enum class XdgIconContext {
    Unknown,
    Actions,
    Apps,
    Categories,
    Devices,
    Emblems,
    Emotes,
    International,
    Mimetypes,
    Places,
    Status,
    Stock,
};

struct XdgIconDir {
    QString subdir;
    QSize size;
    QSize maxSize;
    QSize minSize;
    XdgIconType type = XdgIconType::Threshold;
    XdgIconContext context = XdgIconContext::Unknown;
    int scale = 1;
};

struct XdgIconEntry {
    QString path;
    QString name;
    int size = 0;
    int scale = 1;
    bool isScalable = false;
};

#endif // XDGTYPES_H
