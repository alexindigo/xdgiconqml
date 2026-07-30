#include <QCommandLineParser>
#include <QCoreApplication>
#include <QTextStream>

#include "xdgresolver.h"
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
    XdgResolver *r = XdgResolver::instance();

    if (parser.isSet("paths")) {
        out << "Search paths:\n";
        for (const auto &p : r->searchPaths())
            out << "  " << p << "\n";
        return 0;
    }

    if (parser.isSet("chain")) {
        QString theme = parser.value("theme");
        if (!theme.isEmpty())
            r->setCurrentTheme(theme);

        out << "Theme chain:\n";
        for (const auto &t : r->themeChain())
            out << "  " << t << "\n";
        return 0;
    }

    if (parser.positionalArguments().isEmpty())
        parser.showHelp(1);

    QString iconName = parser.positionalArguments().first();
    int size = parser.value("size").toInt();
    int scale = parser.value("scale").toInt();
    QString theme = parser.value("theme");

    if (!theme.isEmpty())
        r->setCurrentTheme(theme);

    auto result = r->lookupIcon(iconName, size, scale);

    if (result.found) {
        out << result.path << "\n";
        return 0;
    } else {
        out << "(not found)\n";
        return 1;
    }
}
