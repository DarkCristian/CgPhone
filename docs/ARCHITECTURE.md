# Arquitectura

`AppController` expone estado y comandos a QML. `ISipEngine` define el límite con telefonía; `MockSipEngine` sirve para UI y pruebas, mientras `PjsipEngine` implementa PJSUA2. `SettingsStore` concentra persistencia y detección de elevación. `CallHistoryModel` ofrece el modelo de llamadas a la interfaz.

La edición Pro debe añadirse como módulos de capacidad, nunca como una segunda aplicación:

- `VideoSipCapability`: cámara, SDP/H.264 y renderizado Qt Multimedia.
- `GoogleWorkspaceConnector`: OAuth, Calendar y creación/apertura de Meet.
- `Microsoft365Connector`: Entra ID, Graph Calendar y enlaces de Teams.
- `LicenseService`: verifica capacidades firmadas; la interfaz consume capacidades, no el nombre del plan.

No se debe mezclar el motor SIP con APIs de reuniones. Un enlace de Meet/Teams no es una URI SIP ni un stream compatible con Asterisk.

