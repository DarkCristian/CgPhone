import QtQuick
import QtQuick.Controls

Button {
    id: control
    property color accent: "#2563EB"
    property bool primary: false
    implicitHeight: 48
    font.pixelSize: 15
    font.weight: Font.DemiBold
    contentItem: Text { text: control.text; color: control.primary ? "white" : "#111827"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
    background: Rectangle {
        radius: 12
        color: control.down ? Qt.darker(control.primary ? control.accent : "#FFFFFF", 1.06) : (control.primary ? control.accent : "#FFFFFF")
        border.color: control.primary ? control.accent : "#D8E1EE"
        opacity: control.enabled ? 1 : 0.45
    }
}

