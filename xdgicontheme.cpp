#include "xdgicontheme.h"

XdgIconTheme::XdgIconTheme(QObject *parent)
    : QObject(parent)
{
}

XdgIconTheme::~XdgIconTheme() = default;

XdgIconTheme *XdgIconTheme::instance()
{
    static XdgIconTheme inst;
    return &inst;
}

QString XdgIconTheme::currentTheme() const { return m_currentTheme; }

void XdgIconTheme::setCurrentTheme(const QString &theme)
{
    if (m_currentTheme == theme) return;
    m_currentTheme = theme;
    emit currentThemeChanged();
}

QStringList XdgIconTheme::availableThemes() const { return m_availableThemes; }

void XdgIconTheme::rescan()
{
    // TODO: Scan available themes from XDG data dirs
    scanAvailableThemes();
    emit themeReloaded();
}

void XdgIconTheme::detectDefaultTheme()
{
    // TODO: Read from XDG settings or gsettings/kdeglobals
}

void XdgIconTheme::scanAvailableThemes()
{
    // TODO: Walk XDG_DATA_DIRS/icons/ for index.theme files
}
