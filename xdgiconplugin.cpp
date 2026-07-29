#include <QtQml/qqmlengineextensionplugin.h>

void qml_register_types_XdgIcon();

class XdgIconQmlPlugin : public QQmlEngineExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid)
    Q_DISABLE_COPY_MOVE(XdgIconQmlPlugin)

public:
    XdgIconQmlPlugin(QObject *parent = nullptr)
        : QQmlEngineExtensionPlugin(parent)
    {
        volatile auto registration = &qml_register_types_XdgIcon;
        Q_UNUSED(registration);
    }
};

#include "xdgiconplugin.moc"
