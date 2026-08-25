# CgPhone

CgPhone es un softphone de escritorio para Windows y Linux construido con Qt 6/QML. Este repositorio implementa el MVP **Free**: una cuenta SIP por terminal, llamadas de audio, DND, autorespuesta, transferencia por interno, DTMF, historial y configuración mediante una sesión UAC temporal.

El icono oficial se integra en el ejecutable de Windows mediante un recurso ICO multirresolución y se reutiliza en la ventana, bandeja, instalador, menú Inicio y escritorio público. Linux instala el PNG y el archivo `cgphone.desktop` correspondientes.

## Estado real

- La compilación de desarrollo usa `MockSipEngine` de manera predeterminada.
- La compilación portable utiliza PJPROJECT/PJSUA2 con `-DCGPHONE_WITH_PJSIP=ON`.
- El proyecto está preparado para separar funciones Free/Pro mediante módulos; video, Google Workspace y Microsoft 365 no están incluidos en este MVP.
- Asterisk y Neotel deben validarse contra centrales reales. “SIP compatible” no garantiza compatibilidad con cada configuración de NAT, transporte, DTMF o transferencia.

## Funciones implementadas

- Discador y teclado telefónico.
- Prueba local de micrófono y headset desde el botón del encabezado del discador; el monitoreo se limita a CgPhone y se corta automáticamente al comenzar una llamada.
- Llamada, atención/rechazo, corte y contador de duración.
- DND: responde llamadas entrantes con `486 Busy Here` en el adaptador PJSIP.
- Autorespuesta: responde automáticamente con `200 OK`.
- Early media y anuncios de central: RTP remoto en `183 + SDP` se envía al
  headset y silencia el ringback local; el micrófono se conecta recién después
  del `200 OK`. CgPhone no genera localmente anuncios de campaña/cola.
- Transferencia ciega mediante SIP REFER y diálogo de interno.
- Historial persistente por usuario mediante `QSettings`.
- Una cuenta SIP con servidor y proxy opcional.
- Registro automático al iniciar cuando ya existe una cuenta válida.
- El engranaje abre un proceso separado con UAC; Guardar/Descartar lo cierra y el softphone normal nunca queda elevado.
- La contraseña SIP usa DPAPI con alcance de máquina en Windows.
- Área de notificación Qt: atender, abrir, desloguear y cerrar; minimizar oculta la ventana.
- Sliders redondeados que modifican el volumen real del headset y del micrófono
  predeterminados del sistema (Core Audio en Windows; PulseAudio/PipeWire vía
  `pactl` en Linux).
- Codecs mostrados como base de configuración futura.

## Compilar en Linux (Ubuntu/Debian)

Instalá Qt 6.5+, CMake 3.22+, Ninja y las dependencias de desarrollo de PJPROJECT. Los nombres exactos de los paquetes varían por distribución.

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/CgPhone
```

Para el motor SIP real:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCGPHONE_WITH_PJSIP=ON
cmake --build build
```

El módulo `FindPJSIP.cmake` espera que `pkg-config --libs libpjproject` funcione. Según cómo se compile PJPROJECT, puede ser necesario adaptar el nombre del paquete o las bibliotecas.

## Compilar en Windows

Para una guía desde cero, abrí
[`TUTORIAL-GITHUB-COMPILAR-WINDOWS.md`](TUTORIAL-GITHUB-COMPILAR-WINDOWS.md).

### Build automatizado recomendado

El workflow `.github/workflows/windows-portable.yml` crea un Windows x64
portable con MSYS2/MinGW, Qt 6 y PJPROJECT 2.17. Se ejecuta manualmente mediante
**Actions → Windows portable SIP → Run workflow** y publica el artefacto
`CgPhone-0.3.0-windows-x64-portable.zip`.

El mismo proceso puede ejecutarse en una consola MSYS2 MINGW64 con:

```bash
bash scripts/build-windows-portable-msys2.sh
```

Este build todavía debe validarse contra una central de laboratorio antes de
instalarse en boxes de campaña.

### Seguridad y compliance

