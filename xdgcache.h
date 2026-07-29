#ifndef XDGCACHE_H
#define XDGCACHE_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QDateTime>

struct XdgCacheEntry {
    QString path;
    int size = 0;
    int scale = 1;
    QDateTime timestamp;
};

class XdgCache : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int maxEntries READ maxEntries WRITE setMaxEntries NOTIFY maxEntriesChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    explicit XdgCache(QObject *parent = nullptr);

    static QString makeKey(const QString &name, int size,
                           int scale, const QString &theme);

    void insert(const QString &key, const XdgCacheEntry &entry);
    XdgCacheEntry lookup(const QString &key) const;
    bool contains(const QString &key) const;
    void remove(const QString &key);

    void clear();
    int count() const;

    Q_INVOKABLE void invalidate();
    Q_INVOKABLE void invalidateTheme(const QString &theme);
    Q_INVOKABLE void invalidateName(const QString &name);

    int maxEntries() const;
    void setMaxEntries(int max);

signals:
    void cacheCleared();
    void maxEntriesChanged();
    void countChanged();

private:
    void evictIfNeeded();
    QHash<QString, XdgCacheEntry> m_cache;
    int m_maxEntries = 500;
};

#endif // XDGCACHE_H
