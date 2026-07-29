#ifndef XDGBROADCAST_H
#define XDGBROADCAST_H

#include <QObject>
#include <QString>

#ifdef WITH_DBUS_BROADCAST
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDateTime>
#include <QHash>
#endif

class XdgBroadcast : public QObject
{
    Q_OBJECT

public:
    explicit XdgBroadcast(QObject *parent = nullptr);

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

    Q_INVOKABLE void onSettingChanged(const QString &ns,
                                       const QString &key,
                                       const QDBusVariant &value);
    Q_INVOKABLE void onIconChanged(const QString &name);

    QDBusConnection m_connection;
    QHash<QString, QDateTime> m_recentBroadcasts;
    static constexpr int kDampenMs = 2000;
#endif
};

#endif // XDGBROADCAST_H
