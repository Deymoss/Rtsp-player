import QtQuick 2.4
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.1
import ACME.VideoItem 1.0

import org.freedesktop.gstreamer.GLVideoItem 1.0

VideoItem {
    id: root
    anchors.fill: parent

    GstGLVideoItem {
        id: video
        objectName: "videoItem"
        anchors.centerIn: parent
        width: parent.width
        height: parent.height
    }

        Button {
            id: closeButton
            text: "Close Stream"
            height: 20
            width: 40
            anchors.top: parent.top
            anchors.right: parent.right
            onClicked: {
                root.close() // Предполагается, что у объекта GstGLVideoItem есть метод stop
            }
        }
        Button {
            id: newStream
            text: "New Stream"
            height: 20
            width: 40
            anchors.top: parent.top
            anchors.left: parent.left
            onClicked: {
                videoUrlDialog.open() // Предполагается, что у объекта GstGLVideoItem есть метод stop
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
