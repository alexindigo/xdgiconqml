#ifndef XDGCACHE_H
#define XDGCACHE_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QUrl>

struct XdgCacheEntry {
    QUrl path;
    int size = 0;
    int scale = 1;
};

class XdgCache : public QObject
{
    Q_OBJECT

public:
    explicit XdgCache(QObject *parent = nullptr);

    void insert(const QString &key, const XdgCacheEntry &entry);
    XdgCacheEntry lookup(const QString &key) const;
    bool contains(const QString &key) const;
    void remove(const QString &key);
    void clear();
    int size() const;

    Q_INVOKABLE void invalidate();

signals:
    void cacheCleared();

private:
    QHash<QString, XdgCacheEntry> m_cache;
};

#endif // XDGCACHE_H
