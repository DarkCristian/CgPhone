import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    property alias text: label.text
    property bool checked: false
    signal toggled(bool checked)
    radius: 14; color: "#FFFFFF"; border.color: "#DDE6F2"; implicitHeight: 62
    Text { id: label; anchors.left: parent.left; anchors.leftMargin: 14; anchors.verticalCenter: parent.verticalCenter; color: "#111827"; font.pixelSize: 13 }
    Rectangle {
        id: toggle; width: 34; height: 20; radius: 10
        anchors.right: parent.right; anchors.rightMargin: 8; anchors.verticalCenter: parent.verticalCenter
        color: root.checked ? "#2563EB" : "#CBD5E1"
        Rectangle {
            width: 14; height: 14; radius: 7; color: "#FFFFFF"; y: 3
            x: root.checked ? 17 : 3
            Behavior on x { NumberAnimation { duration: 120 } }
        }
        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: { root.checked = !root.checked; root.toggled(root.checked) } }
    }
}
