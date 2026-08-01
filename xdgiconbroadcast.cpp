#include "xdgiconbroadcast.h"
#include "xdgresolver.h"

XdgIconBroadcast::XdgIconBroadcast(QObject *parent) : QObject(parent) {}

void XdgIconBroadcast::startListening() {
#ifdef WITH_DBUS_BROADCAST
    connectPortalSettings();
    subscribePeerBroadcast();
#endif
}

void XdgIconBroadcast::broadcastIconChanged(const QString &name) {
#ifdef WITH_DBUS_BROADCAST
    if (isDampened(name))
        return;

    QDBusMessage signal = QDBusMessage::createSignal(QStringLiteral("/org/atmosphera/IconResolver"),
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
void XdgIconBroadcast::connectPortalSettings() {
    m_connection = QDBusConnection::sessionBus();
    if (!m_connection.isConnected())
        return;

    bool connected = m_connection.connect(QStringLiteral("org.freedesktop.portal.Desktop"),
                                          QStringLiteral("/org/freedesktop/portal/desktop"),
                                          QStringLiteral("org.freedesktop.portal.Settings"),
                                          QStringLiteral("SettingChanged"), this,
                                          SLOT(onSettingChanged(QString, QString, QDBusVariant)));

    if (!connected) {
        qInfo("XdgIconBroadcast: Desktop portal Settings not available");
    }
}

void XdgIconBroadcast::subscribePeerBroadcast() {
    bool ok =
        m_connection.connect(QString(), QStringLiteral("/org/atmosphera/IconResolver"),
                             QStringLiteral("org.atmosphera.IconResolver"),
                             QStringLiteral("IconChanged"), this, SLOT(onIconChanged(QString)));

    if (!ok) {
        qInfo("XdgIconBroadcast: Peer IconChanged subscription failed");
    }
}

void XdgIconBroadcast::onSettingChanged(const QString &ns, const QString &key,
                                        const QDBusVariant &value) {
    if (ns == QLatin1String("org.freedesktop.appearance") && key == QLatin1String("icon-theme")) {
        QString theme = value.variant().toString();
        if (!theme.isEmpty())
            emit themeChanged(theme);
    }
}

void XdgIconBroadcast::onIconChanged(const QString &name) {
    if (name.isEmpty())
        return;
    if (isDampened(name))
        return;
    markBroadcasted(name);
    emit iconChanged(name);
    XdgResolver::instance()->invalidateName(name);
}

bool XdgIconBroadcast::isDampened(const QString &name) {
    auto it = m_recentBroadcasts.find(name);
    if (it == m_recentBroadcasts.end())
        return false;

    qint64 elapsed = it->msecsTo(QDateTime::currentDateTimeUtc());
    return elapsed < kDampenMs;
}

void XdgIconBroadcast::markBroadcasted(const QString &name) {
    // Prune entries older than the dampen window. Keeps the map
    // structurally bounded: at most N entries where N is the burst
    // rate × kDampenMs (Finding 4a, AUDIT-gemini.md).
    const QDateTime now = QDateTime::currentDateTimeUtc();
    auto it = m_recentBroadcasts.begin();
    while (it != m_recentBroadcasts.end()) {
        if (it.value().msecsTo(now) > kDampenMs)
            it = m_recentBroadcasts.erase(it);
        else
            ++it;
    }
    m_recentBroadcasts[name] = now;
}
#endif
