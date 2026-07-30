#include "xdgicontheme.h"
#include "xdgbroadcast.h"
#include "xdgresolver.h"
#include "xdgpathwatcher.h"
#include "xdgthemewatcher.h"

#include <QDir>

XdgIconTheme *XdgIconTheme::s_instance = nullptr;

XdgIconTheme::XdgIconTheme(QObject *parent) : QObject(parent) {
    if (!s_instance)
        s_instance = this;
    setupWatchers();
    setupBroadcast();
}

XdgIconTheme::~XdgIconTheme() {
    if (s_instance == this)
        s_instance = nullptr;
}

XdgIconTheme *XdgIconTheme::instance() {
    if (!s_instance)
        s_instance = new XdgIconTheme();
    return s_instance;
}

QString XdgIconTheme::currentTheme() const {
    return XdgResolver::instance()->currentTheme();
}

void XdgIconTheme::setCurrentTheme(const QString &theme) {
    if (XdgResolver::instance()->currentTheme() == theme || theme.isEmpty())
        return;
    XdgResolver::instance()->setCurrentTheme(theme);
    emit currentThemeChanged();
}

QStringList XdgIconTheme::availableThemes() const {
    return XdgResolver::instance()->availableThemes();
}

QStringList XdgIconTheme::searchPaths() const {
    return XdgResolver::instance()->searchPaths();
}

QStringList XdgIconTheme::themeChain() const {
    return XdgResolver::instance()->themeChain();
}

void XdgIconTheme::rescan() {
    XdgResolver::instance()->invalidateAll();
    XdgResolver::instance()->resolveThemeChain();
    emit currentThemeChanged();
    emit searchPathsChanged();
    emit availableThemesChanged();
    emit themeReloaded();
    emit propertiesChanged();
}

void XdgIconTheme::setupWatchers() {
    m_pathWatcher = new XdgPathWatcher(this);
    m_pathWatcher->watchPaths(XdgResolver::instance()->searchPaths());
    connect(m_pathWatcher, &XdgPathWatcher::rescanTriggered, this, &XdgIconTheme::rescan);

    m_themeWatcher = new XdgThemeWatcher(this);
    QString home = QDir::homePath();
    m_themeWatcher->watchThemeConfig(home + QStringLiteral("/.config/gtk-3.0/settings.ini"));
    m_themeWatcher->watchThemeConfig(home + QStringLiteral("/.config/gtk-4.0/settings.ini"));
    m_themeWatcher->watchThemeConfig(home + QStringLiteral("/.config/qt6ct/qt6ct.conf"));
    m_themeWatcher->watchThemeConfig(home + QStringLiteral("/.config/kdeglobals"));
    connect(m_themeWatcher, &XdgThemeWatcher::themeConfigChanged, this, &XdgIconTheme::rescan);
}

void XdgIconTheme::setupBroadcast() {
    m_broadcast = new XdgBroadcast(this);
    connect(m_broadcast, &XdgBroadcast::themeChanged, this, &XdgIconTheme::setCurrentTheme);
    connect(m_broadcast, &XdgBroadcast::iconChanged, this,
            [](const QString &name) { XdgResolver::instance()->invalidateName(name); });
}

bool XdgIconTheme::dbusBroadcastEnabled() const {
    return m_dbusBroadcastEnabled;
}

void XdgIconTheme::setDbusBroadcastEnabled(bool enabled) {
    if (m_dbusBroadcastEnabled == enabled)
        return;
    m_dbusBroadcastEnabled = enabled;
    emit dbusBroadcastEnabledChanged();

    if (enabled && m_broadcast) {
#ifdef WITH_DBUS_BROADCAST
        m_broadcast->startListening();
#endif
    }
}
