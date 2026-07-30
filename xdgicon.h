#ifndef XDGICON_H
#define XDGICON_H

#include <QObject>
#include <QStringList>
#include <QUrl>
#include <QtQml/qqmlparserstatus.h>
#include <QtQml/qqmlregistration.h>

class XdgIcon : public QObject, public QQmlParserStatus {
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(int scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(
        QString themeOverride READ themeOverride WRITE setThemeOverride NOTIFY themeOverrideChanged)
    Q_PROPERTY(QUrl path READ path NOTIFY pathChanged)
    Q_PROPERTY(bool found READ found NOTIFY foundChanged)
    Q_PROPERTY(QString extension READ extension NOTIFY extensionChanged)
    Q_PROPERTY(bool isSymbolic READ isSymbolic NOTIFY isSymbolicChanged)
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit XdgIcon(QObject *parent = nullptr);
    ~XdgIcon() override;

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
    static void invalidateCacheForName(const QString &name);

signals:
    void nameChanged();
    void sizeChanged();
    void scaleChanged();
    void themeOverrideChanged();
    void pathChanged();
    void foundChanged();
    void extensionChanged();
    void isSymbolicChanged();

protected:
    void componentComplete() override;
    void classBegin() override {}

private:
    void resolve(bool force = false);
    void updateFromResult(const QUrl &newPath, bool newFound);

    QString m_name;
    int m_size = 48;
    int m_scale = 1;
    QString m_themeOverride;
    QUrl m_path;
    bool m_found = false;
    QString m_extension;
    bool m_isSymbolic = false;
    int m_listenerId = 0;
};

#endif // XDGICON_H
