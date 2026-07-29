#ifndef XDGINDEXPARSE_H
#define XDGINDEXPARSE_H

#include <QString>
#include <QVector>

#include "xdgtypes.h"

class XdgIndexParse
{
public:
    static QVector<XdgIconDir> parseIndexFile(const QString &path);
    static QString lookupInherits(const QString &path);

private:
    static QVector<XdgIconDir> parseIconDirectories(const QString &content);
    static QMap<QString, QString> parseKeyValue(const QString &content, const QString &section);
};

#endif // XDGINDEXPARSE_H
