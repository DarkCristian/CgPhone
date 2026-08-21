import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: window
    width: 360; height: 690; minimumWidth: 340; minimumHeight: 640
    visible: true; title: "CgPhone" + (appController.sipUser ? " · " + appController.sipUser : ""); color: "#F6FAFF"
    property bool allowExit: false
    onClosing: function(close) { if (!allowExit) { close.accepted = false; closeConfirm.open() } }

    property int selectedTab: 0
    property var tabs: ["Discador", "Historial"]
    Shortcut { sequence: "Shift+F12"; onActivated: appController.toggleDebugConsole() }

    header: ToolBar {
        height: 66
        background: Rectangle { color: "#FFFFFF"; border.color: "#E7EDF5" }
        RowLayout {
            anchors.fill: parent; anchors.leftMargin: 20; anchors.rightMargin: 20
            Rectangle { width: 30; height: 30; radius: 10; color: "#E9F2FF"; Text { anchors.centerIn: parent; text: "☎"; color: "#2563EB"; font.pixelSize: 17 } }
            Label { text: "CgPhone"; color: "#111827"; font.pixelSize: 18; font.weight: Font.DemiBold }
            Rectangle {
                implicitWidth: sipBadgeText.implicitWidth + 14; implicitHeight: 28; radius: 10
                color: "#EDF5FF"; border.color: "#BCD5FB"
                Label { id: sipBadgeText; anchors.centerIn: parent; text: appController.sipUser || "SIN SIP"; color: "#2563EB"; font.pixelSize: 18; font.weight: Font.DemiBold }
            }
            Item { Layout.fillWidth: true }
            ToolButton { visible: !appController.configurationMode; text: "⚙"; font.pixelSize: 17; onClicked: appController.requestAdminConfiguration() }
            Rectangle { width: 10; height: 10; radius: 5; color: appController.registered ? "#22C55E" : (/SIP [456]/.test(appController.registrationText) ? "#F59E0B" : "#94A3B8") }
            Label { Layout.maximumWidth: 94; text: appController.registrationText; color: "#64748B"; font.pixelSize: 10; elide: Text.ElideRight }
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
        id: nav; visible: !appController.configurationMode; anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 70
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
        id: toast; x: (parent.width-width)/2; y: parent.height-height-88; width: Math.min(parent.width-40, 360); height: 48
        background: Rectangle { radius: 12; color: "#1E293B" }
        contentItem: Label { id: toastText; color: "white"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; wrapMode: Text.Wrap }
        Timer { id: toastTimer; interval: 2600; onTriggered: toast.close() }
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

    Dialog {
        id: incomingDialog; anchors.centerIn: parent; modal: true; closePolicy: Popup.NoAutoClose
        title: "Llamada entrante"
        contentItem: ColumnLayout {
            spacing: 14
            Label { Layout.fillWidth: true; text: appController.peer || "Asterisk"; font.pixelSize: 20; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter }
            Label { Layout.fillWidth: true; text: "La central está llamando"; color: "#64748B"; horizontalAlignment: Text.AlignHCenter }
            RowLayout {
                Layout.fillWidth: true; spacing: 10
                Button { Layout.fillWidth: true; text: "Atender"; onClicked: appController.answer(); background: Rectangle { radius: 12; color: "#16A34A" }; contentItem: Label { text: parent.text; color: "white"; horizontalAlignment: Text.AlignHCenter } }
                Button { Layout.fillWidth: true; text: "Rechazar"; onClicked: appController.hangup(); background: Rectangle { radius: 12; color: "#DC2626" }; contentItem: Label { text: parent.text; color: "white"; horizontalAlignment: Text.AlignHCenter } }
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
