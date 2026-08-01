#include <QTest>

#include <atomic>
#include <thread>
#include <vector>

#include "xdgresolver.h"

class TestXdgResolverConcurrent : public QObject {
    Q_OBJECT
private slots:
    void initTestCase() { XdgResolver::instance()->reset(); }

    void concurrentLookupsDoNotCrash() {
        auto *r = XdgResolver::instance();
        (void)r->searchPaths();

        constexpr int N_THREADS = 8;
        constexpr int N_ITERS = 500;

        std::atomic<int> ready{0};
        std::atomic<bool> go{false};
        std::vector<std::thread> workers;
        workers.reserve(N_THREADS);

        for (int t = 0; t < N_THREADS; ++t) {
            workers.emplace_back([&, t]() {
                ready.fetch_add(1, std::memory_order_release);
                while (!go.load(std::memory_order_acquire)) {
                }
                for (int i = 0; i < N_ITERS; ++i) {
                    const QString name = QStringLiteral("icon_%1_%2").arg(t).arg(i % 32);
                    (void)r->lookupIcon(name, 32 + (i % 4) * 16, 1 + (i % 2));
                }
            });
        }

        while (ready.load(std::memory_order_acquire) < N_THREADS) {
        }
        go.store(true, std::memory_order_release);

        for (auto &w : workers)
            w.join();

        QVERIFY(true);
    }
};

QTEST_APPLESS_MAIN(TestXdgResolverConcurrent)
#include "test_xdgresolver_concurrent.moc"
