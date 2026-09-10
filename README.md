# CgPhone Free

CgPhone Free es un softphone SIP de escritorio construido con Qt 6/QML para
Windows y, próximamente, Linux. El objetivo es ofrecer una aplicación simple,
liviana y transparente para registrar una cuenta SIP, realizar y recibir
llamadas y disponer de herramientas básicas de telefonía sin telemetría.

> **Estado actual:** `v0.3.2-beta.38`, validada para laboratorio controlado
> con Asterisk y Neotel. El [build #38](https://github.com/DarkCristian/CgPhone/actions/runs/34388740867)
> incorporó los parches de PJPROJECT para CVE-2026-57159, CVE-2026-57160 y
> CVE-2026-57162, y pasó compilación, Defender, hashes, SBOM y atestación.
> Windows x64 dispone de instalador y portable; Linux continúa en desarrollo
> experimental.

## Funciones actuales

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
- Al minimizar, CgPhone continúa registrado y disponible para recibir llamadas.
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

La compilación actual es **CgPhone Free v0.3.2-beta.38** para Windows x64:

- Instalador: `CgPhone-Setup-0.3.2-x64.exe`.
- Portable: `CgPhone-0.3.2-windows-x64-portable.zip`.
- Commit exacto compilado: `6d752cbbf5115a3a1f1f30db6d4eeddc41c06cf5`.
- Workflow validado: [#38](https://github.com/DarkCristian/CgPhone/actions/runs/34388740867).
- SBOM: SPDX 2.3, 231 archivos y 16 paquetes.

```text
cf3224819d10e892c250efb35f961b36caa9ac30c0dd377ef6d8e4777161eb09  CgPhone-Setup-0.3.2-x64.exe
e6fb2a737a0729b642d5d3e4d7285d8ef153d5586be03493536d9d255a50c7d3  CgPhone-0.3.2-windows-x64-portable.zip
68b4afae287052cba2f0be05808a3b668c1e599d04e835798301e9d59e2d59aa  CgPhone-0.3.2-sbom.spdx.json
0f100027a17286a6ec3b9cfe1619b784a963021740a3418dd0e837eacc32ce40  Microsoft-Defender-scan-report.txt
e1e9adbf001bdb6da0ab3efced9d3a9318723e4bf319f5a84b8c2024af505491  SHA256SUMS.txt
```

Descargá CgPhone únicamente desde el workflow/release oficial y verificá el
archivo individual, no sólo el ZIP contenedor generado por GitHub.

### Aviso de Microsoft SmartScreen

El instalador de beta.38 todavía no posee una firma Authenticode pública.
Windows puede mostrar el aviso **“Windows protegió su PC”**. La presencia o
ausencia de ese aviso no reemplaza la verificación del SHA-256.

Para verificar el archivo en PowerShell:

```powershell
Get-FileHash .\CgPhone-Setup-0.3.2-x64.exe -Algorithm SHA256
```

El resultado debe coincidir exactamente con el hash publicado arriba. Esta
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

Las instrucciones siguientes permiten compilar y comenzar las pruebas; no
representan todavía un paquete Linux oficialmente soportado.

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
