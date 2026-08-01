#include <QScopedPointer>
#include <QTest>
#include <QQmlComponent>
#include <QQmlEngine>

#include "qmltestutil.h"

class TestXdgIconQml : public QObject {
    Q_OBJECT
private slots:
    void instantiationNoWarnings() {
        QmlWarningGuard guard;
        QQmlEngine engine;
        engine.addImportPath(QStringLiteral(XDGICONQML_BUILD_DIR));
        QQmlComponent c(&engine);
        c.setData("import XdgIcon 1.0\n"
                  "XdgIcon { name: \"firefox\" }",
                  QUrl());
        QScopedPointer<QObject> obj(c.create());
        QVERIFY2(!obj.isNull(), qUtf8Printable(c.errorString()));
        QVERIFY2(guard.clean(), qUtf8Printable("QML warnings:\n" + guard.captured().join("\n")));
    }
};

QTEST_MAIN(TestXdgIconQml)
#include "test_xdgiconqml.moc"
