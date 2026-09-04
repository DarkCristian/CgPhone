# CgPhone Free

CgPhone Free es un softphone SIP de escritorio construido con Qt 6/QML para
Windows y, próximamente, Linux. El objetivo es ofrecer una aplicación simple,
liviana y transparente para registrar una cuenta SIP, realizar y recibir
llamadas y disponer de herramientas básicas de telefonía sin telemetría.

> **Estado actual:** versión 0.3.1 beta para laboratorio. Windows x64 dispone de
> instalador y portable. La compilación nativa para Linux está en desarrollo.

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

La versión preparada actualmente es:

- **CgPhone Free 0.3.1 beta**
- Windows x64.
- Instalador: `CgPhone-Setup-0.3.1-x64.exe`.
- Portable disponible como artefacto independiente.
- Commit de compilación validada:
  `c548e15b2545ab79b2eeea14c5dd2767d0b6a4ef`.
- SHA-256 del instalador:

```text
85a4744787827e2816865b0538ffe4f8d7157dc1be79abb26c27007776375b73
```

El código fuente público está disponible en este repositorio. Los instaladores
oficiales se publicarán en la sección **Releases** después de completar la
auditoría de secretos, licencias y archivos de distribución. No descargues
CgPhone desde sitios o enlaces no oficiales.

### Aviso de Microsoft SmartScreen

El instalador 0.3.1 beta todavía no posee una firma Authenticode pública.
Windows puede mostrar el aviso **“Windows protegió su PC”**. La presencia o
ausencia de ese aviso no reemplaza la verificación del SHA-256.

Para verificar el archivo en PowerShell:

```powershell
Get-FileHash .\CgPhone-Setup-0.3.1-x64.exe -Algorithm SHA256
```

El resultado debe coincidir exactamente con el hash publicado arriba. Esta
beta está destinada a pruebas controladas; no se presenta todavía como una
release aprobada para producción.

## Validación funcional de la beta

La beta.35 fue probada manualmente en Windows x64 contra centrales **Asterisk y
Neotel**. En ambos entornos se validaron satisfactoriamente:

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

Esta validación corresponde al artefacto exacto de la beta.35. Debe repetirse
en cada release y no sustituye un análisis de seguridad, compatibilidad o carga.

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
PJPROJECT mediante MSYS2/MinGW. La rama de release 0.3.1 también genera el
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

La pre-release `v0.3.1-beta.35` publica además un
[SBOM SPDX 2.3](https://github.com/DarkCristian/CgPhone/releases/download/v0.3.1-beta.35/CgPhone-0.3.1-beta.35-sbom.spdx.json)
generado desde los 231 archivos del runtime distribuido. Su SHA-256 es:

```text
bc2031ea10e0a59209c01c38abd2dd696969e66ca69e0b4f943b9a75b476ca5a
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
