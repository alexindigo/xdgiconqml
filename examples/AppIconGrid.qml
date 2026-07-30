import QtQuick
import QtQuick.Layouts
import XdgIcon 1.0

Rectangle {
    id: root
    width: 600
    height: 400
    color: "#1e1e2e"

    property var appIds: [
        "firefox", "thunderbird", "org.gnome.Terminal",
        "code-oss", "gimp", "org.gnome.Nautilus",
        "vlc", "steam", "blender"
    ]

    GridLayout {
        anchors.centerIn: parent
        columns: 3
        columnSpacing: 20
        rowSpacing: 20

        Repeater {
            model: root.appIds

            Item {
                id: cell
                required property string modelData
                width: 120
                height: 140

                XdgIcon {
                    id: appIcon
                    name: cell.modelData
                    size: 64
                }

                Rectangle {
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 80; height: 80; radius: 12
                    color: appIcon.found ? "#313244" : "#45475a"

                    Image {
                        anchors.centerIn: parent
                        width: 64; height: 64
                        source: appIcon.path
                        fillMode: Image.PreserveAspectFit
                        visible: appIcon.found
                    }
                }

                Text {
                    anchors.top: parent.top
                    anchors.topMargin: 88
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: cell.modelData
                    color: appIcon.found ? "#cdd6f4" : "#6c7086"
                    font.pixelSize: 11
                    elide: Text.ElideRight
                    width: 120
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
    }
}
