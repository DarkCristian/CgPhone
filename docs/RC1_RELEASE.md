# CgPhone 0.3.2 RC1 — Windows y Linux experimental

RC sin firma Authenticode, publicada como prerelease. No equivale a aprobación productiva ni certificación de seguridad.

## Paquetes

- Windows x64: instalador EXE y portable ZIP exactos del build #39, sin recompilar ni cambiar sus hashes. El mantenedor validó las pruebas de Windows, incluida minimización en barra de tareas y restauración desde tray. Se mantienen los parches PJPROJECT de beta.38.
- Linux amd64: .deb y .tar.xz EXPERIMENTALES compilados para Debian 13 (trixie), con dependencias Qt del sistema. El tar.xz no es autocontenido. No se garantiza compatibilidad binaria con Ubuntu, Mint, Zorin, LMDE o Arch; consultar las notas Linux y no forzar dependencias.

La denominación RC se aplica a la candidata Windows. Linux sigue experimental: su compilación, ctest y arranque QML en entorno limpio no sustituyen pruebas de SIP, centrales Asterisk/Neotel, audio físico o escritorios reales. No incluye macOS.

## Cambios y límites

CgPhone conserva el botón en la barra de tareas al minimizar. El tray mantiene sus acciones y las llamadas entrantes restauran la ventana. Los binarios Windows mantienen la versión interna 0.3.2 del build validado.

En Linux la contraseña SIP se guarda sin cifrar en configuración de sistema. Usar exclusivamente credenciales de laboratorio. No hay elevación gráfica integrada para Ajustes, ni paridad con DPAPI/UAC; MP3 no está disponible en esta preview, usar WAV. Detalles de instalación, permisos, configuración y pruebas en LINUX-EXPERIMENTAL.md adjunto y docs/LINUX_EXPERIMENTAL.md.

## Evidencias

Se adjuntan SBOM separados para Windows y Linux, versiones del entorno Linux, reportes de pruebas, manifiestos SHA-256 por plataforma y un manifiesto combinado. El SBOM Linux contiene inventario de archivos y entorno de compilación, no una afirmación de ausencia de vulnerabilidades. El reporte Microsoft Defender es exclusivamente del build Windows #39. Las atestaciones verifican procedencia y no sustituyen Authenticode ni la revisión de licencias pendiente.

SmartScreen puede advertir. SignPath sigue pendiente; MSIX no está implementado. Trellix y otras políticas empresariales pueden requerir aprobación específica del hash por Seguridad; no se publican logs, rutas personales, IP ni datos empresariales.

Se esperan aportes y PR de beta testers Linux, indicando distribución/versión, escritorio, X11/Wayland y resultados de llamadas/audio. Los aportes se revisarán antes de integrarse.
