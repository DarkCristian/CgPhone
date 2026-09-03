# Arquitectura de CgPhone Free

CgPhone mantiene una arquitectura simple orientada a telefonía SIP de audio:

- `AppController` expone el estado y los comandos a QML.
- `ISipEngine` define el límite entre la interfaz y la telefonía.
- `MockSipEngine` permite probar la interfaz sin conectarse a una central.
- `PjsipEngine` implementa el backend SIP mediante PJSUA2.
- `SettingsStore` concentra la persistencia y la detección de elevación.
- `CallHistoryModel` ofrece el historial de llamadas a la interfaz.
- `SystemAudioController` administra los niveles del dispositivo de audio.
- `DiagnosticWindow` presenta el log SIP local bajo demanda.

## Principios

1. Mantener una cuenta SIP por terminal.
2. Separar la operación normal de la configuración administrativa.
3. Evitar telemetría y conexiones externas ajenas a la central configurada.
4. Mantener las dependencias al mínimo y documentar su procedencia.
5. Preservar compatibilidad entre Windows y Linux sin introducir funciones que
   comprometan la estabilidad de la línea Free.
6. Incorporar cambios mediante ramas y pull requests con pruebas de regresión.

El alcance del proyecto es el softphone SIP Free. Cualquier función futura debe
evaluarse por su utilidad para esa línea, costo de mantenimiento, superficie de
ataque y compatibilidad multiplataforma.
