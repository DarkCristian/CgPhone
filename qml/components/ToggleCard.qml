import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    property alias text: label.text
    property alias checked: toggle.checked
    signal toggled(bool checked)
    radius: 14; color: "#FFFFFF"; border.color: "#DDE6F2"; implicitHeight: 62
    Text { id: label; anchors.left: parent.left; anchors.leftMargin: 14; anchors.verticalCenter: parent.verticalCenter; color: "#111827"; font.pixelSize: 13 }
    Switch { id: toggle; anchors.right: parent.right; anchors.rightMargin: 8; anchors.verticalCenter: parent.verticalCenter; onToggled: root.toggled(checked) }
}

