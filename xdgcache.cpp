#include "xdgcache.h"

XdgCache::XdgCache(QObject *parent)
    : QObject(parent)
{
}

void XdgCache::insert(const QString &key, const XdgCacheEntry &entry)
{
    m_cache.insert(key, entry);
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
}

void XdgCache::clear()
{
    m_cache.clear();
}

int XdgCache::size() const
{
    return m_cache.size();
}

void XdgCache::invalidate()
{
    clear();
    emit cacheCleared();
}
