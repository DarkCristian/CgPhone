# CgPhone Free

CgPhone Free es un softphone SIP de escritorio construido con Qt 6/QML para
Windows y, próximamente, Linux. El objetivo es ofrecer una aplicación simple,
liviana y transparente para registrar una cuenta SIP, realizar y recibir
llamadas y disponer de herramientas básicas de telefonía sin telemetría.

> **Estado actual:** versión 0.3.1 beta para laboratorio. Windows x64 dispone de
> instalador y portable. La compilación nativa para Linux está en desarrollo.

## Funciones actuales

- Una cuenta SIP por terminal mediante PJPROJECT/PJSUA2.
- Registro automático y estado visible de la cuenta.
- Llamadas entrantes y salientes, ringtone, atender, rechazar y colgar.
- DTMF desde el pad y el teclado.
- DND con respuesta `486 Busy Here`.
- Autorespuesta con `200 OK`.
- Early media y anuncios SIP recibidos desde la central.
- Hold, mute, transferencia ciega mediante SIP REFER y grabación WAV.
- Historial persistente con filtros, rellamada y borrado.
- Volumen real del headset y nivel del micrófono.
- Prueba local de audio.
- Inicio con Windows, siempre visible y funcionamiento en el system tray.
- Configuración administrativa separada mediante UAC.
- Contraseña SIP protegida con DPAPI de máquina en Windows.
- Diagnóstico SIP con `Shift+F12` y lectura del log nativo de PJSIP.
- Pantalla de Créditos con autor y licencias.

El alcance mantenido se concentra en telefonía SIP de audio con una cuenta por
terminal. Las nuevas funciones deberán preservar esa base simple y auditable.

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

## Prueba mínima de la beta

1. Instalar y desinstalar en una terminal Windows x64 limpia.
2. Configurar una cuenta mediante el engranaje y confirmar el cierre de UAC.
3. Verificar registro SIP y recuperación después de una interrupción de red.
4. Realizar al menos dos llamadas salientes consecutivas.
5. Probar llamada entrante, atender, rechazar, DND y autorespuesta.
6. Probar DTMF, hold/unhold, mute y transferencia.
7. Validar audio, prueba local, volumen y nivel de micrófono.
8. Verificar historial, grabación WAV, tray y autoarranque.
9. Abrir y ocultar el diagnóstico mediante `Shift+F12`.
10. Confirmar que `cgphone-sip.log` no exponga contraseñas ni secretos.

## Linux

La interfaz y buena parte del código son multiplataforma, pero el paquete
Linux todavía no está publicado. Faltan validar la compilación nativa,
PJPROJECT, PulseAudio/PipeWire, almacenamiento seguro de credenciales,
integración de escritorio y empaquetado para distribuciones compatibles.

No se anuncia una fecha hasta completar esas pruebas.

## Compilar

### Windows

El workflow `.github/workflows/windows-portable.yml` compila CgPhone y
PJPROJECT mediante MSYS2/MinGW. La rama de release 0.3.1 también genera el
instalador con Inno Setup y publica el hash SHA-256 como artefacto.

Para ejecutar el build portable en MSYS2 MINGW64:

```bash
bash scripts/build-windows-portable-msys2.sh
```

### Linux

Requiere Qt 6.5 o posterior, CMake 3.22 o posterior, Ninja y PJPROJECT/PJSUA2
para la misma arquitectura:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

Para activar el backend SIP real:

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCGPHONE_WITH_PJSIP=ON
cmake --build build
```

Los nombres de paquetes y la integración de audio varían según la
distribución. Este procedimiento aún no representa un paquete Linux soportado.

## Seguridad

CgPhone procesa tráfico de red y contenido SIP no confiable. La política de
reporte, las limitaciones de la beta y el estado de dependencias están
documentados en:

- [Política de seguridad](SECURITY.md)
- [Seguridad y compliance](docs/SECURITY_COMPLIANCE.md)

La firma digital, SmartScreen, el análisis de dependencias y las pruebas
funcionales son controles distintos. Ninguno sustituye a los demás.

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
