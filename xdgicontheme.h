#ifndef XDGICONTHEME_H
#define XDGICONTHEME_H

#include <QObject>
#include <QStringList>
#include <QtQml/qqmlregistration.h>

class XdgBroadcast;
class XdgCache;
class XdgPathWatcher;
class XdgThemeWatcher;

class XdgIconTheme : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentTheme READ currentTheme WRITE setCurrentTheme
               NOTIFY currentThemeChanged)
    Q_PROPERTY(QStringList availableThemes READ availableThemes NOTIFY availableThemesChanged)
    Q_PROPERTY(QStringList searchPaths READ searchPaths NOTIFY searchPathsChanged)
    Q_PROPERTY(bool dbusBroadcastEnabled READ dbusBroadcastEnabled WRITE setDbusBroadcastEnabled
               NOTIFY dbusBroadcastEnabledChanged)
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit XdgIconTheme(QObject *parent = nullptr);
    ~XdgIconTheme() override;

    static XdgIconTheme *instance();

    QString currentTheme() const;
    void setCurrentTheme(const QString &theme);

    QStringList availableThemes() const;
    QStringList searchPaths() const;

    bool dbusBroadcastEnabled() const;
    void setDbusBroadcastEnabled(bool enabled);

    QStringList themeChain() const;

    Q_INVOKABLE void rescan();

signals:
    void currentThemeChanged();
    void availableThemesChanged();
    void searchPathsChanged();
    void dbusBroadcastEnabledChanged();
    void themeReloaded();
    void propertiesChanged();

private:
    void detectCurrentTheme();
    void scanAvailableThemes();
    void buildSearchPaths();
    void resolveThemeChain();
    void setupWatchers();
    void setupBroadcast();

    QString readGtkConfigTheme(const QString &configPath);
    QString readQt6CtTheme();
    QString themeFromEnvOrConfig();

    QString m_currentTheme;
    QStringList m_availableThemes;
    QStringList m_searchPaths;
    QStringList m_themeChain;
    bool m_initialized = false;
    bool m_dbusBroadcastEnabled = false;

    XdgPathWatcher *m_pathWatcher = nullptr;
    XdgThemeWatcher *m_themeWatcher = nullptr;
    XdgBroadcast *m_broadcast = nullptr;

    static XdgIconTheme *s_instance;
};

#endif // XDGICONTHEME_H
