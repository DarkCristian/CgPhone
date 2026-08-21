import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import CgPhone

Page {
    id: page
    background: Rectangle { color: "#F6FAFF" }
    property var saved: appController.account()
    property var supportedCodecs: [
        {name:"Opus", id:"opus/48000/2"}, {name:"G.711 A-law", id:"PCMA/8000/1"},
        {name:"G.711 μ-law", id:"PCMU/8000/1"}, {name:"G.722", id:"G722/16000/1"}, {name:"GSM", id:"GSM/8000/1"}
    ]
    ListModel { id: codecModel }
    Component.onCompleted: {
        var active = page.saved.enabledCodecs || []
        for (var a=0; a<active.length; ++a)
            for (var i=0; i<supportedCodecs.length; ++i) if (supportedCodecs[i].id === active[a]) codecModel.append({name:supportedCodecs[i].name, codecId:supportedCodecs[i].id, codecEnabled:true})
        for (var j=0; j<supportedCodecs.length; ++j) {
            var found=false; for (var k=0;k<codecModel.count;++k) if (codecModel.get(k).codecId===supportedCodecs[j].id) found=true
            if (!found) codecModel.append({name:supportedCodecs[j].name, codecId:supportedCodecs[j].id, codecEnabled:false})
        }
    }
    function enabledCodecIds() { var out=[]; for (var i=0;i<codecModel.count;++i) if(codecModel.get(i).codecEnabled) out.push(codecModel.get(i).codecId); return out }
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
            Label { text: "Código de deslogueo de la central"; color: "#334155"; font.pixelSize: 12; Layout.leftMargin: 20 }
            TextField { id: logoutCode; Layout.fillWidth: true; Layout.margins: 20; Layout.topMargin: 0; Layout.bottomMargin: 0; placeholderText: "Ej. *02"; text: page.saved.logoutCode || "*02" }
            Label { text: "Se marca desde el menú del tray para cerrar la sesión del agente. Es configurable porque *02 no es universal en Asterisk/Neotel."; color: "#64748B"; font.pixelSize: 10; wrapMode: Text.WordWrap; Layout.fillWidth: true; Layout.leftMargin: 20; Layout.rightMargin: 20 }
            ToggleCard { id: alwaysVisibleToggle; Layout.fillWidth: true; Layout.margins: 20; Layout.topMargin: 0; Layout.bottomMargin: 0; text: "Siempre visible"; checked: page.saved.alwaysVisible || false }
            ToggleCard { id: startWithOsToggle; Layout.fillWidth: true; Layout.margins: 20; Layout.topMargin: 0; Layout.bottomMargin: 0; text: "Iniciar con el SO"; checked: page.saved.startWithOs || false }
            Label { text: "Codecs de audio"; color: "#2563EB"; font.pixelSize: 17; font.weight: Font.DemiBold; Layout.leftMargin: 20 }
            Label { text: "Activá los disponibles y ordenalos por prioridad."; color: "#64748B"; font.pixelSize: 11; Layout.leftMargin: 20 }
            Repeater { model: codecModel
                Rectangle {
                    required property int index; required property string name; required property bool codecEnabled
                    Layout.fillWidth: true; Layout.leftMargin: 20; Layout.rightMargin: 20; height: 48; radius: 12; color: "white"; border.color: "#DDE6F2"
                    RowLayout { anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 8
                        Text { text: "⋮⋮"; color: "#64748B" }
                        Text { text: parent.parent.name; color: "#111827"; Layout.fillWidth: true }
                        ToolButton { text: "↑"; enabled: index>0; onClicked: codecModel.move(index,index-1,1) }
                        ToolButton { text: "↓"; enabled: index<codecModel.count-1; onClicked: codecModel.move(index,index+1,1) }
                        Switch { checked: parent.parent.codecEnabled; onToggled: codecModel.setProperty(index,"codecEnabled",checked) }
                    }
                }
            }
            Label { text: "Grabación local"; color: "#2563EB"; font.pixelSize: 17; font.weight: Font.DemiBold; Layout.leftMargin: 20 }
            ToggleCard { id: recordingToggle; Layout.fillWidth: true; Layout.margins: 20; Layout.topMargin: 0; Layout.bottomMargin: 0; text: "Habilitar grabaciones locales"; checked: page.saved.localRecordingEnabled || false }
            TextField { id: recordingPath; visible: recordingToggle.checked; Layout.fillWidth: true; Layout.margins: 20; Layout.topMargin: 0; Layout.bottomMargin: 0; placeholderText: "Ruta de grabaciones"; text: page.saved.recordingPath || ""; rightPadding: 42
                ToolButton { anchors.right: parent.right; anchors.verticalCenter: parent.verticalCenter; text: "…"; onClicked: recordingFolder.open() }
            }
            ComboBox { id: recordingFormat; visible: recordingToggle.checked; Layout.fillWidth: true; Layout.leftMargin: 20; Layout.rightMargin: 20; model: ["wav","mp3"]; currentIndex: (page.saved.recordingFormat || "wav") === "mp3" ? 1 : 0 }
            RowLayout {
                Layout.fillWidth: true; Layout.margins: 20; Layout.topMargin: 6; spacing: 10
                SoftButton { Layout.fillWidth: true; text: "Descartar y volver"; onClicked: appController.discardConfiguration() }
                SoftButton { Layout.fillWidth: true; primary: true; text: "Guardar y volver"; onClicked: appController.saveAccount(user.text,password.text,server.text,proxy.text,proxyToggle.checked,logoutCode.text,alwaysVisibleToggle.checked,startWithOsToggle.checked,page.enabledCodecIds(),recordingToggle.checked,recordingPath.text,recordingFormat.currentText) }
            }
            Label { Layout.fillWidth: true; Layout.leftMargin: 20; Layout.rightMargin: 20; text: "La sesión normal probará el registro automáticamente al guardar."; wrapMode: Text.WordWrap; color: "#64748B"; font.pixelSize: 11 }
            Rectangle { Layout.fillWidth: true; Layout.margins: 20; height: 50; radius: 12; color: "#EAF3FF"; Text { anchors.centerIn: parent; text: "Compatible por SIP · Validar con Asterisk y Neotel"; color: "#334155"; font.pixelSize: 12 } }
            Item { height: 18 }
        }
    }
    FolderDialog { id: recordingFolder; title: "Elegir carpeta de grabaciones"; onAccepted: recordingPath.text = decodeURIComponent(selectedFolder.toString()).replace(/^file:\/\/\//, "") }
}
