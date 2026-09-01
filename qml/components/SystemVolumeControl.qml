import QtQuick
import CgPhone
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: panel
    implicitHeight: 72
    radius: 13
    color: Theme.surface
    border.color: Theme.border
    opacity: systemAudio.available ? 1.0 : 0.55
    ToolTip.visible: !systemAudio.available && hover.hovered
    ToolTip.text: "No se detectaron los dispositivos de audio del sistema"
    HoverHandler { id: hover }

    component RoundStepButton: Button {
        id: stepButton
        implicitWidth: 24; implicitHeight: 24
        contentItem: Text { text: stepButton.text; color: Theme.secondary; font.pixelSize: 14; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
        background: Rectangle { radius: 12; color: stepButton.down ? Theme.hover : Theme.surfaceAlt; border.color: Theme.blueBorder }
    }

    component RoundSlider: Slider {
        id: control
        from: 0; to: 100; stepSize: 1
        implicitHeight: 26
        background: Rectangle {
            x: control.leftPadding; y: control.topPadding + control.availableHeight / 2 - height / 2
            width: control.availableWidth; height: 6; radius: 3; color: Theme.track
            Rectangle { width: control.visualPosition * parent.width; height: parent.height; radius: 3; color: "#60A5FA" }
        }
        handle: Rectangle {
            x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
            y: control.topPadding + control.availableHeight / 2 - height / 2
            width: 16; height: 16; radius: 8; color: "#FFFFFF"; border.width: 3; border.color: Theme.accent
        }
    }

    ColumnLayout {
        anchors.fill: parent; anchors.margins: 6; spacing: 2
        RowLayout {
            Layout.fillWidth: true; spacing: 5
            Rectangle { width: 25; height: 25; radius: 12.5; color: Theme.softBlue; Text { anchors.centerIn: parent; text: "🔊"; font.pixelSize: 11 } }
            RoundStepButton { text: "−"; enabled: systemAudio.available; onClicked: systemAudio.outputVolume = Math.max(0, systemAudio.outputVolume - 5) }
            RoundSlider { Layout.fillWidth: true; enabled: systemAudio.available; value: systemAudio.outputVolume; onMoved: systemAudio.outputVolume = Math.round(value) }
            RoundStepButton { text: "+"; enabled: systemAudio.available; onClicked: systemAudio.outputVolume = Math.min(100, systemAudio.outputVolume + 5) }
            Label { text: systemAudio.outputVolume + "%"; color: Theme.muted; font.pixelSize: 9; Layout.preferredWidth: 28 }
        }
        RowLayout {
            Layout.fillWidth: true; spacing: 5
            Button {
                width: 25; height: 25
                ToolTip.visible: hovered
                ToolTip.text: systemAudio.microphoneMuted ? "Activar micrófono" : "Mutear micrófono"
                background: Rectangle { radius: 12.5; color: systemAudio.microphoneMuted ? "#FEE2E2" : Theme.softBlue; border.color: systemAudio.microphoneMuted ? "#FCA5A5" : "transparent" }
                contentItem: Text { text: systemAudio.microphoneMuted ? "🔇" : "🎙"; font.pixelSize: 11; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                onClicked: systemAudio.toggleMicrophoneMute()
            }
            RoundStepButton { text: "−"; enabled: systemAudio.available; onClicked: systemAudio.microphoneVolume = Math.max(0, systemAudio.microphoneVolume - 5) }
            RoundSlider { Layout.fillWidth: true; enabled: systemAudio.available; value: systemAudio.microphoneVolume; onMoved: systemAudio.microphoneVolume = Math.round(value) }
            RoundStepButton { text: "+"; enabled: systemAudio.available; onClicked: systemAudio.microphoneVolume = Math.min(100, systemAudio.microphoneVolume + 5) }
            Label { text: systemAudio.microphoneVolume + "%"; color: Theme.muted; font.pixelSize: 9; Layout.preferredWidth: 28 }
        }
    }
}
