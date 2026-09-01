import QtQuick
import CgPhone
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: page
    property string filter: "todas"
    property string recallTarget: ""
    property string recallLabel: ""
    function requestRecall(peer, dialTarget) {
        var fallback = /^[0-9*#]+$/.test(peer) ? peer : ""
        recallTarget = dialTarget || fallback
        recallLabel = peer
        recallConfirm.open()
    }
    function confirmRecall() {
        if (!recallTarget.length) return
        appController.redial(recallTarget)
        recallConfirm.close()
    }
    background: Rectangle { color: Theme.background }
    ColumnLayout {
        anchors.fill: parent; anchors.margins: 20; spacing: 14
        RowLayout {
            Layout.fillWidth: true
            Item { Layout.fillWidth: true }
            Label { text: "Historial"; color: Theme.accent; font.pixelSize: 21; font.weight: Font.DemiBold }
            Item { Layout.fillWidth: true }
            ToolButton { text: "🗑"; width: 32; height: 32; onClicked: clearConfirm.open(); background: Rectangle { radius: 16; color: Theme.softBlue; border.color: Theme.blueBorder } }
        }
        TextField { id: search; Layout.fillWidth: true; placeholderText: "Buscar llamadas…"; leftPadding: 14; background: Rectangle { radius: 12; color: Theme.surface; border.color: search.activeFocus ? "#60A5FA" : Theme.borderStrong } }
        RowLayout {
            Layout.fillWidth: true; spacing: 5
            Repeater {
                model: [{k:"todas",t:"Todas"},{k:"entrante",t:"Entrantes"},{k:"saliente",t:"Salientes"},{k:"perdida",t:"Perdidas"}]
                Button { Layout.fillWidth: true; text: modelData.t; checked: page.filter === modelData.k; checkable: true; onClicked: page.filter=modelData.k
                    background: Rectangle { radius: 12; color: parent.checked ? Theme.accent : "white"; border.color: parent.checked ? Theme.accent : Theme.borderStrong }
                    contentItem: Text { text: parent.text; color: parent.checked ? "white" : Theme.textSecondary; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; font.pixelSize: 11 }
                }
            }
        }
        ListView {
            Layout.fillWidth: true; Layout.fillHeight: true; spacing: 8; clip: true
            model: appController.history
            delegate: Rectangle {
                required property string peer; required property string direction; required property string timestamp; required property string duration; required property bool missed; required property string dialTarget
                property bool matches: (!search.text || peer.toLowerCase().includes(search.text.toLowerCase())) &&
                                       (page.filter === "todas" || (page.filter === "perdida" ? missed : direction === page.filter))
                width: ListView.view.width
                height: matches ? 72 : 0
                radius: 14
                color: cardHover.hovered ? Theme.hover : "white"
                border.color: cardHover.hovered ? "#9FC5F8" : Theme.border
                visible: matches
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    Item {
                        width: 30; height: 40
                        Text {
                            anchors.centerIn: parent
                            text: direction === "saliente" ? "↗" : "↙"
                            color: missed ? "#DC2626" : (direction === "saliente" ? Theme.accent : "#16A34A")
                            font.pixelSize: 22
                        }
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2
                        Text { text: peer; color: Theme.text; font.pixelSize: 15; font.weight: Font.DemiBold }
                        Text { text: timestamp; color: Theme.muted; font.pixelSize: 12 }
                    }
                    Text { text: duration; color: Theme.textSecondary; font.pixelSize: 13 }
                }
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    cursorShape: Qt.PointingHandCursor
                    onDoubleClicked: page.requestRecall(peer, dialTarget)
                }
                HoverHandler { id: cardHover }
                ToolTip.visible: cardHover.hovered
                ToolTip.text: "Doble clic para rellamar"
            }
            Label { anchors.centerIn: parent; visible: parent.count===0; text: "Todavía no hay llamadas"; color: Theme.muted }
        }
    }
    Dialog {
        id: recallConfirm; anchors.centerIn: parent; modal: true; width: Math.min(parent.width-48, 370)
        title: "Rellamar"; standardButtons: Dialog.NoButton; closePolicy: Popup.CloseOnEscape
        background: Rectangle { radius: 18; color: Theme.surface; border.color: Theme.borderStrong }
        contentItem: ColumnLayout {
            spacing: 14
            Label {
                Layout.fillWidth: true
                text: page.recallTarget.length
                      ? "¿Querés rellamar a " + page.recallLabel + " (" + page.recallTarget + ")?"
                      : "Esta llamada antigua no contiene un interno rellamable."
                wrapMode: Text.WordWrap; horizontalAlignment: Text.AlignHCenter
                color: Theme.text; font.pixelSize: 14
            }
            RowLayout {
                Layout.fillWidth: true; spacing: 10
                Button {
                    Layout.fillWidth: true; text: "No"; onClicked: recallConfirm.close()
                    background: Rectangle { radius: 12; color: Theme.surface; border.color: Theme.borderStrong }
                }
                Button {
                    Layout.fillWidth: true; text: "Sí"; enabled: page.recallTarget.length > 0
                    onClicked: page.confirmRecall()
                    background: Rectangle { radius: 12; color: parent.enabled ? Theme.accent : Theme.disabled }
                    contentItem: Text { text: parent.text; color: Theme.onAccent; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                }
            }
        }
        Shortcut { sequence: "Return"; enabled: recallConfirm.opened && page.recallTarget.length > 0; onActivated: page.confirmRecall() }
        Shortcut { sequence: "Enter"; enabled: recallConfirm.opened && page.recallTarget.length > 0; onActivated: page.confirmRecall() }
        Shortcut { sequence: "Escape"; enabled: recallConfirm.opened; onActivated: recallConfirm.close() }
    }

    Dialog { id: clearConfirm; anchors.centerIn: parent; title: "Borrar historial"; modal: true; standardButtons: Dialog.Yes | Dialog.No
        contentItem: Label { text: "¿Querés borrar todo el historial de llamadas?"; wrapMode: Text.WordWrap }
        onAccepted: appController.history.clear()
    }
}
