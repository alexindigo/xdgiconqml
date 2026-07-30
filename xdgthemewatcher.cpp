#include "xdgthemewatcher.h"

XdgThemeWatcher::XdgThemeWatcher(QObject *parent) : QObject(parent) {
    connect(&m_watcher, &QFileSystemWatcher::fileChanged, this,
            &XdgThemeWatcher::themeConfigChanged);
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged, this,
            &XdgThemeWatcher::themeDirChanged);
}

void XdgThemeWatcher::watchThemeConfig(const QString &path) {
    m_watcher.addPath(path);
}

void XdgThemeWatcher::watchThemeDir(const QString &path) {
    m_watcher.addPath(path);
}
