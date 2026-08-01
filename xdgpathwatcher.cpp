#include "xdgpathwatcher.h"

#include <QDir>
#include <QFileInfo>

XdgPathWatcher::XdgPathWatcher(QObject *parent) : QObject(parent) {
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString &path) {
        emit iconDirChanged(path);
        emit rescanTriggered();
    });

    startMtimeTimer();
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
        m_mtimes[path] = QFileInfo(path).lastModified();
    }
}

void XdgPathWatcher::removePath(const QString &path) {
    m_watcher.removePath(path);
    m_watchedPaths.removeAll(path);
    m_mtimes.remove(path);
}

QStringList XdgPathWatcher::watchedPaths() const {
    return m_watchedPaths;
}

void XdgPathWatcher::startMtimeTimer() {
    // Fallback polling for filesystems where QFileSystemWatcher is
    // unreliable (FUSE mounts, NFS/SMB remote homes, containers with
    // inotify pressure). 5 s is a compromise between change visibility
    // and idle CPU wakeups (~17k/day); do NOT lower without measuring.
    // See Finding 4b in AUDIT-gemini.md.
    m_mtimeTimer.setInterval(5000);
    connect(&m_mtimeTimer, &QTimer::timeout, this, &XdgPathWatcher::checkMtimes);
    m_mtimeTimer.start();
}

void XdgPathWatcher::checkMtimes() {
    bool changed = false;
    for (const QString &path : m_watchedPaths) {
        QDateTime mtime = QFileInfo(path).lastModified();
        if (mtime != m_mtimes.value(path)) {
            m_mtimes[path] = mtime;
            changed = true;
        }
    }
    if (changed)
        emit rescanTriggered();
}
