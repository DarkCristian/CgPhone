import QtQuick
import CgPhone
import QtQuick.Controls

Rectangle {
    id: root
    property alias text: label.text
    property bool checked: false
    signal toggled(bool checked)
    radius: 14; color: Theme.surface; border.color: Theme.border; implicitHeight: 62
    Text { id: label; anchors.left: parent.left; anchors.leftMargin: 14; anchors.verticalCenter: parent.verticalCenter; color: Theme.text; font.pixelSize: 13 }
    Rectangle {
        id: toggle; width: 34; height: 20; radius: 10
        anchors.right: parent.right; anchors.rightMargin: 8; anchors.verticalCenter: parent.verticalCenter
        color: root.checked ? Theme.accent : Theme.disabled
        Rectangle {
            width: 14; height: 14; radius: 7; color: "#FFFFFF"; y: 3
            x: root.checked ? 17 : 3
            Behavior on x { NumberAnimation { duration: 120 } }
        }
        MouseArea { anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: { root.checked = !root.checked; root.toggled(root.checked) } }
    }
}
