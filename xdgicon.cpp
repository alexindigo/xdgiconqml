#include "xdgicon.h"
#include "xdgcache.h"
#include "xdgicontheme.h"
#include "xdgindexparse.h"
#include "xdglookup.h"

#include <QDir>
#include <QFileInfo>

XdgIcon::XdgIcon(QObject *parent)
    : QObject(parent)
{
}

QString XdgIcon::name() const { return m_name; }

void XdgIcon::setName(const QString &name)
{
    if (m_name == name)
        return;
    m_name = name;
    emit nameChanged();
    resolve();
}

int XdgIcon::size() const { return m_size; }

void XdgIcon::setSize(int size)
{
    if (m_size == size || size < 1)
        return;
    m_size = size;
    emit sizeChanged();
    resolve();
}

int XdgIcon::scale() const { return m_scale; }

void XdgIcon::setScale(int scale)
{
    if (m_scale == scale || scale < 1)
        return;
    m_scale = scale;
    emit scaleChanged();
    resolve();
}

QString XdgIcon::themeOverride() const { return m_themeOverride; }

void XdgIcon::setThemeOverride(const QString &theme)
{
    if (m_themeOverride == theme)
        return;
    m_themeOverride = theme;
    emit themeOverrideChanged();
    resolve();
}

QUrl XdgIcon::path() const { return m_path; }
bool XdgIcon::found() const { return m_found; }
QString XdgIcon::extension() const { return m_extension; }
bool XdgIcon::isSymbolic() const { return m_isSymbolic; }

void XdgIcon::reload()
{
    resolve();
}

void XdgIcon::resolve()
{
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

    QStringList themes = effectiveThemeChain(m_themeOverride);
    QStringList paths = effectiveSearchPaths();

    QString key = XdgCache::makeKey(m_name, m_size, m_scale,
                                     themes.join(QLatin1Char(',')));

    static XdgCache cache;
    XdgCacheEntry cached = cache.lookup(key);
    if (cached.path.isEmpty() && cache.contains(key)) {
        if (m_found) {
            m_found = false;
            m_path.clear();
            m_extension.clear();
            emit foundChanged();
            emit pathChanged();
            emit extensionChanged();
        }
        return;
    }

    if (!cached.path.isEmpty()) {
        QString newPath = cached.path;
        if (m_path.toLocalFile() != newPath) {
            m_found = true;
            m_path = QUrl::fromLocalFile(newPath);
            QFileInfo fi(newPath);
            m_extension = fi.suffix();

            emit foundChanged();
            emit pathChanged();
            emit extensionChanged();
        }
        return;
    }

    auto result = XdgLookup::lookupIcon(m_name, m_size, m_scale, paths, themes);

    if (result.found) {
        XdgCacheEntry entry;
        entry.path = result.path;
        entry.size = m_size;
        entry.scale = m_scale;
        cache.insert(key, entry);

        QUrl newUrl = QUrl::fromLocalFile(result.path);
        if (m_path != newUrl || !m_found) {
            m_found = true;
            m_path = newUrl;
            QFileInfo fi(result.path);
            m_extension = fi.suffix();

            emit foundChanged();
            emit pathChanged();
            emit extensionChanged();
        }
    } else {
        XdgCacheEntry missEntry;
        missEntry.path = QString();
        cache.insert(key, missEntry);

        if (m_found) {
            m_found = false;
            m_path.clear();
            m_extension.clear();
            emit foundChanged();
            emit pathChanged();
            emit extensionChanged();
        }
    }
}

QStringList XdgIcon::effectiveSearchPaths()
{
    XdgIconTheme *theme = XdgIconTheme::instance();
    if (theme) {
        QStringList paths = theme->searchPaths();
        if (!paths.isEmpty())
            return paths;
    }
    return XdgLookup::xdgIconPaths();
}

QStringList XdgIcon::effectiveThemeChain(const QString &themeOverride)
{
    XdgIconTheme *theme = XdgIconTheme::instance();

    if (!themeOverride.isEmpty()) {
        QStringList chain;
        chain.append(themeOverride);
        if (theme) {
            for (const QString &base : theme->searchPaths()) {
                auto meta = XdgIndexParse::parseIndexFile(
                    base + QLatin1Char('/') + themeOverride);
                if (!meta.themeName.isEmpty()) {
                    for (const QString &parent : meta.inherits)
                        chain.append(parent);
                    break;
                }
            }
        }
        if (!chain.contains(QStringLiteral("hicolor")))
            chain.append(QStringLiteral("hicolor"));
        return chain;
    }

    if (theme) {
        QStringList chain = theme->themeChain();
        if (!chain.isEmpty())
            return chain;
    }
    return {QStringLiteral("hicolor")};
}
