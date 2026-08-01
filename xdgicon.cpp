#include "xdgicon.h"
#include "xdgresolver.h"

#include <QDir>
#include <QFileInfo>

XdgIcon::XdgIcon(QObject *parent) : QObject(parent) {}

XdgIcon::~XdgIcon() {
    if (m_listenerId)
        XdgResolver::instance()->removeInvalidationListener(m_listenerId);
}

void XdgIcon::componentComplete() {
    m_listenerId = XdgResolver::instance()->addInvalidationListener([this](const QString &name) {
        if (name.isEmpty() || name == m_name)
            QMetaObject::invokeMethod(this, [this] { resolve(); }, Qt::QueuedConnection);
    });
    resolve();
}

QString XdgIcon::name() const {
    return m_name;
}

void XdgIcon::setName(const QString &name) {
    if (m_name == name)
        return;
    m_name = name;
    emit nameChanged();
    resolve();
}

int XdgIcon::size() const {
    return m_size;
}

void XdgIcon::setSize(int size) {
    if (m_size == size || size < 1)
        return;
    m_size = size;
    emit sizeChanged();
    resolve();
}

int XdgIcon::scale() const {
    return m_scale;
}

void XdgIcon::setScale(int scale) {
    if (m_scale == scale || scale < 1)
        return;
    m_scale = scale;
    emit scaleChanged();
    resolve();
}

QString XdgIcon::themeOverride() const {
    return m_themeOverride;
}

void XdgIcon::setThemeOverride(const QString &theme) {
    if (m_themeOverride == theme)
        return;
    m_themeOverride = theme;
    emit themeOverrideChanged();
    resolve();
}

QUrl XdgIcon::path() const {
    return m_path;
}
bool XdgIcon::found() const {
    return m_found;
}
QString XdgIcon::extension() const {
    return m_extension;
}
bool XdgIcon::isSymbolic() const {
    return m_isSymbolic;
}

void XdgIcon::reload(bool force) {
    if (force)
        XdgResolver::instance()->invalidateName(m_name);
    resolve();
}

void XdgIcon::resolve(bool /*force*/) {
    bool wasSymbolic = m_isSymbolic;
    m_isSymbolic = m_name.endsWith(QStringLiteral("-symbolic"));
    bool symbolicChanged = (wasSymbolic != m_isSymbolic);

    if (m_name.isEmpty()) {
        if (m_found) {
            m_found = false;
            m_path.clear();
            m_extension.clear();
            emit foundChanged();
            emit pathChanged();
            emit extensionChanged();
        }
        if (symbolicChanged)
            emit isSymbolicChanged();
        return;
    }

    if (symbolicChanged)
        emit isSymbolicChanged();

    auto result = XdgResolver::instance()->lookupIcon(m_name, m_size, m_scale, m_themeOverride);

    QUrl newUrl = result.found ? QUrl::fromLocalFile(result.path) : QUrl();
    if (m_path != newUrl || m_found != result.found) {
        m_found = result.found;
        m_path = newUrl;
        m_extension.clear();
        if (result.found) {
            QFileInfo fi(result.path);
            m_extension = fi.suffix();
        }

        emit foundChanged();
        emit pathChanged();
        emit extensionChanged();
    }
}

void XdgIcon::invalidateCacheForName(const QString &name) {
    XdgResolver::instance()->invalidateName(name);
}
