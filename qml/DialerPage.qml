import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CgPhone
import "components"

Page {
    id: page
    focus: true
    Component.onCompleted: forceActiveFocus()
    Keys.onPressed: function(event) {
        if (event.text && /^[0-9*#]$/.test(event.text)) { appController.appendDigit(event.text); event.accepted = true }
        else if (event.key === Qt.Key_Backspace) { appController.backspace(); event.accepted = true }
        else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) { if (!appController.inCall) appController.call(); event.accepted = true }
    }
    background: Rectangle { color: "#F6FAFF" }
    property var digits: ["1","2","3","4","5","6","7","8","9","*","0","#"]

    ColumnLayout {
        anchors.fill: parent; anchors.margins: 12; spacing: 6
        Item {
            Layout.fillWidth: true; implicitHeight: 32
            Label { anchors.centerIn: parent; text: "Discador"; color: "#2563EB"; font.pixelSize: 18; font.weight: Font.DemiBold }
            ToolButton {
                anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter
                width: 30; height: 30; text: appController.localAudioMonitor ? "■" : "↙"
                enabled: !appController.inCall
                ToolTip.visible: hovered; ToolTip.text: appController.localAudioMonitor ? "Detener prueba de audio" : "Probar micrófono y headset"
                background: Rectangle { radius: 10; color: parent.down ? "#DCEBFF" : "#EDF5FF"; border.color: appController.localAudioMonitor ? "#22C55E" : "#BCD5FB" }
                contentItem: Text { text: parent.text; color: appController.localAudioMonitor ? "#16A34A" : "#2563EB"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; font.pixelSize: 15 }
                onClicked: appController.localAudioMonitor ? appController.toggleLocalAudioMonitor() : audioTestConfirm.open()
            }
        }
        Rectangle {
            Layout.fillWidth: true; implicitHeight: 72; radius: 14; color: "#EFF6FF"; border.color: "#C9DDF8"
            Column { anchors.centerIn: parent; spacing: 3
                Text { anchors.horizontalCenter: parent.horizontalCenter; text: appController.inCall ? appController.peer : (appController.dialedNumber || "Ingresá un número"); color: "#111827"; font.pixelSize: 20; font.weight: Font.Bold }
                Text { anchors.horizontalCenter: parent.horizontalCenter; text: appController.callStatus; color: appController.held ? "#D97706" : (appController.callStatus === "Disponible" ? "#16A34A" : "#2563EB"); font.pixelSize: 12; font.weight: Font.DemiBold }
                Text { anchors.horizontalCenter: parent.horizontalCenter; visible: appController.inCall; text: appController.duration; color: appController.held ? "#D97706" : "#111827"; font.pixelSize: 12; font.weight: Font.DemiBold }
            }
        }
        GridLayout {
            columns: 3; columnSpacing: 6; rowSpacing: 5; Layout.fillWidth: true
            Repeater {
                model: page.digits
                SoftButton { Layout.fillWidth: true; implicitHeight: 40; text: modelData; font.pixelSize: 20; onClicked: appController.appendDigit(modelData) }
            }
        }
        RowLayout {
            Layout.fillWidth: true; spacing: 6
            SoftButton { visible: !appController.incoming && !appController.inCall; Layout.fillWidth: true; implicitHeight: 38; text: "Borrar"; iconSource: "qrc:/qt/qml/CgPhone/assets/icons/backspace.svg"; onClicked: appController.backspace() }
            SoftButton { visible: appController.incoming; Layout.fillWidth: true; implicitHeight: 38; primary: true; accent: "#16A34A"; text: "Atender"; onClicked: appController.answer() }
            SoftButton { visible: appController.inCall && !appController.incoming; Layout.fillWidth: true; implicitHeight: 38; text: appController.held ? "▶  Retomar" : "Ⅱ  Hold"; primary: appController.held; accent: "#F59E0B"; onClicked: appController.toggleHold() }
            SoftButton { Layout.fillWidth: true; implicitHeight: 38; primary: true; accent: appController.inCall ? "#DC2626" : "#2563EB"; text: appController.incoming ? "Rechazar" : (appController.inCall ? "Cortar" : "Llamar"); iconSource: appController.incoming ? "" : "qrc:/qt/qml/CgPhone/assets/icons/phone-white.svg"; onClicked: appController.inCall ? appController.hangup() : appController.call() }
        }
        RowLayout {
            Layout.fillWidth: true; spacing: 6
            ToggleCard { Layout.fillWidth: true; text: "No molestar"; checked: appController.dnd; onToggled: appController.dnd=checked }
            ToggleCard { Layout.fillWidth: true; text: "Autorespuesta"; checked: appController.autoAnswer; onToggled: appController.autoAnswer=checked }
        }
        RowLayout {
            Layout.fillWidth: true; spacing: 6
            SoftButton { Layout.fillWidth: true; implicitHeight: 38; text: "Transferir"; iconSource: "qrc:/qt/qml/CgPhone/assets/icons/transfer.svg"; enabled: appController.inCall; onClicked: transferDialog.open() }
            SoftButton { Layout.fillWidth: true; implicitHeight: 38; text: appController.recording ? "■  Detener" : "●  Grabar"; enabled: appController.localRecordingEnabled && appController.inCall && !appController.incoming; primary: appController.recording; accent: "#DC2626"; ToolTip.visible: hovered && !appController.localRecordingEnabled; ToolTip.text: "Habilitá la grabación local en Configuración"; onClicked: appController.toggleRecording() }
        }
        SystemVolumeControl { Layout.fillWidth: true }
        Item { Layout.fillHeight: true }
    }

    Dialog {
        id: transferDialog; anchors.centerIn: parent; width: Math.min(parent.width-48, 370); modal: true; title: "Transferir llamada"
        standardButtons: Dialog.NoButton
        background: Rectangle { radius: 18; color: "white"; border.color: "#D8E1EE" }
        contentItem: ColumnLayout {
            spacing: 12
            Label { text: "Interno a transferir"; color: "#111827" }
            TextField { id: extension; Layout.fillWidth: true; placeholderText: "Ej. 204"; inputMethodHints: Qt.ImhDialableCharactersOnly }
            RowLayout {
                Layout.fillWidth: true; spacing: 10
                SoftButton { Layout.fillWidth: true; text: "Cancelar"; onClicked: transferDialog.close() }
                SoftButton { Layout.fillWidth: true; primary: true; text: "Transferir"; onClicked: { appController.transfer(extension.text); extension.clear(); transferDialog.close() } }
            }
        }
    }

    Dialog {
        id: audioTestConfirm; anchors.centerIn: parent; width: Math.min(parent.width-48, 370); modal: true; title: "Prueba de audio local"
        standardButtons: Dialog.Yes | Dialog.No
        contentItem: Label { text: "Vas a escuchar tu micrófono por el headset. Usalo sólo para comprobar audio y evitá altavoces para no generar acople."; wrapMode: Text.WordWrap }
        onAccepted: appController.toggleLocalAudioMonitor()
    }
}
