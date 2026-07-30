#include <QTest>
#include <QDir>
#include <QFile>
#include <QSignalSpy>
#include <QTemporaryDir>

#include "xdgicon.h"

class TestXdgIcon : public QObject {
    Q_OBJECT

    QTemporaryDir m_fixture;

    void setupTree() {
        QDir root(m_fixture.path());
        root.mkpath("hicolor/48x48/apps");
        root.mkpath("hicolor/scalable/apps");

        {
            QFile f(root.filePath("hicolor/index.theme"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            QTextStream(&f) << "[Icon Theme]\n"
                            << "Directories=scalable/apps,48x48/apps\n"
                            << "\n"
                            << "[scalable/apps]\nSize=48\nType=Scalable\nMinSize=1\nMaxSize=512\n"
                            << "\n"
                            << "[48x48/apps]\nSize=48\nType=Fixed\n";
            f.close();
        }
        {
            QFile f(root.filePath("hicolor/48x48/apps/firefox.png"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("png", 3);
            f.close();
        }
        {
            QFile f(root.filePath("hicolor/48x48/apps/chat-symbolic.png"));
            QVERIFY(f.open(QIODevice::WriteOnly));
            f.write("symbolic", 8);
            f.close();
        }
    }

private slots:
    void initTestCase() { setupTree(); }

    void testDefaults() {
        XdgIcon icon;
        QVERIFY(icon.name().isEmpty());
        QCOMPARE(icon.size(), 48);
        QCOMPARE(icon.scale(), 1);
        QVERIFY(!icon.found());
        QVERIFY(icon.path().isEmpty());
        QVERIFY(icon.extension().isEmpty());
        QVERIFY(!icon.isSymbolic());
    }

    void testEmptyName() {
        XdgIcon icon;
        icon.setName("");
        QVERIFY(!icon.found());
    }

    void testPropertySetters() {
        XdgIcon icon;
        QSignalSpy spy(&icon, &XdgIcon::nameChanged);
        icon.setName("test");
        QCOMPARE(spy.count(), 1);
        icon.setName("test");
        QCOMPARE(spy.count(), 1);
    }

    void testSizeValidation() {
        XdgIcon icon;
        QSignalSpy spy(&icon, &XdgIcon::sizeChanged);
        icon.setSize(0);
        QCOMPARE(spy.count(), 0);
        QCOMPARE(icon.size(), 48);
        icon.setSize(-5);
        QCOMPARE(spy.count(), 0);
        icon.setSize(64);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(icon.size(), 64);
    }

    void testScaleValidation() {
        XdgIcon icon;
        QSignalSpy spy(&icon, &XdgIcon::scaleChanged);
        icon.setScale(0);
        QCOMPARE(spy.count(), 0);
        icon.setScale(2);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(icon.scale(), 2);
    }

    void testThemeOverride() {
        XdgIcon icon;
        QSignalSpy spy(&icon, &XdgIcon::themeOverrideChanged);
        icon.setThemeOverride("Adwaita");
        QCOMPARE(spy.count(), 1);
        icon.setThemeOverride("Adwaita");
        QCOMPARE(spy.count(), 1);
    }

    void testIsSymbolic() {
        XdgIcon icon;
        QSignalSpy spy(&icon, &XdgIcon::isSymbolicChanged);
        icon.setName("chat-symbolic");
        QVERIFY(icon.isSymbolic());

        icon.setName("firefox");
        QVERIFY(!icon.isSymbolic());
    }

    void testReload() {
        XdgIcon icon;
        icon.setName("nonexistent-icon");
        // Reload on empty result shouldn't crash
        icon.reload();
    }
};

QTEST_MAIN(TestXdgIcon)
#include "test_xdgicon.moc"
