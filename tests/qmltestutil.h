#pragma once

#include <QtGlobal>
#include <QStringList>
#include <QTest>

class QmlWarningGuard {
public:
    QmlWarningGuard() {
        s_messages.clear();
        m_prev = qInstallMessageHandler(&QmlWarningGuard::handler);
    }
    ~QmlWarningGuard() { qInstallMessageHandler(m_prev); }

    QStringList captured() const { return s_messages; }
    bool clean() const { return s_messages.isEmpty(); }

private:
    static void handler(QtMsgType type, const QMessageLogContext &ctx, const QString &msg) {
        if (type != QtWarningMsg && type != QtCriticalMsg)
            return;
        const QByteArray cat = ctx.category ? QByteArray(ctx.category) : QByteArray("default");
        if (cat == "qt.qml" || cat.startsWith("qt.qml.") || cat == "qml" ||
            cat.startsWith("qml.") || cat == "default") {
            s_messages.append(QString::fromLatin1(cat) + ": " + msg);
        }
    }
    QtMessageHandler m_prev = nullptr;
    inline static QStringList s_messages;
};
