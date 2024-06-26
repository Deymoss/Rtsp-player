import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Window

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
}
