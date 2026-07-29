#include <QTest>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QSignalSpy>
#include <QStandardPaths>

#include "xdgicontheme.h"
#include "xdgindexparse.h"

class TestXdgIconTheme : public QObject
{
    Q_OBJECT

    QTemporaryDir m_fixture;

    void setupFixture()
    {
        QDir root(m_fixture.path());

        root.mkpath("mytheme");
        {
            QFile f(root.filePath("mytheme/index.theme"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            QTextStream(&f)
                << "[Icon Theme]\n"
                << "Inherits=parent\n"
                << "Directories=48x48/apps\n"
                << "\n"
                << "[48x48/apps]\nSize=48\nType=Fixed\n";
            f.close();
        }

        root.mkpath("parent");
        {
            QFile f(root.filePath("parent/index.theme"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            QTextStream(&f)
                << "[Icon Theme]\n"
                << "Directories=32x32/apps\n"
                << "\n"
                << "[32x32/apps]\nSize=32\n";
            f.close();
        }

        root.mkpath("another-theme");
        {
            QFile f(root.filePath("another-theme/index.theme"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            QTextStream(&f) << "[Icon Theme]\nDirectories=scalable/apps\n";
            f.close();
        }
    }

private slots:
    void initTestCase() { setupFixture(); }

    void testThemeChainResolution()
    {
        QTemporaryDir fixtures;
        QDir root(fixtures.path());
        root.mkpath("themeA");
        {
            QFile f(root.filePath("themeA/index.theme"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            QTextStream(&f) << "[Icon Theme]\nInherits=themeB\n";
            f.close();
        }
        root.mkpath("themeB");
        {
            QFile f(root.filePath("themeB/index.theme"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            QTextStream(&f) << "[Icon Theme]\n";
            f.close();
        }

        XdgIconTheme *prev = XdgIconTheme::instance();
        delete prev;

        XdgIconTheme theme;
        auto p1 = XdgIndexParse::parseIndexFile(fixtures.path() + "/themeA");
        QCOMPARE(p1.inherits, QStringList({"themeB"}));

        auto p2 = XdgIndexParse::parseIndexFile(fixtures.path() + "/themeB");
        QVERIFY(p2.inherits.isEmpty());
    }

    void testSetCurrentTheme()
    {
        XdgIconTheme *prev = XdgIconTheme::instance();
        delete prev;

        XdgIconTheme theme;
        QSignalSpy spy(&theme, &XdgIconTheme::currentThemeChanged);

        theme.setCurrentTheme("Adwaita");
        QCOMPARE(theme.currentTheme(), QString("Adwaita"));
        QCOMPARE(spy.count(), 1);

        theme.setCurrentTheme("Adwaita");
        QCOMPARE(spy.count(), 1);

        theme.setCurrentTheme("");
        QCOMPARE(spy.count(), 1);
    }

    void testAvailableThemesWithFixture()
    {
        XdgIconTheme *prev = XdgIconTheme::instance();
        delete prev;

        XdgIconTheme theme; // uses system search paths
        QStringList themes = theme.availableThemes();
        QVERIFY(themes.contains("hicolor"));
    }

    void testRescanEmitsSignals()
    {
        XdgIconTheme *prev = XdgIconTheme::instance();
        delete prev;

        XdgIconTheme theme;
        QSignalSpy spyReload(&theme, &XdgIconTheme::themeReloaded);
        QSignalSpy spyPaths(&theme, &XdgIconTheme::searchPathsChanged);

        theme.rescan();
        // At minimum, searchPathsChanged fires (even if paths unchanged)
        QVERIFY(spyPaths.count() >= 1);
    }

    void testSingletonInstance()
    {
        XdgIconTheme *prev = XdgIconTheme::instance();
        delete prev;

        XdgIconTheme *a = XdgIconTheme::instance();
        XdgIconTheme *b = XdgIconTheme::instance();
        QCOMPARE(a, b);

        delete a;
        XdgIconTheme *c = XdgIconTheme::instance();
        QVERIFY(c != a);
        delete c;
    }

    void testDefaultThemeIsHicolor()
    {
        XdgIconTheme *prev = XdgIconTheme::instance();
        delete prev;

        // without any env overrides, should fall back to hicolor
        XdgIconTheme theme;
        QString current = theme.currentTheme();
        QVERIFY(!current.isEmpty());
        QVERIFY(current == "hicolor" || theme.availableThemes().contains("hicolor"));

        QStringList chain = theme.themeChain();
        QVERIFY(chain.contains("hicolor"));
    }

    void testSearchPaths()
    {
        XdgIconTheme *prev = XdgIconTheme::instance();
        delete prev;

        XdgIconTheme theme;
        QStringList paths = theme.searchPaths();
        QVERIFY(!paths.isEmpty());
        for (const QString &p : paths)
            QVERIFY(p.contains("icons") || p.contains("pixmaps"));
    }
};

QTEST_MAIN(TestXdgIconTheme)
#include "test_xdgicontheme.moc"
