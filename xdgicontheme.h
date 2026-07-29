#ifndef XDGICONTHEME_H
#define XDGICONTHEME_H

#include <QObject>
#include <QStringList>

class XdgIndexParse;
class XdgPathWatcher;
class XdgThemeWatcher;
class XdgCache;

class XdgIconTheme : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentTheme READ currentTheme WRITE setCurrentTheme NOTIFY currentThemeChanged)
    Q_PROPERTY(QStringList availableThemes READ availableThemes NOTIFY availableThemesChanged)

public:
    explicit XdgIconTheme(QObject *parent = nullptr);
    ~XdgIconTheme() override;

    static XdgIconTheme *instance();

    QString currentTheme() const;
    void setCurrentTheme(const QString &theme);

    QStringList availableThemes() const;

    Q_INVOKABLE void rescan();

signals:
    void currentThemeChanged();
    void availableThemesChanged();
    void themeReloaded();

private:
    void detectDefaultTheme();
    void scanAvailableThemes();

    QString m_currentTheme;
    QStringList m_availableThemes;
};

#endif // XDGICONTHEME_H
