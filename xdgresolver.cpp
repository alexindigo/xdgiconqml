#include "xdgresolver.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QThread>

#include "xdglookup.h"

size_t qHash(const XdgLookupKey &k, size_t seed) {
    return qHashMulti(seed, k.name, k.size, k.scale, k.themeOverride);
}

namespace {

QString readGtkConfigTheme(const QString &configPath) {
    if (!QFileInfo::exists(configPath))
        return {};
    QSettings s(configPath, QSettings::IniFormat);
    s.beginGroup(QStringLiteral("Settings"));
    QString theme = s.value(QStringLiteral("gtk-icon-theme-name")).toString();
    s.endGroup();
    if (theme.size() >= 2 && theme.startsWith(QLatin1Char('"')) && theme.endsWith(QLatin1Char('"')))
        theme = theme.mid(1, theme.size() - 2);
    return theme;
}

QString readQt6CtTheme() {
    QString path = QDir::homePath() + QStringLiteral("/.config/qt6ct/qt6ct.conf");
    if (!QFileInfo::exists(path))
        return {};
    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("Appearance"));
    QString theme = settings.value(QStringLiteral("icon_theme")).toString();
    settings.endGroup();
    return theme;
}

QString readKdeGlobalsTheme() {
    QString path = QDir::homePath() + QStringLiteral("/.config/kdeglobals");
    if (!QFileInfo::exists(path))
        return {};
    QSettings s(path, QSettings::IniFormat);
    s.beginGroup(QStringLiteral("Icons"));
    QString theme = s.value(QStringLiteral("Theme")).toString();
    s.endGroup();
    return theme;
}

QString themeFromEnvOrConfig() {
    if (!qEnvironmentVariableIsEmpty("QS_ICON_THEME"))
        return qEnvironmentVariable("QS_ICON_THEME");

    QString platform = qEnvironmentVariable("QT_QPA_PLATFORMTHEME");
    if (!platform.isEmpty()) {
        if (platform == QLatin1String("gtk3") || platform == QLatin1String("gtk") ||
            platform == QLatin1String("gtk4")) {
            for (const QString &v : {QStringLiteral("gtk-3.0"), QStringLiteral("gtk-4.0")}) {
                QString t = readGtkConfigTheme(QDir::homePath() + QStringLiteral("/.config/") + v +
                                               QStringLiteral("/settings.ini"));
                if (!t.isEmpty())
                    return t;
            }
        } else if (platform == QLatin1String("qt6ct")) {
            QString t = readQt6CtTheme();
            if (!t.isEmpty())
                return t;
        } else if (platform == QLatin1String("kde") || platform == QLatin1String("kde5") ||
                   platform == QLatin1String("kde6")) {
            QString t = readKdeGlobalsTheme();
            if (!t.isEmpty())
                return t;
        }
    }

    for (const QString &v : {QStringLiteral("gtk-3.0"), QStringLiteral("gtk-4.0")}) {
        QString t = readGtkConfigTheme(QDir::homePath() + QStringLiteral("/.config/") + v +
                                       QStringLiteral("/settings.ini"));
        if (!t.isEmpty())
            return t;
    }

    {
        QString t = readQt6CtTheme();
        if (!t.isEmpty())
            return t;
    }

    {
        QString t = readKdeGlobalsTheme();
        if (!t.isEmpty())
            return t;
    }

    return {};
}

} // namespace

// -- instance --

XdgResolver *XdgResolver::instance() {
    static XdgResolver s;
    return &s;
}

XdgResolver::XdgResolver()
    : m_searchPaths(detectSearchPaths()), m_currentTheme(detectCurrentTheme()) {
    m_ownerThread = QThread::currentThread();
    resolveThemeChain();
}

XdgResolver::~XdgResolver() = default;

// -- configuration --

void XdgResolver::setSearchPaths(const QStringList &paths) {
    Q_ASSERT(m_ownerThread == QThread::currentThread());
    if (m_searchPaths == paths)
        return;
    m_searchPaths = paths;
    m_pathMtimes.clear();
    m_lastMtimeCheckMs.store(0, std::memory_order_relaxed);
    invalidateAll();
    resolveThemeChain();
}

void XdgResolver::setCurrentTheme(const QString &theme) {
    Q_ASSERT(m_ownerThread == QThread::currentThread());
    if (m_currentTheme == theme || theme.isEmpty())
        return;
    m_currentTheme = theme;
    invalidateAll();
    resolveThemeChain();
}

QStringList XdgResolver::searchPaths() const {
    return m_searchPaths;
}

QString XdgResolver::currentTheme() const {
    return m_currentTheme;
}

QStringList XdgResolver::themeChain() const {
    return m_themeChain;
}

QStringList XdgResolver::availableThemes() const {
    return detectAvailableThemes(m_searchPaths);
}

void XdgResolver::resolveThemeChain() {
    Q_ASSERT(m_ownerThread == QThread::currentThread());
    m_themeChain.clear();
    if (m_currentTheme.isEmpty())
        return;
    m_themeChain = themeChainFor(m_currentTheme, m_searchPaths);
}

// -- lookup --

