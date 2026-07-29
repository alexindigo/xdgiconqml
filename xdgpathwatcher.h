#ifndef XDGPATHWATCHER_H
#define XDGPATHWATCHER_H

#include <QObject>
#include <QDateTime>
#include <QFileSystemWatcher>
#include <QHash>
#include <QStringList>
#include <QTimer>

class XdgPathWatcher : public QObject
{
    Q_OBJECT

public:
    explicit XdgPathWatcher(QObject *parent = nullptr);

    void watchPaths(const QStringList &paths);
    void addPath(const QString &path);
    void removePath(const QString &path);

    QStringList watchedPaths() const;

signals:
    void iconDirChanged(const QString &path);
    void rescanTriggered();

private:
    void startMtimeTimer();
    void checkMtimes();

    QFileSystemWatcher m_watcher;
    QTimer m_mtimeTimer;
    QStringList m_watchedPaths;
    QHash<QString, QDateTime> m_mtimes;
};

#endif // XDGPATHWATCHER_H
