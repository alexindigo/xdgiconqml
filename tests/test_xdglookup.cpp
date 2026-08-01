#include <QTest>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTemporaryFile>

#include "xdglookup.h"
#include "xdgindexparse.h"

class TestXdgLookup : public QObject {
    Q_OBJECT

    QTemporaryDir m_fixture;

    // Builds a synthetic icon theme tree in m_fixture:
    //   mytheme/
    //     index.theme  (Inherits=parent)
    //     48x48/apps/icon.png
    //     scalable/apps/icon.svg
    //   parent/
    //     index.theme
    //     32x32/apps/icon.png
    //     64x64/apps/parentonly.png
    //   hicolor/
    //     index.theme  (no Inherits)
    //     48x48/apps/hicolor.png
    //   bare-icon-in-base.png  (loose file)
    void setupTree() {
        QDir root(m_fixture.path());

        // mytheme
        QDir mytheme = root.filePath("mytheme");
        mytheme.mkpath("48x48/apps");
        mytheme.mkpath("scalable/apps");

        QFile index1(mytheme.filePath("index.theme"));
        QVERIFY(index1.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream(&index1) << "[Icon Theme]\n"
                             << "Inherits=parent\n"
                             << "Directories=48x48/apps,scalable/apps\n"
                             << "\n"
                             << "[48x48/apps]\nSize=48\nType=Fixed\n"
                             << "\n"
                             << "[scalable/apps]\nSize=48\nType=Scalable\nMinSize=1\nMaxSize=512\n";
        index1.close();

        QFile f(nullptr);
        (void)f;

        // mytheme/48x48/apps/icon.png
        {
            QFile f(mytheme.filePath("48x48/apps/icon.png"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("png", 3);
            f.close();
        }
        // mytheme/scalable/apps/icon.svg
        {
            QFile f(mytheme.filePath("scalable/apps/icon.svg"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("svg", 3);
            f.close();
        }

        // parent
        QDir parent = root.filePath("parent");
        parent.mkpath("32x32/apps");
        parent.mkpath("64x64/apps");

        QFile index2(parent.filePath("index.theme"));
        QVERIFY(index2.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream(&index2) << "[Icon Theme]\n"
                             << "Directories=32x32/apps,64x64/apps\n"
                             << "\n"
                             << "[32x32/apps]\nSize=32\nType=Fixed\n"
                             << "\n"
                             << "[64x64/apps]\nSize=64\nType=Fixed\n";
        index2.close();

        // parent/32x32/apps/icon.png
        {
            QFile f(parent.filePath("32x32/apps/icon.png"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("png32", 5);
            f.close();
        }
        // parent/64x64/apps/parentonly.png
        {
            QFile f(parent.filePath("64x64/apps/parentonly.png"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("parent", 6);
            f.close();
        }

        // hicolor
        QDir hicolor = root.filePath("hicolor");
        hicolor.mkpath("48x48/apps");

        QFile index3(hicolor.filePath("index.theme"));
        QVERIFY(index3.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream(&index3) << "[Icon Theme]\n"
                             << "Directories=48x48/apps\n"
                             << "\n"
                             << "[48x48/apps]\nSize=48\nType=Fixed\n";
        index3.close();

        // hicolor/48x48/apps/hicolor.png
        {
            QFile f(hicolor.filePath("48x48/apps/hicolor.png"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("hicolor", 7);
            f.close();
        }

        // loose file at base
        {
            QFile f(root.filePath("bare-icon-in-base.png"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("bare", 4);
            f.close();
        }

        // theme without index.theme
        QDir noindex = root.filePath("noindex");
        noindex.mkpath("48x48/apps");
        {
            QFile f(noindex.filePath("48x48/apps/noindex-icon.png"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("noindex", 7);
            f.close();
        }

        // theme with hicolor subdirs but no index.theme at root
        QDir basenoindex = root.filePath("basenoindex");
        basenoindex.mkpath("hicolor/512x512/apps");
        {
            QFile f(basenoindex.filePath("hicolor/512x512/apps/kew.png"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("kew", 3);
            f.close();
        }
    }

    QStringList searchPaths() const { return {m_fixture.path()}; }
    QStringList themeChain() const { return {"mytheme", "parent", "hicolor"}; }

private slots:
    void initTestCase() { setupTree(); }

    void testDirMatchesIcon() {
        XdgIconDir d1;
        d1.type = XdgIconType::Fixed;
        d1.size = 48;
        d1.scale = 1;
        QVERIFY(XdgLookup::dirMatchesIcon(d1, 48, 1));
        QVERIFY(!XdgLookup::dirMatchesIcon(d1, 32, 1));
        QVERIFY(!XdgLookup::dirMatchesIcon(d1, 48, 2));

        XdgIconDir d2;
        d2.type = XdgIconType::Scalable;
        d2.size = 48;
        d2.minSize = 1;
        d2.maxSize = 512;
        d2.scale = 1;
        QVERIFY(XdgLookup::dirMatchesIcon(d2, 48, 1));
        QVERIFY(XdgLookup::dirMatchesIcon(d2, 256, 1));
        QVERIFY(!XdgLookup::dirMatchesIcon(d2, 0, 1));
        QVERIFY(!XdgLookup::dirMatchesIcon(d2, 1024, 1));

        XdgIconDir d3;
        d3.type = XdgIconType::Threshold;
        d3.size = 48;
        d3.threshold = 2;
        d3.scale = 1;
        QVERIFY(XdgLookup::dirMatchesIcon(d3, 48, 1));
        QVERIFY(XdgLookup::dirMatchesIcon(d3, 46, 1));
        QVERIFY(XdgLookup::dirMatchesIcon(d3, 50, 1));
        QVERIFY(!XdgLookup::dirMatchesIcon(d3, 44, 1));
    }

    void testFindExactMatch() {
        // Directories=48x48/apps,scalable/apps — 48x48/apps first in file order.
        // At size 48, the Fixed 48x48 PNG must win over the Scalable SVG.
        auto result = XdgLookup::lookupIcon("icon", 48, 1, searchPaths(), themeChain());
        QVERIFY(result.found);
        QVERIFY(result.path.endsWith("mytheme/48x48/apps/icon.png"));
    }

    void testFindScalableSVG() {
        auto result = XdgLookup::lookupIcon("icon", 128, 1, searchPaths(), themeChain());
        QVERIFY(result.found);
        QVERIFY(result.path.contains("scalable/apps/icon.svg"));
    }

    void testFindInParentTheme() {
        auto result =
            XdgLookup::lookupIcon("icon", 32, 1, {"mytheme-not-found"}, {"mytheme", "parent"});
        QVERIFY(!result.found);

        auto result2 = XdgLookup::lookupIcon("icon", 32, 1, searchPaths(), {"parent", "hicolor"});
        QVERIFY(result2.found);
        QVERIFY(result2.path.contains("32x32/apps/icon.png"));
    }

    void testParentOnlyIcon() {
        auto result = XdgLookup::lookupIcon("parentonly", 64, 1, searchPaths(), themeChain());
        QVERIFY(result.found);
        QVERIFY(result.path.contains("64x64/apps/parentonly.png"));
        QVERIFY(result.path.contains("parent"));
    }

    void testHicolorFallback() {
        auto result = XdgLookup::lookupIcon("hicolor", 48, 1, searchPaths(), {"parent", "hicolor"});
        QVERIFY(result.found);
        QVERIFY(result.path.contains("hicolor/48x48/apps/hicolor.png"));
    }

    void testMissingIcon() {
        auto result = XdgLookup::lookupIcon("nonexistent", 48, 1, searchPaths(), themeChain());
        QVERIFY(!result.found);
        QVERIFY(result.path.isEmpty());
    }

    void testAbsolutePath() {
        QString path = m_fixture.path() + "/test-abspath.png";
        QFile f(path);
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write("abs", 3);
        f.close();

        auto result = XdgLookup::lookupIcon(path, 48, 1, {}, {});
        QVERIFY(result.found);
        QCOMPARE(result.path, path);

        auto result2 = XdgLookup::lookupIcon("/no/such/path.png", 48, 1, {}, {});
        QVERIFY(!result2.found);
    }

    void testLooseFileFallback() {
        QStringList paths = {m_fixture.path()};
        auto result = XdgLookup::lookupIcon("bare-icon-in-base", 48, 1, paths, {"nonexistent"});
        QVERIFY(result.found);
        QVERIFY(result.path.contains("bare-icon-in-base.png"));
    }

    void testThemeWithoutIndexTheme() {
        QStringList paths = {m_fixture.path()};
        auto result = XdgLookup::lookupIcon("noindex-icon", 48, 1, paths, {"noindex"});
        QVERIFY(result.found);
        QVERIFY(result.path.contains("noindex-icon.png"));
    }

    void testBaseWithoutIndexThemeKewCase() {
        // kew case: BASE/hicolor/512x512/apps/kew.png exists
        // but BASE/ has no index.theme — should still walk hicolor subtree.
        QStringList paths = {m_fixture.path()};
        auto result = XdgLookup::lookupIcon("kew", 512, 1, paths, {"basenoindex", "hicolor"});
        QVERIFY(result.found);
        QVERIFY(result.path.endsWith("512x512/apps/kew.png"));
    }

    void testFirstInChainPriority() {
        // icon.png exists in both mytheme/48x48/apps/ and parent/32x32/apps/
        // For size 48, mytheme's exact 48 match should win
        auto result48 = XdgLookup::lookupIcon("icon", 48, 1, searchPaths(), themeChain());
        QVERIFY(result48.found);
        QVERIFY(result48.path.contains("mytheme"));
        QVERIFY(!result48.path.contains("parent"));
    }

    void testSizeDistance() {
        XdgIconDir d1;
        d1.type = XdgIconType::Fixed;
        d1.size = 48;
        d1.scale = 1;
        QCOMPARE(XdgLookup::sizeDistance(d1, 48, 1), 0);
        QCOMPARE(XdgLookup::sizeDistance(d1, 64, 1), 16);

        XdgIconDir d2;
        d2.type = XdgIconType::Scalable;
        d2.size = 48;
        d2.minSize = 1;
        d2.maxSize = 512;
        d2.scale = 1;
        QCOMPARE(XdgLookup::sizeDistance(d2, 128, 1), 0);
        QCOMPARE(XdgLookup::sizeDistance(d2, 1, 1), 0);
        QCOMPARE(XdgLookup::sizeDistance(d2, 1024, 1), 512);

        XdgIconDir d3;
        d3.type = XdgIconType::Threshold;
        d3.size = 48;
        d3.threshold = 2;
        d3.scale = 1;
        QCOMPARE(XdgLookup::sizeDistance(d3, 48, 1), 0);
        QCOMPARE(XdgLookup::sizeDistance(d3, 64, 1), 14);
    }

    void testXdgIconPaths() {
        QStringList paths = XdgLookup::xdgIconPaths();
        QVERIFY(paths.size() > 0);
        QVERIFY(!paths.contains(QString()));
    }

    void testEmptyName() {
        auto result = XdgLookup::lookupIcon("", 48, 1, searchPaths(), themeChain());
        QVERIFY(!result.found);
        QVERIFY(result.path.isEmpty());
    }
};

QTEST_MAIN(TestXdgLookup)
#include "test_xdglookup.moc"
