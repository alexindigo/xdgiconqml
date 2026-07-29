#include "xdgicon.h"

XdgIcon::XdgIcon(QObject *parent)
    : QObject(parent)
{
}

QString XdgIcon::name() const { return m_name; }
void XdgIcon::setName(const QString &name)
{
    if (m_name == name) return;
    m_name = name;
    emit nameChanged();
    triggerReload();
}

int XdgIcon::size() const { return m_size; }
void XdgIcon::setSize(int size)
{
    if (m_size == size) return;
    m_size = size;
    emit sizeChanged();
    triggerReload();
}

int XdgIcon::scale() const { return m_scale; }
void XdgIcon::setScale(int scale)
{
    if (m_scale == scale) return;
    m_scale = scale;
    emit scaleChanged();
    triggerReload();
}

QString XdgIcon::themeOverride() const { return m_themeOverride; }
void XdgIcon::setThemeOverride(const QString &theme)
{
    if (m_themeOverride == theme) return;
    m_themeOverride = theme;
    emit themeOverrideChanged();
    triggerReload();
}

QUrl XdgIcon::path() const { return m_path; }
bool XdgIcon::found() const { return m_found; }

void XdgIcon::reload()
{
    triggerReload();
}

void XdgIcon::triggerReload()
{
    // TODO: Implement XDG icon lookup
    m_path.clear();
    m_found = false;
    emit pathChanged();
    emit foundChanged();
}
