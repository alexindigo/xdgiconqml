#include <QTest>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>

#include "xdgindexparse.h"
#include "xdgtypes.h"

class TestXdgIndexParse : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {}

    void testSizeFromDirName() {
        QCOMPARE(XdgIndexParse::sizeFromDirName("48x48/apps"), 48);
        QCOMPARE(XdgIndexParse::sizeFromDirName("scalable/apps"), 0);
        QCOMPARE(XdgIndexParse::sizeFromDirName("256x256"), 256);
        QCOMPARE(XdgIndexParse::sizeFromDirName("actions/22"), 22);
        QCOMPARE(XdgIndexParse::sizeFromDirName("hello"), 0);
    }

    void testParseType() {
        auto meta = XdgIndexParse::parseIndexFile(QString());

        QVERIFY(meta.themeName.isEmpty());
        QVERIFY(meta.directories.isEmpty());
        QVERIFY(meta.iconDirs.isEmpty());
    }

    void testParseFullIndexTheme() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());

        QDir hicolor(tempDir.path());
        hicolor.mkdir("test-theme");

        QFile themeFile(tempDir.path() + "/test-theme/index.theme");
        QVERIFY(themeFile.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream out(&themeFile);
        out << "[Icon Theme]\n";
        out << "Name=Test Theme\n";
        out << "Comment=Test\n";
        out << "Inherits=hicolor\n";
        out << "Directories=48x48/apps,scalable/apps,32x32/apps\n";
        out << "ScaledDirectories=48x48@2/apps\n";
        out << "Hidden=false\n";
        out << "\n";
        out << "[48x48/apps]\n";
        out << "Size=48\n";
        out << "Type=Fixed\n";
        out << "Context=Applications\n";
        out << "\n";
        out << "[scalable/apps]\n";
        out << "Size=48\n";
        out << "Type=Scalable\n";
        out << "MinSize=1\n";
        out << "MaxSize=512\n";
        out << "\n";
        out << "[32x32/apps]\n";
        out << "Size=32\n";
        out << "Type=Threshold\n";
        out << "Threshold=2\n";
        out << "\n";
        out << "[48x48@2/apps]\n";
        out << "Size=48\n";
        out << "Scale=2\n";
        themeFile.close();

        auto meta = XdgIndexParse::parseIndexFile(tempDir.path() + "/test-theme");

        QCOMPARE(meta.themeName, QString("test-theme"));
        QCOMPARE(meta.inherits, QStringList({"hicolor"}));
        QVERIFY(!meta.hidden);
        QCOMPARE(meta.directories.size(), 4); // 3 regular + 1 scaled

        QCOMPARE(meta.iconDirs.size(), 4);

        int fixedCount = 0, scalableCount = 0, thresholdCount = 0, scale2Count = 0;
        for (const auto &d : meta.iconDirs) {
            if (d.subdir == "48x48/apps") {
                QCOMPARE(d.size, 48);
                QCOMPARE(d.type, XdgIconType::Fixed);
                QCOMPARE(d.minSize, 48);
                QCOMPARE(d.maxSize, 48);
                QCOMPARE(d.context, XdgIconContext::Apps);
                QCOMPARE(d.scale, 1);
                fixedCount++;
            } else if (d.subdir == "scalable/apps") {
                QCOMPARE(d.size, 48);
                QCOMPARE(d.type, XdgIconType::Scalable);
                QCOMPARE(d.minSize, 1);
                QCOMPARE(d.maxSize, 512);
                scalableCount++;
            } else if (d.subdir == "32x32/apps") {
                QCOMPARE(d.size, 32);
                QCOMPARE(d.type, XdgIconType::Threshold);
                QCOMPARE(d.threshold, 2);
                thresholdCount++;
            } else if (d.subdir == "48x48@2/apps") {
                QCOMPARE(d.scale, 2);
                scale2Count++;
            }
        }
        QCOMPARE(fixedCount, 1);
        QCOMPARE(scalableCount, 1);
        QCOMPARE(thresholdCount, 1);
        QCOMPARE(scale2Count, 1);
    }

    void testParseIndexThemeSortOrder() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        QDir(tempDir.path()).mkdir("test-sort");

        QFile themeFile(tempDir.path() + "/test-sort/index.theme");
        QVERIFY(themeFile.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream out(&themeFile);
        out << "[Icon Theme]\n";
        out << "Directories=32x32/apps,scalable/apps,48x48/apps,256x256/apps\n";
        out << "\n";
        out << "[32x32/apps]\nSize=32\nType=Threshold\n";
        out << "\n";
        out << "[scalable/apps]\nSize=48\nType=Scalable\nMinSize=1\nMaxSize=512\n";
        out << "\n";
        out << "[48x48/apps]\nSize=48\nType=Fixed\n";
        out << "\n";
        out << "[256x256/apps]\nSize=256\nType=Threshold\n";
        themeFile.close();

        auto meta = XdgIndexParse::parseIndexFile(tempDir.path() + "/test-sort");
        QCOMPARE(meta.iconDirs.size(), 4);

        QCOMPARE(meta.iconDirs[0].subdir, QString("32x32/apps"));
        QCOMPARE(meta.iconDirs[1].subdir, QString("scalable/apps"));
        QCOMPARE(meta.iconDirs[1].type, XdgIconType::Scalable);
        QCOMPARE(meta.iconDirs[2].subdir, QString("48x48/apps"));
        QCOMPARE(meta.iconDirs[3].subdir, QString("256x256/apps"));
    }

    void testParseInheritsOnly() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        QDir(tempDir.path()).mkdir("inherits-only");

        QFile themeFile(tempDir.path() + "/inherits-only/index.theme");
        QVERIFY(themeFile.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream out(&themeFile);
        out << "[Icon Theme]\n";
        out << "Inherits=Adwaita,hicolor\n";
        themeFile.close();

        auto meta = XdgIndexParse::parseIndexFile(tempDir.path() + "/inherits-only");

        QCOMPARE(meta.inherits, QStringList({"Adwaita", "hicolor"}));
        QCOMPARE(meta.iconDirs.size(), 0);
    }

    void testParseMissingIndexTheme() {
        auto meta = XdgIndexParse::parseIndexFile("/nonexistent/path");
        QVERIFY(meta.themeName.isEmpty());
        QVERIFY(meta.iconDirs.isEmpty());
        QVERIFY(meta.inherits.isEmpty());
    }

    void testParseThemeWithoutIconThemeSection() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        QDir(tempDir.path()).mkdir("no-section");

        QFile themeFile(tempDir.path() + "/no-section/index.theme");
        QVERIFY(themeFile.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream out(&themeFile);
        out << "[Desktop Entry]\n";
        out << "Name=Not an icon theme\n";
        themeFile.close();

        auto meta = XdgIndexParse::parseIndexFile(tempDir.path() + "/no-section");
        QCOMPARE(meta.themeName, QString("no-section"));
        QVERIFY(meta.iconDirs.isEmpty());
    }

    void testParseHiddenTheme() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        QDir(tempDir.path()).mkdir("hidden-theme");

        QFile themeFile(tempDir.path() + "/hidden-theme/index.theme");
        QVERIFY(themeFile.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream out(&themeFile);
        out << "[Icon Theme]\n";
        out << "Hidden=true\n";
        themeFile.close();

        auto meta = XdgIndexParse::parseIndexFile(tempDir.path() + "/hidden-theme");
        QVERIFY(meta.hidden);
    }

    void testSizeInferredFromDirName() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        QDir(tempDir.path()).mkdir("inferred");

        QFile themeFile(tempDir.path() + "/inferred/index.theme");
        QVERIFY(themeFile.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream out(&themeFile);
        out << "[Icon Theme]\n";
        out << "Directories=128x128/emblems\n";
        out << "\n";
        out << "[128x128/emblems]\n";
        out << "# No Size= line — should infer 128 from dir name\n";
        out << "Type=Fixed\n";
        themeFile.close();

        auto meta = XdgIndexParse::parseIndexFile(tempDir.path() + "/inferred");
        QCOMPARE(meta.iconDirs.size(), 1);
        QCOMPARE(meta.iconDirs[0].size, 128);
        QCOMPARE(meta.iconDirs[0].type, XdgIconType::Fixed);
    }

    void testParseContextAllValues() {
        QVERIFY(true); // parseContext is tested via testParseFullIndexTheme

        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        QDir(tempDir.path()).mkdir("contexts");

        QFile themeFile(tempDir.path() + "/contexts/index.theme");
        QVERIFY(themeFile.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream out(&themeFile);
        out << "[Icon Theme]\n";
        out << "Directories=48x48/actions,48x48/devices,48x48/emblems\n";
        out << "\n";
        out << "[48x48/actions]\nSize=48\nContext=Actions\n";
        out << "\n";
        out << "[48x48/devices]\nSize=48\nContext=Devices\n";
        out << "\n";
        out << "[48x48/emblems]\nSize=48\nContext=Emblems\n";
        themeFile.close();

        auto meta = XdgIndexParse::parseIndexFile(tempDir.path() + "/contexts");
        QCOMPARE(meta.iconDirs.size(), 3);

        for (const auto &d : meta.iconDirs) {
            if (d.subdir == "48x48/actions")
                QCOMPARE(d.context, XdgIconContext::Actions);
            else if (d.subdir == "48x48/devices")
                QCOMPARE(d.context, XdgIconContext::Devices);
            else if (d.subdir == "48x48/emblems")
                QCOMPARE(d.context, XdgIconContext::Emblems);
        }
    }

    void testHandlesCommentsAndWhitespace() {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());
        QDir(tempDir.path()).mkdir("whitespace");

        QFile themeFile(tempDir.path() + "/whitespace/index.theme");
        QVERIFY(themeFile.open(QIODevice::WriteOnly | QIODevice::Text));
        QTextStream out(&themeFile);
        out << "# This is a comment\n";
        out << "[Icon Theme]\n";
        out << "# Another comment\n";
        out << "Directories = 64x64/apps , 32x32/apps  \n";
        out << "  Inherits = hicolor  \n";
        out << "\n";
        out << "[64x64/apps]\n";
        out << "  Size = 64  \n";
        out << "\n";
        out << "[32x32/apps]\n";
        out << "Size=32\n";
        themeFile.close();

        auto meta = XdgIndexParse::parseIndexFile(tempDir.path() + "/whitespace");
        QCOMPARE(meta.iconDirs.size(), 2);
        QCOMPARE(meta.inherits, QStringList({"hicolor"}));
    }
};

QTEST_MAIN(TestXdgIndexParse)
#include "test_xdgindexparse.moc"
