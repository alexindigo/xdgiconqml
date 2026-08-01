#include <QTest>

#include "xdgresolver.h"

class BenchXdgResolver : public QObject {
    Q_OBJECT
private slots:
    void initTestCase() { XdgResolver::instance()->reset(); }

    void benchLookupCached() {
        auto *r = XdgResolver::instance();
        (void)r->lookupIcon(QStringLiteral("firefox"), 48, 1);
        QBENCHMARK {
            (void)r->lookupIcon(QStringLiteral("firefox"), 48, 1);
        }
    }

    void benchLookupUncachedKeyBuild() {
        auto *r = XdgResolver::instance();
        int i = 0;
        QBENCHMARK {
            (void)r->lookupIcon(QStringLiteral("nonexistent_%1").arg(++i), 48, 1);
        }
    }
};

QTEST_APPLESS_MAIN(BenchXdgResolver)
#include "bench_xdgresolver.moc"