Los artefactos actuales son exclusivamente de laboratorio. Cada build genera
un directorio `COMPLIANCE` con la revisión utilizada, versiones de dependencias
y hashes SHA-256. La política, los advisories evaluados y el gate obligatorio
para una versión productiva están documentados en
[`SECURITY.md`](SECURITY.md) y
[`docs/SECURITY_COMPLIANCE.md`](docs/SECURITY_COMPLIANCE.md).

PJPROJECT 2.17 sin parches posteriores bloquea una release final. La versión
productiva también requiere SBOM, escaneo de vulnerabilidades, CodeQL, análisis
de Defender, firma digital con timestamp y aceptación de Seguridad.

1. Instalá Qt 6.5+ para MSVC 2022 y CMake/Ninja.
2. Compilá PJPROJECT con PJSUA2 para el mismo compilador y arquitectura.
3. Proporcioná un archivo `libpjproject.pc` accesible mediante `PKG_CONFIG_PATH`, o reemplazá `FindPJSIP.cmake` por rutas importadas de tu build.
4. Configurá y compilá con los mismos comandos de CMake.
5. Usá `cmake --install build --prefix package` para generar el directorio desplegable de Qt.

El instalador Inno Setup de `installer/CgPhone.iss` requiere administrador,
instala por equipo en Program Files mediante `{autopf}` y crea un acceso directo
en `{commondesktop}` para todos los usuarios del dominio. No usa AppData/Roaming.

La DLL LAME aportada durante el prototipo es PE32/i386. No debe incluirse en un
build Qt x64. `RecordingEncoderLocator` busca primero una DLL del sistema y luego
la versión empaquetada, pero solo la acepta si puede cargarse y resolver
`beInitStream`; WAV queda disponible aunque MP3 no lo esté.

Windows y Linux deben compilarse nativamente en cada sistema; el script oficial de despliegue Qt no empaqueta Windows desde un host Linux.

## Configuración y administración

En Windows, el proceso normal nunca muestra Cuenta ni se eleva. El engranaje
inicia `CgPhone.exe --configure` mediante UAC; esa sesión solo muestra ajustes y
se cierra al Guardar o Descartar.

La cuenta SIP se guarda en `QSettings::SystemScope`, por lo que pertenece al box
y no al usuario de dominio. En Windows la contraseña se protege con DPAPI de
máquina. El archivo de configuración debe conservar ACL de solo lectura para
usuarios estándar. Linux todavía necesita integración con Secret Service/KWallet.

El badge superior y el tooltip del tray muestran el usuario SIP del box. La
opción administrativa "Iniciar con el SO" registra CgPhone para todos los
usuarios mediante HKLM Run en Windows y `/etc/xdg/autostart/CgPhone.desktop` en
Linux. Es autoarranque al iniciar una sesión gráfica, no un servicio previo al
login.

## Compatibilidad SIP a probar

1. Registro UDP, TCP y TLS.
2. RTP simétrico, STUN/TURN/ICE según la red.
3. DTMF RFC 2833/4733 y SIP INFO.
4. Codecs PCMA, PCMU, G.722, Opus y GSM.
5. Transferencia REFER directa y consultiva.
6. Re-INVITE, hold, cancelación y reconexión.
7. SRTP y validación de certificados.
8. Anuncios previos al bridge y early media de Asterisk/Neotel, sin audio local
   superpuesto y sin límite de duración impuesto por CgPhone.

## Licencias

Qt y PJPROJECT tienen obligaciones que dependen de cómo se distribuye el producto. PJPROJECT 2.17 se distribuye bajo GPL-2.0 o licencia comercial: no se puede lanzar una edición cerrada/Pro enlazada con esta biblioteca sin resolver la licencia. El desacoplamiento `ISipEngine` permite sustituir el backend sin reescribir la interfaz.

## Próxima iteración

1. Persistir historial en SQLite.
2. Credenciales en almacenes seguros del sistema.
3. Selección de micrófono, parlantes, ringtones y codecs reales.
4. Transporte TLS/SRTP, NAT traversal y logs diagnósticos.
5. Instaladores firmados y pruebas con Asterisk/Neotel.
6. Después: módulo Pro de video SIP y conectores de calendario.
