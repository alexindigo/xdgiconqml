#include "xdgindexparse.h"

#include <QFile>
#include <QTextStream>

QVector<XdgIconDir> XdgIndexParse::parseIndexFile(const QString &path)
{
    Q_UNUSED(path)
    // TODO: Parse [Icon Theme] section and icon directories
    return {};
}

QString XdgIndexParse::lookupInherits(const QString &path)
{
    Q_UNUSED(path)
    // TODO: Return Inherits= value from index.theme
    return {};
}

QVector<XdgIconDir> XdgIndexParse::parseIconDirectories(const QString &content)
{
    Q_UNUSED(content)
    return {};
}

QMap<QString, QString> XdgIndexParse::parseKeyValue(const QString &content, const QString &section)
{
    Q_UNUSED(content)
    Q_UNUSED(section)
    return {};
}
