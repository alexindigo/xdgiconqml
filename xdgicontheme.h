#ifndef XDGICONTHEME_H
#define XDGICONTHEME_H

#include <QObject>
#include <QStringList>
#include <QTimer>
#include <QtQml/qqmlregistration.h>

class XdgBroadcast;
class XdgPathWatcher;
class XdgThemeWatcher;

class XdgIconTheme : public QObject {
    Q_OBJECT
    Q_PROPERTY(
        QString currentTheme READ currentTheme WRITE setCurrentTheme NOTIFY currentThemeChanged)
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
    void setupWatchers();
    void setupBroadcast();
    void onWatcherFired();

    bool m_dbusBroadcastEnabled = false;
    QTimer *m_debounceTimer = nullptr;

    XdgPathWatcher *m_pathWatcher = nullptr;
    XdgThemeWatcher *m_themeWatcher = nullptr;
    XdgBroadcast *m_broadcast = nullptr;

    static XdgIconTheme *s_instance;
};

#endif // XDGICONTHEME_H
