# CgPhone Free

## Release actual: v0.3.2-rc.1

[Descargar Windows y Linux](https://github.com/DarkCristian/CgPhone/releases/tag/v0.3.2-rc.1). Windows RC1 conserva los binarios exactos del build #39 validados por el mantenedor. Linux es EXPERIMENTAL para Debian 13 amd64, con .deb y .tar.xz; el archivo comprimido requiere bibliotecas del sistema y no es universal para todas las distribuciones.

[Workflow de publicación y pruebas Linux](https://github.com/DarkCristian/CgPhone/actions/runs/34656431755). Linux pasó compilación con PJSUA2, ctest, instalación en Debian limpio, arranque QML como usuario normal y comparación del ejecutable del tar.xz con el instalado. No se probaron centrales SIP, hardware de audio, Wayland ni tray real en Linux. Se observaron avisos QML de tamaño implícito pendientes de revisión visual.

La contraseña SIP Linux se guarda sin cifrar. No hay elevación gráfica integrada para Ajustes; usar únicamente cuentas de laboratorio. MP3 no está soportado en esta preview; usar WAV. Consultar [limitaciones e instalación Linux](docs/LINUX_EXPERIMENTAL.md).

Se adjuntan SBOM distintos por plataforma y manifiestos SHA-256. El SBOM Linux inventaría archivos y entorno de build; no certifica ausencia de CVE ni resuelve la revisión de licencias. Defender corresponde sólo a Windows #39. RC1 sigue sin Authenticode: SmartScreen puede advertir. Atestaciones y hashes no sustituyen esa firma.

### Hashes de los paquetes y SBOM de RC1

```text
e6aa46ba4ff7654d3da65ac9f375d35447f8b6d4ede7e8ac704e2c30c4fa21cd  CgPhone-0.3.2-rc.1-linux-experimental-amd64.tar.xz
aa4b55a9e524f228a34d84b9814824722e3b0822a39ccf2a7bd6c488bf02f0d5  CgPhone-0.3.2-sbom.spdx.json
2c3417ba6f7029d800ba974dd70612a77fb2b70b1e6dccd997809dfb2bbf6b6e  CgPhone-0.3.2-windows-x64-portable.zip
974ea9a0baad83e4c47300ae71668f3bcc75f7c36cce2437d46e01d1682be0b8  CgPhone-Setup-0.3.2-x64.exe
217419f0d7c32c3836daa7d8504288b187cad5c4d6a4342b3c6652fbb10827ca  CgPhone-linux-sbom.spdx.json
cf75890d85d160376a5cbc1137056b070454526dd05083dcfc33a1850dcf880c  cgphone_0.3.2-rc.1_linux-experimental_amd64.deb
```

[Manifiesto completo SHA256SUMS.txt](https://github.com/DarkCristian/CgPhone/releases/download/v0.3.2-rc.1/SHA256SUMS.txt). Los paquetes anteriores se conservan como evidencia histórica; no mezclar sus hashes con Linux RC1.

## Evidencia histórica v0.3.2-beta.39 — minimización en barra de tareas

CgPhone permanece en la barra de tareas al minimizar. El system tray conserva sus acciones y permite restaurar la misma instancia. Las llamadas entrantes restauran la ventana.

El mantenedor confirmó la validación funcional del build #39, incluidas las pruebas solicitadas de minimización, restauración y llamadas. Se conserva la base funcional y los parches de seguridad de beta.38. El diagnóstico SIP y la confirmación de cierre mantienen su comportamiento.

Se publican sin recompilar los artefactos del [build #39](https://github.com/DarkCristian/CgPhone/actions/runs/34619531060), commit fuente `7d1ec349afe4a8d52555cc6bc16a13d1308938cf`. La versión interna y los nombres de los binarios siguen siendo 0.3.2; el tag beta.39 y estos hashes identifican esta entrega.

Evidencia propia de esta compilación: SBOM SPDX 2.3, manifiesto SHA-256, atestaciones de GitHub Actions y reporte Microsoft Defender CLEAN, 0 detecciones. Los hashes y análisis de versiones anteriores no se atribuyen a estos binarios. La validación Trellix previa corresponde al entorno y políticas donde fue realizada; no constituye una certificación universal de este build. Ante una alerta, verificar hash y evento con Seguridad antes de autorizar una excepción específica. No desactivar la protección ni asumir que ejecutar desde Program Files evita las políticas.

Todavía sin firma Authenticode: SmartScreen puede advertir sobre el publicador desconocido. Las atestaciones no sustituyen esa firma. No se publican registros ni datos de equipos o empresas.

### SHA-256 del build #39

```text
974ea9a0baad83e4c47300ae71668f3bcc75f7c36cce2437d46e01d1682be0b8  CgPhone-Setup-0.3.2-x64.exe
2c3417ba6f7029d800ba974dd70612a77fb2b70b1e6dccd997809dfb2bbf6b6e  CgPhone-0.3.2-windows-x64-portable.zip
aa4b55a9e524f228a34d84b9814824722e3b0822a39ccf2a7bd6c488bf02f0d5  CgPhone-0.3.2-sbom.spdx.json
cf4b3915f54a6084f08d6c3c0879862ab8c88a479c8d43c3f41b46806fc13ac8  Microsoft-Defender-scan-report.txt
```

Descargar instalador, portable y evidencias en [v0.3.2-beta.39](https://github.com/DarkCristian/CgPhone/releases/tag/v0.3.2-beta.39).

CgPhone Free es un softphone SIP de escritorio construido con Qt 6/QML para
Windows y Linux experimental. El objetivo es ofrecer una aplicación simple,
liviana y transparente para registrar una cuenta SIP, realizar y recibir
llamadas y disponer de herramientas básicas de telefonía sin telemetría.

> **Estado actual:** `v0.3.2-rc.1`, candidata Windows con los binarios validados del build #39 y paquetes Linux experimentales para Debian 13 amd64. No es una aprobación productiva. Las validaciones de Windows no se atribuyen a Linux.

## Funciones actuales

Funciones validadas en Windows. La edición Linux experimental requiere su propia validación; consultar las limitaciones de plataforma antes de usarla.

### Operación de llamadas

- Una cuenta SIP por terminal mediante PJPROJECT/PJSUA2, con registro automático y estado visible.
- Llamadas entrantes y salientes con ringtone, identificación, popup para atender o rechazar y control para colgar.
- Marcación DTMF desde el pad gráfico y el teclado numérico físico.
- Early media y reproducción de anuncios recibidos desde la central antes de establecer la llamada.
- **Hold con recordatorios operacionales:** mientras una llamada permanece en espera, CgPhone muestra un aviso emergente cada 30 segundos para reducir olvidos y facilitar que el operador retome la comunicación.
- **Mute con advertencia persistente:** si el micrófono se silencia desde CgPhone, aparece un aviso emergente cada 3 segundos. Este control no puede detectar el mute físico o analógico realizado directamente desde una vincha o headset.
- Transferencia ciega mediante SIP REFER, con confirmación visible del interno ingresado antes de ejecutar la transferencia.
- DND con respuesta `486 Busy Here`.
- Autorespuesta con `200 OK`.
- Grabación local en formato WAV o Mp3 cuando el administrador habilita la función desde Ajustes.

### Audio y experiencia de uso

- Control integrado del volumen de salida y del nivel del micrófono, conectado al subsistema de audio de Windows.
- Botón de prueba local para verificar parlantes/auriculares y micrófono antes de operar.
- Interfaz compacta inspirada en Fluent UI, optimizada para escritorios de operación.
- Estados de llamada y registro diferenciados visualmente.
- Avisos emergentes no intrusivos para hold, mute y acciones que requieren atención.
- Confirmación antes de cerrar CgPhone para evitar finalizar accidentalmente la aplicación y dejar de recibir llamadas.

### Administración y continuidad operativa

- Ajustes administrativos accesibles únicamente mediante elevación UAC.
- Contraseña SIP protegida con DPAPI de máquina en Windows.
- Inicio automático con Windows, opción de mantener la ventana siempre visible y funcionamiento en el system tray.
- Al minimizar, CgPhone permanece en la barra de tareas, continúa registrado y disponible para recibir llamadas. El icono y las acciones del system tray siguen disponibles; «Abrir CgPhone» restaura la misma ventana. Una llamada entrante también restaura la ventana.
- Historial persistente con filtros de llamadas entrantes, salientes y perdidas, además de rellamada y borrado.
- Accesos directos instalados en el escritorio público y el menú Inicio para que estén disponibles a todos los usuarios del equipo.
- Diagnóstico SIP mediante `Shift+F12`, con lectura del log nativo de PJSIP; puede mostrarse u ocultarse sin cerrar CgPhone.
- Pantalla de Créditos con autor y licencias de los componentes utilizados.
- Desinstalador con opción explícita para conservar o eliminar la cuenta SIP y la configuración local, sin borrar grabaciones ni documentos personales.
- Opcion para Atender llamadas, maximizar y desloguearse de la central en las opciones del system tray.

El alcance mantenido se concentra en telefonía SIP de audio con una cuenta por
terminal. CgPhone no incorpora telemetría y no puede supervisar controles
eléctricos o analógicos propios del headset. Las nuevas funciones deberán
preservar esta base simple, auditable y orientada al uso operacional.

## Descargar y probar

La release [v0.3.2-rc.1](https://github.com/DarkCristian/CgPhone/releases/tag/v0.3.2-rc.1) reúne:

- Windows: instalador EXE y portable ZIP exactos del build #39.
- Linux: paquete .deb y archivo .tar.xz experimentales para Debian 13 amd64. El tar.xz requiere dependencias del sistema.
- SBOM separados por plataforma, hashes SHA-256 y reportes con su alcance explícito.

Verificar cada archivo contra el [manifiesto combinado](https://github.com/DarkCristian/CgPhone/releases/download/v0.3.2-rc.1/SHA256SUMS.txt). Los hashes históricos de beta.38/39 sólo identifican sus archivos; Windows RC1 reutiliza exactamente los de beta.39.

### Aviso de Microsoft SmartScreen

El instalador Windows de RC1 todavía no posee una firma Authenticode pública.
Windows puede mostrar el aviso **“Windows protegió su PC”**. La presencia o
ausencia de ese aviso no reemplaza la verificación del SHA-256.

Para verificar el archivo en PowerShell:

```powershell
Get-FileHash .\CgPhone-Setup-0.3.2-x64.exe -Algorithm SHA256
```

El resultado debe coincidir exactamente con el hash del manifiesto de RC1. Esta
beta está destinada a pruebas controladas; no es una release aprobada para
producción.

## Validación funcional de la beta

La matriz funcional se validó manualmente en Windows x64 con beta.35, se
replicó en beta.37 y volvió a completarse satisfactoriamente en beta.38 contra
centrales **Asterisk y Neotel**.
En ambos entornos se comprobaron:

- instalación, desinstalación y limpieza opcional de la configuración;
- configuración de la cuenta desde Ajustes con elevación UAC;
- registro SIP y recuperación después de una interrupción de red;
- llamadas salientes consecutivas y llamadas entrantes;
- atender, rechazar, DND y autorespuesta;
- DTMF, hold/unhold, mute y transferencia;
- audio, prueba local, volumen y nivel de micrófono;
- historial, grabación WAV, bandeja del sistema y autoarranque;
- apertura y ocultamiento del diagnóstico mediante `Shift+F12`;
- revisión del log SIP para evitar la exposición de contraseñas o secretos.

La validación fue completada sobre los artefactos exactos de beta.35, beta.37 y beta.38.
Debe repetirse en cada release y no sustituye un análisis de seguridad,
compatibilidad o carga.

## Integridad, procedencia y análisis

El workflow de la rama `security/artifact-attestations` fue validado en el
[run 33923023903](https://github.com/DarkCristian/CgPhone/actions/runs/33923023903).
Genera `SHA256SUMS.txt` y atestaciones de procedencia para el instalador, el
portable y el SBOM. Los hashes del manifiesto coincidieron con los artefactos
generados. Estas atestaciones permiten comprobar el origen del build mediante
`gh attestation verify`, pero no reemplazan Authenticode ni evitan SmartScreen.

El [workflow #38](https://github.com/DarkCristian/CgPhone/actions/runs/34388740867)
validó el gate de Microsoft Defender sobre la distribución final: instalador,
runtime portable, ZIP y árbol instalado. El resultado fue `CLEAN`, con cero
detecciones, usando motor 1.1.26080.3 e inteligencia 1.459.130.0. El reporte
sanitizado forma parte de `SHA256SUMS.txt` y de la atestación. Este resultado
corresponde sólo al commit y archivos indicados; no convierte automáticamente
en limpio otro archivo.

### Validación con Trellix Endpoint Security

Beta.37 fue probada manualmente como portable y después de instalarse en la
ubicación estándar de aplicaciones. Un análisis rápido examinó elementos y finalizó con cero detecciones; no se observó un evento nuevo
atribuible a estos binarios.

Un evento histórico correspondía a una regla corporativa de Protección de
acceso asociada a iniciar desde el navegador archivos ubicados en Descargas.
La acción era de auditoría (“bloquearía”) y no una detección de malware. Ejecutar
la aplicación instalada separa el runtime de esa regla específica, aunque el
instalador descargado todavía puede activar políticas, reputación o SmartScreen.

Si aparece una alerta: verificá el SHA-256 y el origen; no desactives Trellix ni
excluyas carpetas completas; usá distribución corporativa controlada; solicitá
a Seguridad revisar módulo, regla, acción y hash exactos; y limitá cualquier
allowlisting temporal al hash del release. Una detección real o cuarentena
requiere detener el despliegue y enviar la muestra a Trellix. Nunca publiques
logs completos de endpoints ni datos internos.

## Linux

El soporte Linux se encuentra en fase de desarrollo comunitario. La interfaz y
buena parte del código son multiplataforma, pero todavía deben validarse
PJPROJECT, PipeWire/PulseAudio, el almacenamiento seguro de credenciales, la
integración de escritorio y el empaquetado en cada distribución.

RC1 incorpora paquetes experimentales .deb y .tar.xz para Debian 13 amd64. Consultá [instalación, limitaciones y credenciales Linux](docs/LINUX_EXPERIMENTAL.md). No son binarios universales para todos los derivados ni una edición validada para producción.

Para evitar la colisión entre el ejecutable y el módulo QML en Linux, agregá `-DCMAKE_RUNTIME_OUTPUT_DIRECTORY="$PWD/build/bin"` al comando de configuración y ejecutá `./build/bin/CgPhone`.

## Compilar

### Windows

El workflow `.github/workflows/windows-portable.yml` compila CgPhone y
PJPROJECT mediante MSYS2/MinGW. El workflow actual también genera el
instalador con Inno Setup y publica el hash SHA-256 como artefacto.

Para ejecutar el build portable en MSYS2 MINGW64:

```bash
bash scripts/build-windows-portable-msys2.sh
```

### Ubuntu y Debian

También aplica a sus derivados habituales:

- **Linux Mint** basado en Ubuntu.
- **Linux Mint Debian Edition (LMDE)** basado en Debian.
- **Zorin OS** basado en Ubuntu.

Instalá las herramientas y bibliotecas de desarrollo:

```bash
sudo apt update
sudo apt install --no-install-recommends \
  build-essential cmake ninja-build pkg-config \
  qt6-base-dev qt6-declarative-dev qt6-multimedia-dev qt6-svg-dev \
  libpjproject-dev pulseaudio-utils
```

Comprobá que PJPROJECT esté visible mediante `pkg-config`:

```bash
pkg-config --modversion libpjproject
```

### Arch Linux y derivados

Este bloque aplica a Arch Linux y derivados que utilicen sus repositorios y
`pacman`, como EndeavourOS. En Manjaro la versión de los paquetes puede quedar
rezagada respecto de Arch y debe registrarse en el reporte de prueba.

```bash
sudo pacman -Syu --needed \
  base-devel cmake ninja pkgconf \
  qt6-base qt6-declarative qt6-multimedia qt6-svg \
  pjproject libpulse
```

Comprobá la dependencia SIP:

```bash
pkg-config --modversion libpjproject
```

Si la distribución no ofrece `pjproject`, o la versión instalada no publica
`libpjproject.pc`, no reemplaces bibliotecas al azar. Compilá PJPROJECT desde
una revisión identificada y documentá el commit utilizado en el pull request.

### Configurar y compilar

Para compilar solamente la interfaz con el motor simulado:

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/CgPhone
```

Para una beta funcional con PJSUA2:

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCGPHONE_WITH_PJSIP=ON
cmake --build build
ctest --test-dir build --output-on-failure
./build/CgPhone
```

Si CMake no encuentra Qt o PJPROJECT, incluí en el reporte las salidas de:

```bash
cmake --version
qmake6 --version || qtpaths6 --qt-version
pkg-config --modversion libpjproject
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCGPHONE_WITH_PJSIP=ON
```

No publiques contraseñas SIP, cabeceras `Authorization`, dominios internos ni
logs sin sanitizar.

## Beta testers y contribuciones Linux

Se reciben pruebas y pull requests para Ubuntu, Debian, Linux Mint, LMDE, Zorin
OS, Arch Linux y sus derivados. Para que un aporte pueda revisarse, el PR debe
indicar:

- distribución y versión exactas;
- arquitectura;
- entorno de escritorio y sesión X11 o Wayland;
- PipeWire o PulseAudio;
- versiones de Qt y PJPROJECT;
- comandos de compilación utilizados;
- resultado de `ctest`;
- pruebas de registro, llamadas entrantes/salientes, audio, DTMF, hold,
  transferencia, tray y diagnóstico;
- logs sanitizados y una descripción reproducible del problema.

Flujo recomendado:

1. Hacé un fork de este repositorio.
2. Creá una rama específica, por ejemplo `linux/ubuntu-24.04-audio`.
3. Aplicá un cambio pequeño y enfocado.
4. Compilá con `CGPHONE_WITH_PJSIP=ON` y ejecutá las pruebas.
5. Abrí un pull request contra `main` con la evidencia indicada.

Los aportes se revisarán y se integrarán progresivamente cuando sean
reproducibles, mantengan las funciones Free y no introduzcan regresiones en
Windows ni en otras distribuciones. Enviar un PR no garantiza su merge
automático.

## Seguridad

CgPhone procesa tráfico de red y contenido SIP no confiable. La política de
reporte, las limitaciones de la beta y el estado de dependencias están
documentados en:

- [Política de seguridad](SECURITY.md)
- [Seguridad y compliance](docs/SECURITY_COMPLIANCE.md)

La firma digital, SmartScreen, el análisis de dependencias y las pruebas
funcionales son controles distintos. Ninguno sustituye a los demás.

El build `v0.3.2-beta.38` genera un SBOM SPDX 2.3 desde los 231 archivos
del runtime distribuido y 16 paquetes. Su SHA-256 es:

```text
68b4afae287052cba2f0be05808a3b668c1e599d04e835798301e9d59e2d59aa
```

El SBOM permite inventariar componentes y evaluar CVE; no es ejecutable, no es
un certificado y no evita por sí mismo alertas de SmartScreen o Trellix.

## Licencias

CgPhone Free se prepara para distribución como software libre. Qt y PJPROJECT
tienen obligaciones de licencia que deben acompañar cada distribución.
PJPROJECT/PJSUA2 2.17 requiere resolver tanto sus advisories de seguridad como
la licencia aplicable antes de declarar una release productiva.

El código fuente correspondiente, avisos de copyright y textos de licencia
deben publicarse junto con los binarios Free. Recibir aportes voluntarios no
cambia las libertades ni las obligaciones de la licencia.

## Apoyar el proyecto

CgPhone Free se mantendrá gratuito. Los aportes ayudan a financiar pruebas,
mantenimiento, actualizaciones de seguridad y la futura firma digital de los
instaladores.

- [Apoyar en Ko-fi](https://ko-fi.com/J4Z326B1Z3)
- [Invitarme un tecito](https://tecito.app/cgphone)

## Autor

**Cristian Carroccio**

El icono oficial utiliza un teléfono blanco sobre el emblema azul de CgPhone y
se incluye como recurso multirresolución para aplicación, tray, accesos directos
e instalador.
