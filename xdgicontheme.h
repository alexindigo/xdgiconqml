#ifndef XDGICONTHEME_H
#define XDGICONTHEME_H

#include <QObject>
#include <QStringList>
#include <QtQml/qqmlregistration.h>

class XdgCache;
class XdgThemeWatcher;

class XdgIconTheme : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentTheme READ currentTheme WRITE setCurrentTheme
               NOTIFY currentThemeChanged)
    Q_PROPERTY(QStringList availableThemes READ availableThemes NOTIFY availableThemesChanged)
    Q_PROPERTY(QStringList searchPaths READ searchPaths NOTIFY searchPathsChanged)
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

    QStringList themeChain() const;

    Q_INVOKABLE void rescan();

signals:
    void currentThemeChanged();
    void availableThemesChanged();
    void searchPathsChanged();
    void themeReloaded();

private:
    void detectCurrentTheme();
    void scanAvailableThemes();
    void buildSearchPaths();
    void resolveThemeChain();

    QString readGtkConfigTheme(const QString &configPath);
    QString readQt6CtTheme();
    QString themeFromEnvOrConfig();

    QString m_currentTheme;
    QStringList m_availableThemes;
    QStringList m_searchPaths;
    QStringList m_themeChain;
    bool m_initialized = false;

    static XdgIconTheme *s_instance;
};

#endif // XDGICONTHEME_H
