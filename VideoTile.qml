import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Window
import ACME.VideoItem

import org.freedesktop.gstreamer.Qt6GLVideoItem 1.0

VideoItem {
    id: root
    anchors.fill: parent

    GstGLQt6VideoItem {
        id: video
        objectName: "videoItem"
        anchors.fill: parent
    }

        Button {
            id: closeButton
            text: "Close Stream"
            height: 30
            width: 80
            anchors.top: parent.top
            anchors.right: parent.right
            onClicked: {
                root.close()
            }
        }
        Button {
            id: newStream
            text: "New Stream"
            height: 30
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
                        root.setSource(videoUrlField.text) // Предполагается, что у объекта GstGLVideoItem есть метод setUrl
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
