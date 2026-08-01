#ifndef XDGRESOLVER_H
#define XDGRESOLVER_H

#include <QHash>
#include <QString>
#include <QStringList>

#include <QThread>

#include <atomic>
#include <functional>

#include "xdgindexparse.h"
#include "xdglookup.h"
#include "xdgtypes.h"

struct XdgLookupKey {
    QString name;
    int size = 0;
    int scale = 1;
    QString themeOverride;

    bool operator==(const XdgLookupKey &o) const {
        return name == o.name && size == o.size && scale == o.scale &&
               themeOverride == o.themeOverride;
    }
};

size_t qHash(const XdgLookupKey &k, size_t seed = 0);

class XdgResolver {
    // Thread affinity: XdgResolver is thread-affine — all method calls
    // must occur on the same thread. This matches Qt's convention for
    // singleton QML-engine types (QNetworkAccessManager, QQmlEngine).
    //
    // lookupIcon() is const but mutates mutable caches (m_lookupCache,
    // m_themeCache) — this memoization is safe only when the thread-affinity
    // contract is respected. Debug builds assert it; release builds trust it.
public:
    using ThemeMeta = XdgIndexParse::ThemeMeta;
    using InvalidationCallback = std::function<void(const QString &name)>;

    static XdgResolver *instance();

    XdgResolver();
    ~XdgResolver();

    void setSearchPaths(const QStringList &paths);
    void setCurrentTheme(const QString &theme);
    QStringList searchPaths() const;
    QString currentTheme() const;
    QStringList themeChain() const;
    QStringList availableThemes() const;

    XdgLookup::Result lookupIcon(const QString &name, int size, int scale,
                                 const QString &themeOverride = {}) const;

    static QStringList detectSearchPaths();
    static QString detectCurrentTheme();
    static QStringList themeChainFor(const QString &theme, const QStringList &searchPaths);

    void invalidateAll();
    void invalidateName(const QString &name);

    int addInvalidationListener(InvalidationCallback cb);
    void removeInvalidationListener(int handle);

    void reset();

    void resolveThemeChain();

private:
    void ensureThemeMeta(const QString &themeRoot) const;
    void notifyListeners(const QString &name);
    QStringList detectAvailableThemes(const QStringList &paths) const;

    QStringList m_searchPaths;
    QString m_currentTheme;
    QStringList m_themeChain;
    mutable QHash<QString, ThemeMeta> m_themeCache;
    mutable QHash<XdgLookupKey, XdgLookup::Result> m_lookupCache;
    QHash<int, InvalidationCallback> m_listeners;
    int m_nextListenerId = 1;
    QThread *m_ownerThread = nullptr;
    mutable std::atomic<qint64> m_lastMtimeCheckMs{0};
};

#endif // XDGRESOLVER_H
