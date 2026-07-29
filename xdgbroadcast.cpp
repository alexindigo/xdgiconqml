#include "xdgbroadcast.h"

XdgBroadcast::XdgBroadcast(QObject *parent)
    : QObject(parent)
{
}

void XdgBroadcast::startListening()
{
#ifdef WITH_DBUS_BROADCAST
    connectPortalSettings();
    subscribePeerBroadcast();
#endif
}

void XdgBroadcast::broadcastIconChanged(const QString &name)
{
#ifdef WITH_DBUS_BROADCAST
    if (isDampened(name))
        return;

    QDBusMessage signal = QDBusMessage::createSignal(
        QStringLiteral("/org/atmosphera/IconResolver"),
        QStringLiteral("org.atmosphera.IconResolver"),
        QStringLiteral("IconChanged"));
    signal << name;

    QDBusConnection::sessionBus().send(signal);
    markBroadcasted(name);
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

void XdgBroadcast::subscribePeerBroadcast()
{
    bool ok = m_connection.connect(
        QString(),
        QStringLiteral("/org/atmosphera/IconResolver"),
        QStringLiteral("org.atmosphera.IconResolver"),
        QStringLiteral("IconChanged"),
        this,
        SLOT(onIconChanged(QString)));

    if (!ok) {
        qInfo("XdgBroadcast: Peer IconChanged subscription failed");
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

void XdgBroadcast::onIconChanged(const QString &name)
{
    if (name.isEmpty())
        return;
    emit iconChanged(name);
}

bool XdgBroadcast::isDampened(const QString &name)
{
    auto it = m_recentBroadcasts.find(name);
    if (it == m_recentBroadcasts.end())
        return false;

    qint64 elapsed = it->msecsTo(QDateTime::currentDateTimeUtc());
    return elapsed < kDampenMs;
}

void XdgBroadcast::markBroadcasted(const QString &name)
{
    m_recentBroadcasts[name] = QDateTime::currentDateTimeUtc();
}
#endif
