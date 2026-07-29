#include "xdgpathwatcher.h"

XdgPathWatcher::XdgPathWatcher(QObject *parent)
    : QObject(parent)
{
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged,
            this, &XdgPathWatcher::iconDirChanged);
}

void XdgPathWatcher::watchPaths(const QStringList &paths)
{
    if (!paths.isEmpty())
        m_watcher.addPaths(paths);
}

void XdgPathWatcher::addPath(const QString &path)
{
    m_watcher.addPath(path);
}

void XdgPathWatcher::removePath(const QString &path)
{
    m_watcher.removePath(path);
}
