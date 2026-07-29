import QtQuick
import QtQuick.Controls
import XdgIcon 1.0

Rectangle {
    width: 500
    height: 400
    color: "#1e1e2e"

    Column {
        anchors.centerIn: parent
        spacing: 16
        width: parent.width - 40
        anchors.left: parent.left
        anchors.leftMargin: 20

        Text {
            text: "Current theme: " + XdgIconTheme.currentTheme
            color: "#cdd6f4"
            font.pixelSize: 16
            font.bold: true
        }

        Rectangle {
            width: parent.width
            height: 200
            color: "#313244"
            radius: 8

            ListView {
                anchors.fill: parent
                anchors.margins: 8
                model: XdgIconTheme.availableThemes
                clip: true

                delegate: Rectangle {
                    width: ListView.view.width
                    height: 36
                    color: modelData === XdgIconTheme.currentTheme ? "#45475a" : "transparent"
                    radius: 4

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 12
                        text: modelData
                        color: modelData === XdgIconTheme.currentTheme ? "#a6e3a1" : "#cdd6f4"
                        font.pixelSize: 13
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: XdgIconTheme.currentTheme = modelData
                    }
                }
            }
        }

        XdgIcon {
            id: preview
            name: "folder"
            size: 48
        }

        Row {
            spacing: 16
            anchors.horizontalCenter: parent.horizontalCenter

            Image {
                width: 48; height: 48
                source: preview.path
                fillMode: Image.PreserveAspectFit
            }

            Column {
                Text { text: "Preview: folder"; color: "#cdd6f4"; font.pixelSize: 13 }
                Text {
                    text: preview.found ? preview.path : "Not in this theme"
                    color: preview.found ? "#a6e3a1" : "#f38ba8"
                    font.pixelSize: 11
                    width: 300
                    elide: Text.ElideMiddle
                }
            }
        }

        Rectangle {
            width: 200; height: 36; radius: 6; color: "#45475a"
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                anchors.centerIn: parent
                text: "Rescan themes"
                color: "#cdd6f4"
                font.pixelSize: 13
            }
            MouseArea {
                anchors.fill: parent
                onClicked: XdgIconTheme.rescan()
            }
        }
    }
}
