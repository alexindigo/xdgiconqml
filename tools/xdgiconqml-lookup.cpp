#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTextStream>

#include "xdgindexparse.h"
#include "xdglookup.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("xdgiconqml-lookup");

    QCommandLineParser parser;
    parser.setApplicationDescription("Resolve an icon name to a filesystem path");
    parser.addHelpOption();

    parser.addOption({{"s", "size"}, "Desired icon size (default 48)", "N", "48"});
    parser.addOption({{"l", "scale"}, "Scale factor (default 1)", "N", "1"});
    parser.addOption({{"t", "theme"}, "Icon theme name", "name", ""});
    parser.addOption({{"p", "paths"}, "Print search paths and exit"});
    parser.addOption({{"c", "chain"}, "Print resolved theme chain and exit"});
    parser.addPositionalArgument("icon", "Icon name to resolve");

    parser.process(app);

    QTextStream out(stdout);

    if (parser.isSet("paths")) {
        out << "Search paths:\n";
        for (const auto &p : XdgLookup::xdgIconPaths())
            out << "  " << p << "\n";
        return 0;
    }

    if (parser.isSet("chain")) {
        out << "Theme chain:\n";
        QString theme = parser.value("theme");
        if (theme.isEmpty())
            theme = "hicolor";
        QStringList chain;
        chain.append(theme);
        QStringList searchPaths = XdgLookup::xdgIconPaths();
        for (const auto &base : searchPaths) {
            auto meta = XdgIndexParse::parseIndexFile(base + "/" + theme);
            if (!meta.themeName.isEmpty()) {
                for (const auto &parent : meta.inherits)
                    chain.append(parent);
                break;
            }
        }
        if (!chain.contains("hicolor"))
            chain.append("hicolor");
        for (const auto &t : chain)
            out << "  " << t << "\n";
        return 0;
    }

    if (parser.positionalArguments().isEmpty()) {
        parser.showHelp(1);
    }

    QString iconName = parser.positionalArguments().first();
    int size = parser.value("size").toInt();
    int scale = parser.value("scale").toInt();
    QString theme = parser.value("theme");

    QStringList searchPaths = XdgLookup::xdgIconPaths();

    QStringList chain;
    if (theme.isEmpty())
        theme = "hicolor";
    chain.append(theme);
    for (const auto &base : searchPaths) {
        auto meta = XdgIndexParse::parseIndexFile(base + "/" + theme);
        if (!meta.themeName.isEmpty()) {
            for (const auto &parent : meta.inherits)
                chain.append(parent);
            break;
        }
    }
    if (!chain.contains("hicolor"))
        chain.append("hicolor");

    auto result = XdgLookup::lookupIcon(iconName, size, scale, searchPaths, chain);

    if (result.found) {
        out << result.path << "\n";
        return 0;
    } else {
        out << "(not found)\n";
        return 1;
    }
}
