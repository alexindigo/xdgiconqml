#ifndef XDGPATHWATCHER_H
#define XDGPATHWATCHER_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QStringList>

class XdgPathWatcher : public QObject {
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
    QFileSystemWatcher m_watcher;
    QStringList m_watchedPaths;
};

#endif // XDGPATHWATCHER_H
