#include "xdgthemewatcher.h"

#include <QFileInfo>

XdgThemeWatcher::XdgThemeWatcher(QObject *parent) : QObject(parent) {
    connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &XdgThemeWatcher::onFileChanged);
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this,
            &XdgThemeWatcher::themeDirChanged);
}

void XdgThemeWatcher::watchThemeConfig(const QString &path) {
    if (QFileInfo::exists(path))
        m_watcher.addPath(path);
}

void XdgThemeWatcher::watchThemeDir(const QString &path) {
    if (QFileInfo::exists(path))
        m_watcher.addPath(path);
}

void XdgThemeWatcher::onFileChanged(const QString &path) {
    // Re-add after atomic rename (editors write to temp then rename(),
    // which replaces the inode and drops QFileSystemWatcher's watch).
    if (QFileInfo::exists(path) && !m_watcher.files().contains(path))
        m_watcher.addPath(path);

    emit themeConfigChanged();
}
