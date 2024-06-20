import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.1

ApplicationWindow {
    id: window
    visible: true
    width: 640
    height: 480
    x: 30
    y: 30
    color: "black"
    VideoTile {
        id: videoTile1
        anchors {
            fill: parent
        }
    }
    Button {
        id: closeButton
        text: "Close Stream"
        height: 20
        width: 80
        anchors.top: parent.top
        anchors.right: parent.right
        onClicked: {
            videoTile1.close()
        }
    }
    Button {
        id: newStream
        text: "New Stream"
        height: 20
        width: 80
        anchors.top: parent.top
        anchors.left: parent.left
        onClicked: {
            videoUrlDialog.open()
        }
    }

Dialog {
    id: videoUrlDialog
    title: "Enter video URL"
    standardButtons: Dialog.NoButton
    Column {
        anchors.fill: parent
        TextField {
            id: videoUrlField
            placeholderText: "Video URL"
        }

        Row {
            Button {
                text: "OK"
                onClicked: {
                    videoTile1.setSource(videoUrlField.text) // Предполагается, что у объекта GstGLVideoItem есть метод setUrl
                    videoUrlDialog.close()
                }
            }

            Button {
                text: "Cancel"
                onClicked: videoUrlDialog.close()
            }
        }
    }

    Component.onCompleted: open()
}
}
