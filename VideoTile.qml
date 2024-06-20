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

}
