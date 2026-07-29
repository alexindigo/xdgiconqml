#include "xdgbroadcast.h"

XdgBroadcast::XdgBroadcast(QObject *parent)
    : QObject(parent)
{
}

void XdgBroadcast::startListening()
{
#ifdef WITH_DBUS_BROADCAST
    connectPortalSettings();
#endif
}

void XdgBroadcast::broadcastIconChanged(const QString &name)
{
#ifdef WITH_DBUS_BROADCAST
    QDBusMessage signal = QDBusMessage::createSignal(
        QStringLiteral("/org/atmosphera/IconResolver"),
        QStringLiteral("org.atmosphera.IconResolver"),
        QStringLiteral("IconChanged"));
    signal << name;

    QDBusConnection::sessionBus().send(signal);
#else
    Q_UNUSED(name)
#endif
}

#ifdef WITH_DBUS_BROADCAST
void XdgBroadcast::connectPortalSettings()
{
    m_connection = QDBusConnection::sessionBus();
    if (!m_connection.isConnected())
        return;

    bool connected = m_connection.connect(
        QStringLiteral("org.freedesktop.portal.Desktop"),
        QStringLiteral("/org/freedesktop/portal/desktop"),
        QStringLiteral("org.freedesktop.portal.Settings"),
        QStringLiteral("SettingChanged"),
        this,
        SLOT(onSettingChanged(QString, QString, QDBusVariant)));

    if (!connected) {
        qInfo("XdgBroadcast: Desktop portal Settings not available");
    }
}

void XdgBroadcast::onSettingChanged(const QString &ns,
                                     const QString &key,
                                     const QDBusVariant &value)
{
    if (ns == QLatin1String("org.freedesktop.appearance")
        && key == QLatin1String("icon-theme")) {
        QString theme = value.variant().toString();
        if (!theme.isEmpty())
            emit themeChanged(theme);
    }
}
#endif
