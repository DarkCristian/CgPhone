import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import CgPhone

Page {
    id: page
    signal backRequested()

    background: Rectangle { color: Theme.background }

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth

        ColumnLayout {
            width: parent.width
            spacing: 12

            Item { height: 8 }

            Label {
                text: "Créditos"
                color: Theme.accent
                font.pixelSize: 21
                font.weight: Font.DemiBold
                Layout.alignment: Qt.AlignHCenter
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                implicitHeight: authorColumn.implicitHeight + 28
                radius: 14
                color: Theme.surface
                border.color: Theme.border

                ColumnLayout {
                    id: authorColumn
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 5

                    Label {
                        text: "Autor"
                        color: Theme.accent
                        font.pixelSize: 17
                        font.weight: Font.DemiBold
                    }
                    Label {
                        text: "Cristian Carroccio"
                        color: Theme.text
                        font.pixelSize: 15
                        font.weight: Font.DemiBold
                    }
                    Label {
                        text: "CgPhone 0.3.0 · Softphone para Windows y Linux"
                        color: Theme.muted
                        font.pixelSize: 11
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }
                }
            }

            Label {
                text: "Licencias"
                color: Theme.accent
                font.pixelSize: 17
                font.weight: Font.DemiBold
                Layout.leftMargin: 20
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                implicitHeight: licenseColumn.implicitHeight + 28
                radius: 14
                color: Theme.surface
                border.color: Theme.border

                ColumnLayout {
                    id: licenseColumn
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 10

                    Label {
                        text: "CgPhone"
                        color: Theme.text
                        font.weight: Font.DemiBold
                    }
                    Label {
                        text: "No se declaró una licencia pública para el código de CgPhone. Todos los derechos permanecen reservados por su autor."
                        color: Theme.secondary
                        font.pixelSize: 11
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: "#E7EDF5" }

                    Label {
                        text: "Qt 6"
                        color: Theme.text
                        font.weight: Font.DemiBold
                    }
                    Label {
                        text: "Disponible bajo licencias comerciales de Qt o, según cada módulo y forma de distribución, bajo LGPLv3/GPLv3. Qt es una marca de The Qt Company."
                        color: Theme.secondary
                        font.pixelSize: 11
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: "#E7EDF5" }

                    Label {
                        text: "PJPROJECT / PJSUA2"
                        color: Theme.text
                        font.weight: Font.DemiBold
                    }
                    Label {
                        text: "El backend SIP utilizado por el build portable se distribuye bajo GPLv2 o mediante una licencia comercial alternativa. La distribución de CgPhone debe respetar la opción de licencia aplicable."
                        color: Theme.secondary
                        font.pixelSize: 11
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }
                }
            }

            Label {
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                text: "Los textos completos y avisos de terceros deben acompañar cada distribución cuando corresponda."
                color: Theme.muted
                font.pixelSize: 10
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }

            SoftButton {
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                primary: true
                text: "Volver a Ajustes"
                onClicked: page.backRequested()
            }

            Item { height: 18 }
        }
    }
}
