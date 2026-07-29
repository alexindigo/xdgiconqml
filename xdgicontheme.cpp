#include "xdgicontheme.h"
#include "xdgbroadcast.h"
#include "xdgicon.h"
#include "xdgindexparse.h"
#include "xdglookup.h"
#include "xdgpathwatcher.h"
#include "xdgthemewatcher.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>

XdgIconTheme *XdgIconTheme::s_instance = nullptr;

XdgIconTheme::XdgIconTheme(QObject *parent)
    : QObject(parent)
{
    if (!s_instance)
        s_instance = this;

    buildSearchPaths();
    detectCurrentTheme();
    scanAvailableThemes();
    resolveThemeChain();
    setupWatchers();
    setupBroadcast();
    m_initialized = true;
}

XdgIconTheme::~XdgIconTheme()
{
    if (s_instance == this)
        s_instance = nullptr;
}

XdgIconTheme *XdgIconTheme::instance()
{
    if (!s_instance) {
        s_instance = new XdgIconTheme();
    } else if (!s_instance->m_initialized) {
        s_instance->buildSearchPaths();
        s_instance->detectCurrentTheme();
        s_instance->scanAvailableThemes();
        s_instance->resolveThemeChain();
        s_instance->m_initialized = true;
    }
    return s_instance;
}

QString XdgIconTheme::currentTheme() const { return m_currentTheme; }

void XdgIconTheme::setCurrentTheme(const QString &theme)
{
    if (m_currentTheme == theme || theme.isEmpty())
        return;
    m_currentTheme = theme;
    resolveThemeChain();
    emit currentThemeChanged();
}

QStringList XdgIconTheme::availableThemes() const { return m_availableThemes; }

QStringList XdgIconTheme::searchPaths() const { return m_searchPaths; }

QStringList XdgIconTheme::themeChain() const { return m_themeChain; }

void XdgIconTheme::rescan()
{
    buildSearchPaths();
    detectCurrentTheme();
    scanAvailableThemes();
    resolveThemeChain();
    emit searchPathsChanged();
    emit availableThemesChanged();
    emit themeReloaded();
    emit propertiesChanged();
}

// -- private --

void XdgIconTheme::detectCurrentTheme()
{
    m_currentTheme = themeFromEnvOrConfig();
    if (m_currentTheme.isEmpty())
        m_currentTheme = QStringLiteral("hicolor");
}

void XdgIconTheme::scanAvailableThemes()
{
    m_availableThemes.clear();
    static const QString index = QStringLiteral("/index.theme");

    for (const QString &base : m_searchPaths) {
        QDir dir(base);
        if (!dir.exists())
            continue;

        const auto entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &entry : entries) {
            if (QFileInfo::exists(base + QLatin1Char('/') + entry + index)
                && !m_availableThemes.contains(entry)) {
                m_availableThemes.append(entry);
            }
        }
    }

    m_availableThemes.sort(Qt::CaseInsensitive);
}

void XdgIconTheme::buildSearchPaths()
{
    m_searchPaths = XdgLookup::xdgIconPaths();
}

void XdgIconTheme::resolveThemeChain()
{
    m_themeChain.clear();
    if (m_currentTheme.isEmpty())
        return;

    QStringList visited;
    QStringList stack;
    stack.append(m_currentTheme);

    while (!stack.isEmpty()) {
        QString theme = stack.takeFirst();
        if (visited.contains(theme))
            continue;
        visited.append(theme);

        for (const QString &base : m_searchPaths) {
            QString themeRoot = base + QLatin1Char('/') + theme;
            if (!QFileInfo::exists(themeRoot))
                continue;

            auto meta = XdgIndexParse::parseIndexFile(themeRoot);
            if (!meta.themeName.isEmpty()) {
                if (!m_themeChain.contains(theme))
                    m_themeChain.append(theme);
                for (const QString &parent : meta.inherits)
                    stack.append(parent);
            }
            break;
        }
    }

    if (!m_themeChain.contains(QStringLiteral("hicolor")))
        m_themeChain.append(QStringLiteral("hicolor"));
}

QString XdgIconTheme::themeFromEnvOrConfig()
{
    const char *env = qgetenv("QS_ICON_THEME");
    if (env && env[0])
        return QString::fromUtf8(env);

    QString gtkTheme = readGtkConfigTheme(
        QDir::homePath() + QStringLiteral("/.config/gtk-3.0/settings.ini"));
    if (!gtkTheme.isEmpty())
        return gtkTheme;

    gtkTheme = readGtkConfigTheme(
        QDir::homePath() + QStringLiteral("/.config/gtk-4.0/settings.ini"));
    if (!gtkTheme.isEmpty())
        return gtkTheme;

    QString qt6ct = readQt6CtTheme();
    if (!qt6ct.isEmpty())
        return qt6ct;

    return {};
}

QString XdgIconTheme::readGtkConfigTheme(const QString &configPath)
{
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    while (!file.atEnd()) {
        QString line = QString::fromUtf8(file.readLine()).trimmed();
        if (line.startsWith(QStringLiteral("gtk-icon-theme-name"))) {
            int eq = line.indexOf(QLatin1Char('='));
            if (eq >= 0)
                return line.mid(eq + 1).trimmed();
        }
    }
    return {};
}

QString XdgIconTheme::readQt6CtTheme()
{
    QString path = QDir::homePath() + QStringLiteral("/.config/qt6ct/qt6ct.conf");
    if (!QFileInfo::exists(path))
        return {};

    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup(QStringLiteral("Appearance"));
    QString theme = settings.value(QStringLiteral("icon_theme")).toString();
    settings.endGroup();
    return theme;
}

void XdgIconTheme::setupWatchers()
{
    m_pathWatcher = new XdgPathWatcher(this);
    m_pathWatcher->watchPaths(m_searchPaths);
    connect(m_pathWatcher, &XdgPathWatcher::rescanTriggered,
            this, &XdgIconTheme::rescan);

    m_themeWatcher = new XdgThemeWatcher(this);
    QString home = QDir::homePath();
    m_themeWatcher->watchThemeConfig(home + QStringLiteral("/.config/gtk-3.0/settings.ini"));
    m_themeWatcher->watchThemeConfig(home + QStringLiteral("/.config/gtk-4.0/settings.ini"));
    m_themeWatcher->watchThemeConfig(home + QStringLiteral("/.config/qt6ct/qt6ct.conf"));
    m_themeWatcher->watchThemeConfig(home + QStringLiteral("/.config/kdeglobals"));
    connect(m_themeWatcher, &XdgThemeWatcher::themeConfigChanged,
            this, &XdgIconTheme::rescan);
}

void XdgIconTheme::setupBroadcast()
{
    m_broadcast = new XdgBroadcast(this);
    connect(m_broadcast, &XdgBroadcast::themeChanged,
            this, &XdgIconTheme::setCurrentTheme);
    connect(m_broadcast, &XdgBroadcast::iconChanged,
            this, [](const QString &name) {
                XdgIcon::invalidateCacheForName(name);
            });
}

bool XdgIconTheme::dbusBroadcastEnabled() const
{
    return m_dbusBroadcastEnabled;
}

void XdgIconTheme::setDbusBroadcastEnabled(bool enabled)
{
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
