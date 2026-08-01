#include "xdgpathwatcher.h"

#include <QFileInfo>

XdgPathWatcher::XdgPathWatcher(QObject *parent) : QObject(parent) {
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString &path) {
        emit iconDirChanged(path);
        emit rescanTriggered();
    });
}

void XdgPathWatcher::watchPaths(const QStringList &paths) {
    for (const QString &path : paths)
        addPath(path);
}

void XdgPathWatcher::addPath(const QString &path) {
    if (path.isEmpty() || m_watchedPaths.contains(path))
        return;

    if (QFileInfo::exists(path)) {
        m_watcher.addPath(path);
        m_watchedPaths.append(path);
    }
}

void XdgPathWatcher::removePath(const QString &path) {
    m_watcher.removePath(path);
    m_watchedPaths.removeAll(path);
}

QStringList XdgPathWatcher::watchedPaths() const {
    return m_watchedPaths;
}
