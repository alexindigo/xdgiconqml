#include "xdgcache.h"

#include <QRegularExpression>

XdgCache::XdgCache(QObject *parent)
    : QObject(parent)
{
}

QString XdgCache::makeKey(const QString &name, int size,
                            int scale, const QString &theme)
{
    return QStringLiteral("%1\x1f%2\x1f%3\x1f%4")
        .arg(name)
        .arg(size)
        .arg(scale)
        .arg(theme);
}

void XdgCache::insert(const QString &key, const XdgCacheEntry &entry)
{
    bool isNew = !m_cache.contains(key);
    m_cache[key] = entry;
    m_cache[key].timestamp = QDateTime::currentDateTimeUtc();

    if (isNew) {
        evictIfNeeded();
        emit countChanged();
    }
}

XdgCacheEntry XdgCache::lookup(const QString &key) const
{
    return m_cache.value(key);
}

bool XdgCache::contains(const QString &key) const
{
    return m_cache.contains(key);
}

void XdgCache::remove(const QString &key)
{
    m_cache.remove(key);
    emit countChanged();
}

void XdgCache::clear()
{
    m_cache.clear();
    emit countChanged();
}

int XdgCache::count() const
{
    return m_cache.size();
}

void XdgCache::invalidate()
{
    clear();
    emit cacheCleared();
}

void XdgCache::invalidateTheme(const QString &theme)
{
    QString suffix = QStringLiteral("\x1f") + theme;
    QStringList toRemove;
    for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
        if (it.key().endsWith(suffix))
            toRemove.append(it.key());
    }
    for (const auto &k : toRemove)
        m_cache.remove(k);

    if (!toRemove.isEmpty())
        emit countChanged();
}

void XdgCache::invalidateName(const QString &name)
{
    QString prefix = name + QLatin1Char('\x1f');
    QStringList toRemove;
    for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
        if (it.key().startsWith(prefix))
            toRemove.append(it.key());
    }
    for (const auto &k : toRemove)
        m_cache.remove(k);

    if (!toRemove.isEmpty())
        emit countChanged();
}

int XdgCache::maxEntries() const
{
    return m_maxEntries;
}

void XdgCache::setMaxEntries(int max)
{
    if (m_maxEntries == max || max < 1)
        return;
    m_maxEntries = max;
    emit maxEntriesChanged();
    evictIfNeeded();
}

void XdgCache::evictIfNeeded()
{
    if (m_cache.size() <= m_maxEntries)
        return;

    QString oldest;
    QDateTime oldestTime;
    for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
        if (oldest.isEmpty() || it->timestamp < oldestTime) {
            oldest = it.key();
            oldestTime = it->timestamp;
        }
    }
    m_cache.remove(oldest);
}
