#include <QTest>

#include "xdglookup.h"

class TestXdgSpec : public QObject {
    Q_OBJECT
private slots:
    void sizeDistance_data() {
        QTest::addColumn<XdgIconType>("type");
        QTest::addColumn<int>("dirSize");
        QTest::addColumn<int>("minSize");
        QTest::addColumn<int>("maxSize");
        QTest::addColumn<int>("dirScale");
        QTest::addColumn<int>("targetSize");
        QTest::addColumn<int>("targetScale");
        QTest::addColumn<int>("expected");

        QTest::newRow("fixed-exact") << XdgIconType::Fixed << 48 << 0 << 0 << 1 << 48 << 1 << 0;
        QTest::newRow("fixed-below") << XdgIconType::Fixed << 48 << 0 << 0 << 1 << 32 << 1 << 16;
        QTest::newRow("fixed-above") << XdgIconType::Fixed << 48 << 0 << 0 << 1 << 64 << 1 << 16;

        QTest::newRow("scalable-in-range")
            << XdgIconType::Scalable << 48 << 1 << 512 << 1 << 128 << 1 << 0;

        QTest::newRow("scalable-below-min")
            << XdgIconType::Scalable << 48 << 8 << 512 << 1 << 4 << 1 << 4;
        QTest::newRow("scalable-above-max")
            << XdgIconType::Scalable << 48 << 8 << 512 << 1 << 1024 << 1 << 512;

        QTest::newRow("scalable-cross-scale-match")
            << XdgIconType::Scalable << 96 << 1 << 256 << 1 << 48 << 2 << 0;
    }

    void sizeDistance() {
        QFETCH(XdgIconType, type);
        QFETCH(int, dirSize);
        QFETCH(int, minSize);
        QFETCH(int, maxSize);
        QFETCH(int, dirScale);
        QFETCH(int, targetSize);
        QFETCH(int, targetScale);
        QFETCH(int, expected);

        XdgIconDir dir;
        dir.type = type;
        dir.size = dirSize;
        dir.minSize = minSize;
        dir.maxSize = maxSize;
        dir.scale = dirScale;

        const QString row = QString::fromLatin1(QTest::currentDataTag());
        if (row == "scalable-below-min" || row == "scalable-above-max") {
            QEXPECT_FAIL("", "Finding 3a — Scalable ignores minSize/maxSize", Continue);
        }
        if (row == "scalable-cross-scale-match") {
            QEXPECT_FAIL("", "Finding 3b — sizeDistance rejects scale mismatch", Continue);
        }
        QCOMPARE(XdgLookup::sizeDistance(dir, targetSize, targetScale), expected);
    }

    void dirMatchesIcon_data() {
        QTest::addColumn<XdgIconType>("type");
        QTest::addColumn<int>("dirSize");
        QTest::addColumn<int>("minSize");
        QTest::addColumn<int>("maxSize");
        QTest::addColumn<int>("dirScale");
        QTest::addColumn<int>("targetSize");
        QTest::addColumn<int>("targetScale");
        QTest::addColumn<bool>("expected");

        QTest::newRow("match-fixed-exact")
            << XdgIconType::Fixed << 48 << 0 << 0 << 1 << 48 << 1 << true;
        QTest::newRow("match-fixed-wrong-size")
            << XdgIconType::Fixed << 48 << 0 << 0 << 1 << 32 << 1 << false;

        QTest::newRow("match-scalable-in-range")
            << XdgIconType::Scalable << 48 << 1 << 512 << 1 << 128 << 1 << true;
        QTest::newRow("match-scalable-below-min")
            << XdgIconType::Scalable << 48 << 8 << 512 << 1 << 4 << 1 << false;
        QTest::newRow("match-scalable-above-max")
            << XdgIconType::Scalable << 48 << 8 << 512 << 1 << 1024 << 1 << false;

        QTest::newRow("match-scale-mismatch")
            << XdgIconType::Fixed << 48 << 0 << 0 << 1 << 48 << 2 << false;
    }

    void dirMatchesIcon() {
        QFETCH(XdgIconType, type);
        QFETCH(int, dirSize);
        QFETCH(int, minSize);
        QFETCH(int, maxSize);
        QFETCH(int, dirScale);
        QFETCH(int, targetSize);
        QFETCH(int, targetScale);
        QFETCH(bool, expected);

        XdgIconDir dir;
        dir.type = type;
        dir.size = dirSize;
        dir.minSize = minSize;
        dir.maxSize = maxSize;
        dir.scale = dirScale;

        QCOMPARE(XdgLookup::dirMatchesIcon(dir, targetSize, targetScale), expected);
    }
};

QTEST_APPLESS_MAIN(TestXdgSpec)
#include "test_xdgspec.moc"
