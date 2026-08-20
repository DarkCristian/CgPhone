import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CgPhone

Page {
    id: page
    background: Rectangle { color: "#F6FAFF" }
    property var saved: appController.account()
    ScrollView {
        anchors.fill: parent; contentWidth: availableWidth
        ColumnLayout {
            width: parent.width; spacing: 12
            Item { height: 6 }
            Label { text: "Cuenta"; color: "#2563EB"; font.pixelSize: 21; font.weight: Font.DemiBold; Layout.alignment: Qt.AlignHCenter }
            Label { text: "🛡  Solo administrador"; color: "#334155"; font.pixelSize: 14; Layout.leftMargin: 20 }
            Label { text: "Cuenta SIP"; color: "#2563EB"; font.pixelSize: 17; font.weight: Font.DemiBold; Layout.leftMargin: 20 }
            TextField { id: user; Layout.fillWidth: true; Layout.margins: 20; Layout.topMargin: 0; Layout.bottomMargin: 0; placeholderText: "Usuario SIP"; text: page.saved.user || "" }
            TextField { id: password; Layout.fillWidth: true; Layout.margins: 20; Layout.topMargin: 0; Layout.bottomMargin: 0; placeholderText: "Contraseña"; echoMode: TextInput.Password; text: page.saved.password || "" }
            TextField { id: server; Layout.fillWidth: true; Layout.margins: 20; Layout.topMargin: 0; Layout.bottomMargin: 0; placeholderText: "Servidor, ej. pbx.empresa.com"; text: page.saved.server || "" }
            ToggleCard { id: proxyToggle; Layout.fillWidth: true; Layout.margins: 20; Layout.topMargin: 0; Layout.bottomMargin: 0; text: "Usar proxy"; checked: page.saved.proxyEnabled || false }
            TextField { id: proxy; visible: proxyToggle.checked; Layout.fillWidth: true; Layout.margins: 20; Layout.topMargin: 0; Layout.bottomMargin: 0; placeholderText: "Proxy, ej. sip.empresa.com:5060"; text: page.saved.proxy || "" }
            TextField { id: logoutCode; Layout.fillWidth: true; Layout.margins: 20; Layout.topMargin: 0; Layout.bottomMargin: 0; placeholderText: "Código de deslogueo"; text: page.saved.logoutCode || "*02" }
            ToggleCard { id: alwaysVisibleToggle; Layout.fillWidth: true; Layout.margins: 20; Layout.topMargin: 0; Layout.bottomMargin: 0; text: "Siempre visible"; checked: page.saved.alwaysVisible || false }
            ToggleCard { id: startWithOsToggle; Layout.fillWidth: true; Layout.margins: 20; Layout.topMargin: 0; Layout.bottomMargin: 0; text: "Iniciar con el SO"; checked: page.saved.startWithOs || false }
            Label { text: "Codecs de audio"; color: "#2563EB"; font.pixelSize: 17; font.weight: Font.DemiBold; Layout.leftMargin: 20 }
            Repeater { model: ["Opus", "G.711 A-law", "G.711 μ-law", "G.722", "GSM"]
                Rectangle { Layout.fillWidth: true; Layout.leftMargin: 20; Layout.rightMargin: 20; height: 45; radius: 10; color: "white"; border.color: "#DDE6F2"; Text { anchors.verticalCenter: parent.verticalCenter; anchors.left: parent.left; anchors.leftMargin: 14; text: modelData; color: "#111827" } }
            }
            RowLayout {
                Layout.fillWidth: true; Layout.margins: 20; Layout.topMargin: 6; spacing: 10
                SoftButton { Layout.fillWidth: true; text: "Descartar y volver"; onClicked: appController.discardConfiguration() }
                SoftButton { Layout.fillWidth: true; primary: true; text: "Guardar y volver"; onClicked: appController.saveAccount(user.text,password.text,server.text,proxy.text,proxyToggle.checked,logoutCode.text,alwaysVisibleToggle.checked,startWithOsToggle.checked) }
            }
            Label { Layout.fillWidth: true; Layout.leftMargin: 20; Layout.rightMargin: 20; text: "La sesión normal probará el registro automáticamente al guardar."; wrapMode: Text.WordWrap; color: "#64748B"; font.pixelSize: 11 }
            Rectangle { Layout.fillWidth: true; Layout.margins: 20; height: 50; radius: 12; color: "#EAF3FF"; Text { anchors.centerIn: parent; text: "Compatible por SIP · Validar con Asterisk y Neotel"; color: "#334155"; font.pixelSize: 12 } }
            Item { height: 18 }
        }
    }
}
