import QtQuick
import XdgIcon 1.0

Rectangle {
    width: 400
    height: 300
    color: "#1e1e2e"

    Column {
        anchors.centerIn: parent
        spacing: 16

        XdgIcon {
            id: icon
            name: "firefox"
            size: 96
        }

        Image {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 96
            height: 96
            source: icon.path
            fillMode: Image.PreserveAspectFit
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: icon.found ? "Found: " + icon.path : "Not found"
            color: icon.found ? "#a6e3a1" : "#f38ba8"
            font.pixelSize: 14
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Extension: " + icon.extension
                  + " | Size: " + icon.size
                  + " | Symbolic: " + icon.isSymbolic
            color: "#cdd6f4"
            font.pixelSize: 12
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 8

            Rectangle {
                width: 120; height: 32; radius: 6; color: "#45475a"
                Text { anchors.centerIn: parent; text: "16px"; color: "#cdd6f4"; font.pixelSize: 12 }
                MouseArea { anchors.fill: parent; onClicked: icon.size = 16 }
            }
            Rectangle {
                width: 120; height: 32; radius: 6; color: "#45475a"
                Text { anchors.centerIn: parent; text: "48px"; color: "#cdd6f4"; font.pixelSize: 12 }
                MouseArea { anchors.fill: parent; onClicked: icon.size = 48 }
            }
            Rectangle {
                width: 120; height: 32; radius: 6; color: "#45475a"
                Text { anchors.centerIn: parent; text: "96px"; color: "#cdd6f4"; font.pixelSize: 12 }
                MouseArea { anchors.fill: parent; onClicked: icon.size = 96 }
            }
        }
    }
}
