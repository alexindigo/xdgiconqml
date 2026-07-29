#include <QTest>
#include <QSignalSpy>

#include "xdgcache.h"

class TestXdgCache : public QObject
{
    Q_OBJECT

private slots:
    void testMakeKey()
    {
        QString key = XdgCache::makeKey("firefox", 48, 1, "hicolor");
        QVERIFY(key.contains("firefox"));
        QVERIFY(key.contains("48"));
        QVERIFY(key.contains("hicolor"));

        QString key2 = XdgCache::makeKey("firefox", 48, 1, "hicolor");
        QCOMPARE(key, key2);

        QString key3 = XdgCache::makeKey("firefox", 64, 1, "hicolor");
        QVERIFY(key != key3);

        QString key4 = XdgCache::makeKey("firefox", 48, 1, "Adwaita");
        QVERIFY(key != key4);
    }

    void testInsertAndLookup()
    {
        XdgCache cache;
        QCOMPARE(cache.count(), 0);

        XdgCacheEntry entry;
        entry.path = "/usr/share/icons/hicolor/48x48/apps/firefox.png";
        entry.size = 48;
        entry.scale = 1;
        QString key = XdgCache::makeKey("firefox", 48, 1, "hicolor");

        cache.insert(key, entry);
        QCOMPARE(cache.count(), 1);
        QVERIFY(cache.contains(key));

        XdgCacheEntry result = cache.lookup(key);
        QCOMPARE(result.path, entry.path);
        QCOMPARE(result.size, entry.size);

        XdgCacheEntry missing = cache.lookup("no-such-key");
        QVERIFY(missing.path.isEmpty());
    }

    void testRemove()
    {
        XdgCache cache;
        XdgCacheEntry entry; entry.path = "/test.png";

        QString key = XdgCache::makeKey("test", 32, 1, "hicolor");
        cache.insert(key, entry);
        QCOMPARE(cache.count(), 1);

        cache.remove(key);
        QCOMPARE(cache.count(), 0);
        QVERIFY(!cache.contains(key));
    }

    void testClear()
    {
        XdgCache cache;
        for (int i = 0; i < 10; ++i) {
            XdgCacheEntry e; e.path = QString("/icon%1.png").arg(i);
            cache.insert(XdgCache::makeKey(QString("icon%1").arg(i), 32, 1, "h"), e);
        }
        QCOMPARE(cache.count(), 10);

        cache.clear();
        QCOMPARE(cache.count(), 0);
    }

    void testInvalidateAll()
    {
        XdgCache cache;
        XdgCacheEntry e; e.path = "/test.png";
        cache.insert(XdgCache::makeKey("a", 32, 1, "h"), e);
        cache.insert(XdgCache::makeKey("b", 32, 1, "h"), e);

        QSignalSpy spy(&cache, &XdgCache::cacheCleared);
        cache.invalidate();
        QCOMPARE(cache.count(), 0);
        QCOMPARE(spy.count(), 1);
    }

    void testInvalidateByName()
    {
        XdgCache cache;
        XdgCacheEntry e; e.path = "/test.png";
        cache.insert(XdgCache::makeKey("firefox", 32, 1, "h"), e);
        cache.insert(XdgCache::makeKey("firefox", 48, 1, "h"), e);
        cache.insert(XdgCache::makeKey("thunderbird", 32, 1, "h"), e);

        cache.invalidateName("firefox");
        QCOMPARE(cache.count(), 1);
        QVERIFY(cache.contains(XdgCache::makeKey("thunderbird", 32, 1, "h")));
        QVERIFY(!cache.contains(XdgCache::makeKey("firefox", 32, 1, "h")));
    }

    void testInvalidateByTheme()
    {
        XdgCache cache;
        XdgCacheEntry e; e.path = "/test.png";
        cache.insert(XdgCache::makeKey("firefox", 32, 1, "Adwaita"), e);
        cache.insert(XdgCache::makeKey("firefox", 32, 1, "hicolor"), e);
        cache.insert(XdgCache::makeKey("firefox", 48, 1, "hicolor"), e);

        cache.invalidateTheme("hicolor");
        QCOMPARE(cache.count(), 1);
        QVERIFY(cache.contains(XdgCache::makeKey("firefox", 32, 1, "Adwaita")));
        QVERIFY(!cache.contains(XdgCache::makeKey("firefox", 32, 1, "hicolor")));
    }

    void testMaxEntries()
    {
        XdgCache cache;
        cache.setMaxEntries(3);

        XdgCacheEntry e; e.path = "/test.png";
        cache.insert(XdgCache::makeKey("a", 32, 1, "h"), e);
        QTest::qSleep(1);
        cache.insert(XdgCache::makeKey("b", 32, 1, "h"), e);
        QTest::qSleep(1);
        cache.insert(XdgCache::makeKey("c", 32, 1, "h"), e);
        QCOMPARE(cache.count(), 3);

        cache.insert(XdgCache::makeKey("d", 32, 1, "h"), e);
        QCOMPARE(cache.count(), 3);
        QVERIFY(!cache.contains(XdgCache::makeKey("a", 32, 1, "h")));
    }

    void testSignals()
    {
        XdgCache cache;
        XdgCacheEntry e; e.path = "/test.png";

        QSignalSpy spyCount(&cache, &XdgCache::countChanged);
        cache.insert(XdgCache::makeKey("a", 32, 1, "h"), e);
        QCOMPARE(spyCount.count(), 1);

        cache.remove(XdgCache::makeKey("a", 32, 1, "h"));
        QCOMPARE(spyCount.count(), 2);
    }

    void testMaxEntriesProperty()
    {
        XdgCache cache;
        QCOMPARE(cache.maxEntries(), 500);

        QSignalSpy spy(&cache, &XdgCache::maxEntriesChanged);
        cache.setMaxEntries(100);
        QCOMPARE(cache.maxEntries(), 100);
        QCOMPARE(spy.count(), 1);

        cache.setMaxEntries(100);
        QCOMPARE(spy.count(), 1);

        cache.setMaxEntries(0); // should not change (clamped)
        QCOMPARE(cache.maxEntries(), 100);
    }
};

QTEST_MAIN(TestXdgCache)
#include "test_xdgcache.moc"
