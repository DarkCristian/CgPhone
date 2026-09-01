import QtQuick
import QtQuick.Controls

Button {
    id: control
    property color accent: "#2563EB"
    property bool primary: false
    property url iconSource: ""
    implicitHeight: 48
    font.pixelSize: 15
    font.weight: Font.DemiBold
    contentItem: Item {
        implicitWidth: buttonContent.implicitWidth
        implicitHeight: buttonContent.implicitHeight
        Row {
            id: buttonContent
            spacing: 5
            anchors.centerIn: parent
            Image {
                visible: control.iconSource.toString().length > 0
                source: control.iconSource
                width: 16
                height: 16
                sourceSize.width: 32
                sourceSize.height: 32
                anchors.verticalCenter: parent.verticalCenter
            }
            Text {
                text: control.text
                color: control.primary ? "white" : "#111827"
                font.pixelSize: control.font.pixelSize
                font.weight: control.font.weight
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
    background: Rectangle {
        radius: 12
        color: control.down ? Qt.darker(control.primary ? control.accent : "#FFFFFF", 1.06) : (control.primary ? control.accent : "#FFFFFF")
        border.color: control.primary ? control.accent : "#D8E1EE"
        opacity: control.enabled ? 1 : 0.45
    }
}