XdgLookup::Result XdgResolver::lookupIcon(const QString &name, int size, int scale,
                                          const QString &themeOverride) const {
    Q_ASSERT(m_ownerThread == QThread::currentThread());
    if (name.isEmpty())
        return {};

    // Lazy mtime check per XDG Icon Theme Spec §Implementation Notes.
    // Stat top-level icon directories at most once per 5 s, only when
    // an actual lookup is in progress (not on a background timer).
    // If any directory has changed, invalidate caches and re-resolve
    // the theme chain so subsequent lookups pick up new content.
    {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        const qint64 last = m_lastMtimeCheckMs.load(std::memory_order_relaxed);
        if (now - last > 5000) {
            bool changed = false;
            for (const QString &path : m_searchPaths) {
                QFileInfo fi(path);
                if (fi.exists()) {
                    QDateTime mtime = fi.lastModified();
                    auto it = m_pathMtimes.find(path);
                    if (it == m_pathMtimes.end() || it.value() != mtime) {
                        m_pathMtimes[path] = mtime;
                        changed = true;
                    }
                }
            }
            if (changed) {
                const_cast<XdgResolver *>(this)->invalidateAll();
                const_cast<XdgResolver *>(this)->resolveThemeChain();
            }
            m_lastMtimeCheckMs.store(now, std::memory_order_relaxed);
        }
    }

    const XdgLookupKey cacheKey{name, size, scale, themeOverride};

    auto it = m_lookupCache.find(cacheKey);
    if (it != m_lookupCache.end())
        return it.value();

    const QStringList themes = themeOverride.isEmpty()
                                   ? m_themeChain
                                   : QStringList({themeOverride, QStringLiteral("hicolor")});

    XdgLookup::Result r = XdgLookup::lookupIcon(name, size, scale, m_searchPaths, themes);

    m_lookupCache[cacheKey] = r;
    return r;
}

// -- detection --

QStringList XdgResolver::detectSearchPaths() {
    QStringList paths;
    auto add = [&](const QString &p) {
        if (!p.isEmpty() && !paths.contains(p))
            paths.append(p);
    };

    add(QDir::homePath() + QStringLiteral("/.local/share/icons"));
    add(QDir::homePath() + QStringLiteral("/.icons"));

    for (const QString &d : QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation))
        add(d + QStringLiteral("/icons"));

    add(QStringLiteral("/usr/local/share/icons"));
    add(QStringLiteral("/usr/share/pixmaps"));
    add(QStringLiteral("/run/current-system/sw/share/icons"));
    add(QStringLiteral("/var/lib/flatpak/exports/share/icons"));
    add(QDir::homePath() + QStringLiteral("/.local/share/flatpak/exports/share/icons"));

    return paths;
}

QString XdgResolver::detectCurrentTheme() {
    QString theme = themeFromEnvOrConfig();
    if (theme.isEmpty())
        theme = QStringLiteral("hicolor");
    return theme;
}

QStringList XdgResolver::themeChainFor(const QString &theme, const QStringList &searchPaths) {
    QStringList chain;
    QStringList visited;
    QStringList stack;
    stack.append(theme);

    while (!stack.isEmpty()) {
        QString t = stack.takeFirst();
        if (visited.contains(t))
            continue;
        visited.append(t);

        for (const QString &base : searchPaths) {
            QString themeRoot = base + QLatin1Char('/') + t;
            if (!QFileInfo::exists(themeRoot))
                continue;

            auto meta = XdgIndexParse::parseIndexFile(themeRoot);
            if (!meta.themeName.isEmpty()) {
                if (!chain.contains(t))
                    chain.append(t);
                for (const QString &parent : meta.inherits)
                    stack.append(parent);
            }
            break;
        }
    }

    if (!chain.contains(QStringLiteral("hicolor")))
        chain.append(QStringLiteral("hicolor"));

    return chain;
}

// -- cache management --

void XdgResolver::ensureThemeMeta(const QString &themeRoot) const {
    Q_ASSERT(m_ownerThread == QThread::currentThread());
    if (m_themeCache.contains(themeRoot))
        return;
    m_themeCache[themeRoot] = XdgIndexParse::parseIndexFile(themeRoot);
}

void XdgResolver::invalidateAll() {
    Q_ASSERT(m_ownerThread == QThread::currentThread());
    m_lookupCache.clear();
    notifyListeners({});
}

void XdgResolver::invalidateName(const QString &name) {
    Q_ASSERT(m_ownerThread == QThread::currentThread());
    // Remove entries matching this icon name from the lookup cache.
    QList<XdgLookupKey> toRemove;
    for (auto it = m_lookupCache.begin(); it != m_lookupCache.end(); ++it) {
        if (it.key().name == name)
            toRemove.append(it.key());
    }
    for (const auto &k : toRemove)
        m_lookupCache.remove(k);

    notifyListeners(name);
}

// -- listeners --

int XdgResolver::addInvalidationListener(InvalidationCallback cb) {
    int id = m_nextListenerId++;
    m_listeners[id] = std::move(cb);
    return id;
}

void XdgResolver::removeInvalidationListener(int handle) {
    m_listeners.remove(handle);
}

void XdgResolver::notifyListeners(const QString &name) {
    // Copy — listeners may deregister during callback.
    const auto listeners = m_listeners;
    for (const auto &cb : listeners)
        cb(name);
}

// -- reset (test seam) --

void XdgResolver::reset() {
    Q_ASSERT(m_ownerThread == QThread::currentThread());
    m_searchPaths = detectSearchPaths();
    m_currentTheme = detectCurrentTheme();
    m_themeChain.clear();
    m_themeCache.clear();
    m_lookupCache.clear();
    m_pathMtimes.clear();
    m_lastMtimeCheckMs.store(0, std::memory_order_relaxed);
    resolveThemeChain();
}

// -- helpers --

QStringList XdgResolver::detectAvailableThemes(const QStringList &paths) const {
    QStringList themes;
    static const QString index = QStringLiteral("/index.theme");

    for (const QString &base : paths) {
        QDir dir(base);
        if (!dir.exists())
            continue;

        const auto entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &entry : entries) {
            if (QFileInfo::exists(base + QLatin1Char('/') + entry + index) &&
                !themes.contains(entry)) {
                themes.append(entry);
            }
        }
    }

    themes.sort(Qt::CaseInsensitive);
    return themes;
}
