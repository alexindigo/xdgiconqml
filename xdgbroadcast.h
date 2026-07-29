#ifndef XDGBROADCAST_H
#define XDGBROADCAST_H

#include <QObject>
#include <QString>

#ifdef WITH_DBUS_BROADCAST
#include <QDBusConnection>
#include <QDBusServiceWatcher>
#endif

class XdgBroadcast : public QObject
{
    Q_OBJECT

public:
    explicit XdgBroadcast(QObject *parent = nullptr);

    void startWatching();

signals:
    void themeChanged(const QString &theme);

private:
#ifdef WITH_DBUS_BROADCAST
    void setupDBusWatches();
    QDBusServiceWatcher *m_serviceWatcher = nullptr;
#endif
};

#endif // XDGBROADCAST_H
