import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    background: Rectangle { color: "#F6FAFF" }
    ColumnLayout {
        anchors.fill: parent; anchors.margins: 20; spacing: 14
        Label { text: "Historial"; color: "#2563EB"; font.pixelSize: 21; font.weight: Font.DemiBold; Layout.alignment: Qt.AlignHCenter }
        TextField { id: search; Layout.fillWidth: true; placeholderText: "Buscar llamadas…" }
        ListView {
            Layout.fillWidth: true; Layout.fillHeight: true; spacing: 8; clip: true
            model: appController.history
            delegate: Rectangle {
                required property string peer; required property string direction; required property string timestamp; required property string duration; required property bool missed
                width: ListView.view.width; height: 72; radius: 14; color: "white"; border.color: "#DDE6F2"
                visible: !search.text || peer.toLowerCase().includes(search.text.toLowerCase())
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
}

