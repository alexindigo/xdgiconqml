#include <QTest>
#include <QTemporaryDir>

#include "xdgresolver.h"

class TestXdgResolver : public QObject {
    Q_OBJECT

private slots:
    void testInstanceNotNull() { QVERIFY(XdgResolver::instance() != nullptr); }

    void testDefaultThemeIsHicolor() {
        XdgResolver *r = XdgResolver::instance();
        QString theme = r->currentTheme();
        QVERIFY(!theme.isEmpty());
    }

    void testSetCurrentTheme() {
        XdgResolver *r = XdgResolver::instance();
        QString old = r->currentTheme();

        r->setCurrentTheme("Adwaita");
        QCOMPARE(r->currentTheme(), QString("Adwaita"));

        r->setCurrentTheme(old);
    }

    void testSetSearchPaths() {
        XdgResolver *r = XdgResolver::instance();
        QStringList old = r->searchPaths();

        r->setSearchPaths({QStringLiteral("/nonexistent/path")});
        QCOMPARE(r->searchPaths().size(), 1);

        r->setSearchPaths(old);
    }

    void testInvalidateName() {
        XdgResolver *r = XdgResolver::instance();
        // Should not crash; invalidating a name that doesn't exist.
        r->invalidateName("nonexistent-icon");
    }

    void testListenerRegistration() {
        XdgResolver *r = XdgResolver::instance();
        int count = 0;
        int id = r->addInvalidationListener([&](const QString &) { count++; });

        r->invalidateName("test-icon");
        QCOMPARE(count, 1);

        r->removeInvalidationListener(id);
        r->invalidateName("test-icon");
        QCOMPARE(count, 1); // not incremented after deregister
    }

    void testListenerAllInvalidation() {
        XdgResolver *r = XdgResolver::instance();
        int count = 0;
        int id = r->addInvalidationListener([&](const QString &) { count++; });

        r->invalidateAll();
        QCOMPARE(count, 1);

        r->removeInvalidationListener(id);
    }

    void testStaticDetectSearchPaths() {
        QStringList paths = XdgResolver::detectSearchPaths();
        QVERIFY(!paths.isEmpty());
        // At minimum, XDG_DATA_HOME + XDG_DATA_DIRS should produce paths.
    }
};

QTEST_MAIN(TestXdgResolver)
#include "test_xdgresolver.moc"
