#ifndef XDGICONBROADCAST_H
#define XDGICONBROADCAST_H

#include <QObject>
#include <QString>

#ifdef WITH_DBUS_BROADCAST
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusVariant>
#include <QDateTime>
#include <QHash>
#endif

class XdgIconBroadcast : public QObject {
    Q_OBJECT

public:
    explicit XdgIconBroadcast(QObject *parent = nullptr);

    void startListening();
    void broadcastIconChanged(const QString &name);

signals:
    void themeChanged(const QString &theme);
    void iconChanged(const QString &name);

private:
#ifdef WITH_DBUS_BROADCAST
    void connectPortalSettings();
    void subscribePeerBroadcast();
    bool isDampened(const QString &name);
    void markBroadcasted(const QString &name);

    QDBusConnection m_connection = QDBusConnection::sessionBus();
    QHash<QString, QDateTime> m_recentBroadcasts;
    static constexpr int kDampenMs = 2000;

private slots:
    void onSettingChanged(const QString &ns, const QString &key, const QDBusVariant &value);
    void onIconChanged(const QString &name);
#endif
};

#endif // XDGICONBROADCAST_H
