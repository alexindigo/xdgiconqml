#include "xdgbroadcast.h"

XdgBroadcast::XdgBroadcast(QObject *parent)
    : QObject(parent)
{
}

void XdgBroadcast::startWatching()
{
#ifdef WITH_DBUS_BROADCAST
    setupDBusWatches();
#endif
}

#ifdef WITH_DBUS_BROADCAST
void XdgBroadcast::setupDBusWatches()
{
    // TODO: Watch org.freedesktop.portal.Settings for icon theme changes
    // TODO: Watch org.kde.GtkConfig or GSettings for theme changes
}
#endif
