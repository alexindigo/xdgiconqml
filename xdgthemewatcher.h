#ifndef XDGTHEMEWATCHER_H
#define XDGTHEMEWATCHER_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QStringList>

class XdgThemeWatcher : public QObject {
    Q_OBJECT

public:
    explicit XdgThemeWatcher(QObject *parent = nullptr);

    void watchThemeConfig(const QString &path);
    void watchThemeDir(const QString &path);

signals:
    void themeConfigChanged();
    void themeDirChanged(const QString &path);

private:
    QFileSystemWatcher m_watcher;
};

#endif // XDGTHEMEWATCHER_H
