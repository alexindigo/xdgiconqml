#ifndef XDGBROADCAST_H
#define XDGBROADCAST_H

#include <QObject>
#include <QString>

#ifdef WITH_DBUS_BROADCAST
#include <QDBusConnection>
#include <QDBusMessage>
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

private:
#ifdef WITH_DBUS_BROADCAST
    void connectPortalSettings();
    Q_INVOKABLE void onSettingChanged(const QString &ns,
                                       const QString &key,
                                       const QDBusVariant &value);
    QDBusConnection m_connection;
#endif
};

#endif // XDGBROADCAST_H
