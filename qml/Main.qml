import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window
    width: 344; height: 640; minimumWidth: 320; minimumHeight: 600
    visible: true; title: "CgPhone" + (appController.sipUser ? " · " + appController.sipUser : ""); color: "#F6FAFF"
    property bool allowExit: false
    onClosing: function(close) { if (!allowExit) { close.accepted = false; closeConfirm.open() } }

    property int selectedTab: 0
    property var tabs: ["Discador", "Historial"]
    Shortcut {
        sequence: "Escape"
        enabled: !appController.configurationMode && appController.inCall
        onActivated: appController.hangup()
    }
    Shortcut { sequence: "Num+0"; enabled: !appController.configurationMode; onActivated: appController.appendDigit("0") }
    Shortcut { sequence: "Num+1"; enabled: !appController.configurationMode; onActivated: appController.appendDigit("1") }
    Shortcut { sequence: "Num+2"; enabled: !appController.configurationMode; onActivated: appController.appendDigit("2") }
    Shortcut { sequence: "Num+3"; enabled: !appController.configurationMode; onActivated: appController.appendDigit("3") }
    Shortcut { sequence: "Num+4"; enabled: !appController.configurationMode; onActivated: appController.appendDigit("4") }
    Shortcut { sequence: "Num+5"; enabled: !appController.configurationMode; onActivated: appController.appendDigit("5") }
    Shortcut { sequence: "Num+6"; enabled: !appController.configurationMode; onActivated: appController.appendDigit("6") }
    Shortcut { sequence: "Num+7"; enabled: !appController.configurationMode; onActivated: appController.appendDigit("7") }
    Shortcut { sequence: "Num+8"; enabled: !appController.configurationMode; onActivated: appController.appendDigit("8") }
    Shortcut { sequence: "Num+9"; enabled: !appController.configurationMode; onActivated: appController.appendDigit("9") }
    Shortcut { sequence: "Num+*"; enabled: !appController.configurationMode; onActivated: appController.appendDigit("*") }

    header: ToolBar {
        height: 50
        background: Rectangle { color: "#FFFFFF"; border.color: "#E7EDF5" }
        RowLayout {
            anchors.fill: parent; anchors.leftMargin: 11; anchors.rightMargin: 11
            Rectangle {
                width: 28
                height: 28
                radius: 9
                color: "#E7F1FF"
                Image {
                    id: headerPhoneIcon
                    anchors.centerIn: parent
                    width: 18
                    height: 18
                    source: "qrc:/qt/qml/CgPhone/assets/icons/phone-blue.svg"
                }
                Text {
                    visible: headerPhoneIcon.status === Image.Error
                    anchors.centerIn: parent
                    text: "☎"
                    color: "#2563EB"
                    font.pixelSize: 16
                }
            }
            Label { text: "CgPhone"; color: "#111827"; font.pixelSize: 18; font.weight: Font.DemiBold }
            Rectangle {
                implicitWidth: sipBadgeText.implicitWidth + 14; implicitHeight: 28; radius: 10
                color: "#EDF5FF"; border.color: "#BCD5FB"
                Label { id: sipBadgeText; anchors.centerIn: parent; text: appController.sipUser || "SIN SIP"; color: "#2563EB"; font.pixelSize: 18; font.weight: Font.DemiBold }
            }
            Item { Layout.fillWidth: true }
            ToolButton {
                visible: !appController.configurationMode
                width: 30
                height: 30
                onClicked: appController.requestAdminConfiguration()
                background: Rectangle { radius: 9; color: "#EDF5FF" }
                contentItem: Item {
                    Image {
                        id: gearIcon
                        source: "qrc:/qt/qml/CgPhone/assets/icons/gear.svg"
                        width: 16
                        height: 16
                        anchors.centerIn: parent
                    }
                    Text {
                        visible: gearIcon.status === Image.Error
                        anchors.centerIn: parent
                        text: "⚙"
                        color: "#2563EB"
                        font.pixelSize: 16
                    }
                }
            }
            Rectangle {
                implicitWidth: Math.min(112, registrationLabel.implicitWidth + 26); height: 30; radius: 10; color: "#FFFFFF"; border.color: "#DBE4EE"
                Row { anchors.centerIn: parent; spacing: 5
                    Rectangle { width: 9; height: 9; radius: 4.5; anchors.verticalCenter: parent.verticalCenter; color: appController.configurationMode ? "#2563EB" : (appController.registered ? "#22C55E" : (/SIP [456]/.test(appController.registrationText) ? "#F59E0B" : "#94A3B8")) }
                    Label { id: registrationLabel; text: appController.configurationMode ? "Configuración" : appController.registrationText; color: "#475569"; font.pixelSize: 10; anchors.verticalCenter: parent.verticalCenter }
                }
            }
        }
    }

    StackLayout {
        visible: !appController.configurationMode
        anchors { left: parent.left; right: parent.right; top: parent.top; bottom: nav.top }
        currentIndex: window.selectedTab
        DialerPage { }
        HistoryPage { }
    }

    SettingsPage { anchors.fill: parent; visible: appController.configurationMode }

    TabBar {
        id: nav; visible: !appController.configurationMode; anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 58
        currentIndex: window.selectedTab
        onCurrentIndexChanged: window.selectedTab = currentIndex
        background: Rectangle { color: "#FFFFFF"; border.color: "#E1E8F2" }
        Repeater {
            model: window.tabs
            TabButton {
                text: modelData
                contentItem: Text { text: parent.text; color: parent.checked ? "#2563EB" : "#334155"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; font.pixelSize: 14; font.weight: parent.checked ? Font.DemiBold : Font.Normal }
                background: Rectangle { color: "transparent"; Rectangle { visible: parent.parent.checked; width: 34; height: 3; radius: 2; color: "#2563EB"; anchors.horizontalCenter: parent.horizontalCenter } }
            }
        }
    }

    Popup {
        id: toast; x: (parent.width-width)/2; y: Math.max(100, (parent.height-height)/2); width: Math.min(parent.width-32, 372); height: Math.max(62, toastText.implicitHeight + 30)
        padding: 14; closePolicy: Popup.NoAutoClose
        background: Rectangle {
            radius: 16; color: "#FFFFFF"; border.width: 1; border.color: "#BFD6F5"
            Rectangle { width: 5; radius: 2.5; color: "#2563EB"; anchors { left: parent.left; top: parent.top; bottom: parent.bottom; topMargin: 10; bottomMargin: 10 } }
        }
        contentItem: Label { id: toastText; color: "#1E293B"; font.pixelSize: 12; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; wrapMode: Text.Wrap }
        Timer { id: toastTimer; interval: toastText.text.length > 90 ? 5200 : 2800; onTriggered: toast.close() }
    }
    Connections {
        target: appController
        function onToast(message) { toastText.text=message; toast.open(); toastTimer.restart() }
        function onCallChanged() {
            if (appController.incoming) {
                window.show(); window.raise(); window.requestActivate(); incomingDialog.open()
            } else incomingDialog.close()
        }
    }
    Connections {
        target: systemAudio
        function onMicrophoneMutedChanged() { appController.setMuted(systemAudio.microphoneMuted) }
    }

    Dialog {
        id: incomingDialog
        anchors.centerIn: parent
        modal: true
        closePolicy: Popup.NoAutoClose
        width: Math.min(parent.width - 32, 380)
        topPadding: 18
        leftPadding: 14
        rightPadding: 14
        bottomPadding: 14
        background: Rectangle {
            radius: 18
            color: "#FFFFFF"
            border.width: 1
            border.color: "#D8E1EE"
        }
        contentItem: ColumnLayout {
            spacing: 12
            Label {
                Layout.fillWidth: true
                text: "Llamada entrante"
                color: "#111827"
                font.pixelSize: 18
                font.weight: Font.DemiBold
                horizontalAlignment: Text.AlignHCenter
            }
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: 66
                radius: 14
                color: "#EFF6FF"
                border.color: "#C9DDF8"
                Column {
                    anchors.centerIn: parent
                    width: parent.width - 20
                    spacing: 4
                    Label {
                        width: parent.width
                        text: appController.peer || "Asterisk"
                        color: "#111827"
                        font.pixelSize: 19
                        font.weight: Font.DemiBold
                        horizontalAlignment: Text.AlignHCenter
                        elide: Text.ElideRight
                    }
                    Label {
                        width: parent.width
                        text: "La central está llamando"
                        color: "#64748B"
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                Button {
                    Layout.fillWidth: true
                    implicitHeight: 40
                    text: "Atender"
                    onClicked: appController.answer()
                    background: Rectangle { radius: 12; color: "#16A34A" }
                    contentItem: Label {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.weight: Font.DemiBold
                    }
                }
                Button {
                    Layout.fillWidth: true
                    implicitHeight: 40
                    text: "Rechazar"
                    onClicked: appController.hangup()
                    background: Rectangle { radius: 12; color: "#DC2626" }
                    contentItem: Label {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.weight: Font.DemiBold
                    }
                }
            }
        }
    }

    Dialog {
        id: closeConfirm; anchors.centerIn: parent; modal: true; title: appController.configurationMode ? "Descartar cambios" : "Cerrar CgPhone"
        standardButtons: Dialog.Yes | Dialog.No
        contentItem: Label { text: appController.configurationMode ? "¿Cerrar sin guardar la configuración?" : "¿Realmente desea cerrar CgPhone? Dejará de recibir llamadas."; wrapMode: Text.WordWrap }
        onAccepted: { window.allowExit = true; Qt.quit() }
    }
}
