#ifndef XDGICON_H
#define XDGICON_H

#include <QObject>
#include <QUrl>
#include <QSize>

#include "xdgtypes.h"

class XdgIconTheme;
class XdgCache;
class XdgLookup;

class XdgIcon : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(int scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QString themeOverride READ themeOverride WRITE setThemeOverride NOTIFY themeOverrideChanged)
    Q_PROPERTY(QUrl path READ path NOTIFY pathChanged)
    Q_PROPERTY(bool found READ found NOTIFY foundChanged)

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

    Q_INVOKABLE void reload();

signals:
    void nameChanged();
    void sizeChanged();
    void scaleChanged();
    void themeOverrideChanged();
    void pathChanged();
    void foundChanged();

private:
    void triggerReload();

    QString m_name;
    int m_size = 48;
    int m_scale = 1;
    QString m_themeOverride;
    QUrl m_path;
    bool m_found = false;
};

#endif // XDGICON_H
