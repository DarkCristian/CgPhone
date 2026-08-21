import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    property string filter: "todas"
    background: Rectangle { color: "#F6FAFF" }
    ColumnLayout {
        anchors.fill: parent; anchors.margins: 20; spacing: 14
        RowLayout {
            Layout.fillWidth: true
            Item { Layout.fillWidth: true }
            Label { text: "Historial"; color: "#2563EB"; font.pixelSize: 21; font.weight: Font.DemiBold }
            Item { Layout.fillWidth: true }
            ToolButton { text: "🗑"; width: 32; height: 32; onClicked: clearConfirm.open(); background: Rectangle { radius: 16; color: "#EAF3FF"; border.color: "#C9DDF8" } }
        }
        TextField { id: search; Layout.fillWidth: true; placeholderText: "Buscar llamadas…"; leftPadding: 14; background: Rectangle { radius: 12; color: "white"; border.color: search.activeFocus ? "#60A5FA" : "#D8E1EE" } }
        RowLayout {
            Layout.fillWidth: true; spacing: 5
            Repeater {
                model: [{k:"todas",t:"Todas"},{k:"entrante",t:"Entrantes"},{k:"saliente",t:"Salientes"},{k:"perdida",t:"Perdidas"}]
                Button { Layout.fillWidth: true; text: modelData.t; checked: page.filter === modelData.k; checkable: true; onClicked: page.filter=modelData.k
                    background: Rectangle { radius: 12; color: parent.checked ? "#2563EB" : "white"; border.color: parent.checked ? "#2563EB" : "#D8E1EE" }
                    contentItem: Text { text: parent.text; color: parent.checked ? "white" : "#334155"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; font.pixelSize: 11 }
                }
            }
        }
        ListView {
            Layout.fillWidth: true; Layout.fillHeight: true; spacing: 8; clip: true
            model: appController.history
            delegate: Rectangle {
                required property string peer; required property string direction; required property string timestamp; required property string duration; required property bool missed
                property bool matches: (!search.text || peer.toLowerCase().includes(search.text.toLowerCase())) &&
                                       (page.filter === "todas" || (page.filter === "perdida" ? missed : direction === page.filter))
                width: ListView.view.width; height: matches ? 72 : 0; radius: 14; color: "white"; border.color: "#DDE6F2"
                visible: matches
                RowLayout { anchors.fill: parent; anchors.margins: 14
                    Text { text: direction === "saliente" ? "↗" : "↙"; color: missed ? "#DC2626" : (direction === "saliente" ? "#2563EB" : "#16A34A"); font.pixelSize: 22 }
                    ColumnLayout { Layout.fillWidth: true; spacing: 2
                        Text { text: peer; color: "#111827"; font.pixelSize: 15; font.weight: Font.DemiBold }
                        Text { text: timestamp; color: "#64748B"; font.pixelSize: 12 }
                    }
                    Text { text: duration; color: "#334155"; font.pixelSize: 13 }
                }
            }
            Label { anchors.centerIn: parent; visible: parent.count===0; text: "Todavía no hay llamadas"; color: "#64748B" }
        }
    }
    Dialog { id: clearConfirm; anchors.centerIn: parent; title: "Borrar historial"; modal: true; standardButtons: Dialog.Yes | Dialog.No
        contentItem: Label { text: "¿Querés borrar todo el historial de llamadas?"; wrapMode: Text.WordWrap }
        onAccepted: appController.history.clear()
    }
}
