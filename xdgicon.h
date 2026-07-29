#ifndef XDGICON_H
#define XDGICON_H

#include <QObject>
#include <QUrl>
#include <QStringList>
#include <QtQml/qqmlregistration.h>

class XdgIconTheme;
class XdgCache;

class XdgIcon : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(int scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QString themeOverride READ themeOverride WRITE setThemeOverride
               NOTIFY themeOverrideChanged)
    Q_PROPERTY(QUrl path READ path NOTIFY pathChanged)
    Q_PROPERTY(bool found READ found NOTIFY foundChanged)
    Q_PROPERTY(QString extension READ extension NOTIFY extensionChanged)
    Q_PROPERTY(bool isSymbolic READ isSymbolic NOTIFY isSymbolicChanged)
    QML_ELEMENT

public:
    explicit XdgIcon(QObject *parent = nullptr);

    QString name() const;
    void setName(const QString &name);

    int size() const;
    void setSize(int size);

    int scale() const;
    void setScale(int scale);

    QString themeOverride() const;
    void setThemeOverride(const QString &theme);

    QUrl path() const;
    bool found() const;

    QString extension() const;
    bool isSymbolic() const;

    Q_INVOKABLE void reload(bool force = false);

signals:
    void nameChanged();
    void sizeChanged();
    void scaleChanged();
    void themeOverrideChanged();
    void pathChanged();
    void foundChanged();
    void extensionChanged();
    void isSymbolicChanged();

private:
    void resolve(bool force = false);
    static QStringList effectiveSearchPaths();
    static QStringList effectiveThemeChain(const QString &themeOverride);

    QString m_name;
    int m_size = 48;
    int m_scale = 1;
    QString m_themeOverride;
    QUrl m_path;
    bool m_found = false;
    QString m_extension;
    bool m_isSymbolic = false;
};

#endif // XDGICON_H
