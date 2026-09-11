# Linux experimental — CgPhone 0.3.2-rc.1

Esta release incluye Windows RC validado y Linux experimental. No implica paridad de validación entre plataformas.

## Compatibilidad

Primer objetivo binario: Debian 13 (trixie), amd64/x86-64, Qt del sistema. El .deb declara las dependencias obtenidas del binario y los módulos QML necesarios. El tar.xz contiene el mismo ejecutable y recursos, pero NO es un portable autocontenido: necesita las bibliotecas indicadas en Linux-runtime-dependencies.txt. No incluye Qt ni glibc.

Ubuntu, Linux Mint, Zorin OS, LMDE, Arch y derivados requieren comprobar sus versiones de Qt, glibc y bibliotecas. No instalar el .deb de Debian forzando dependencias en otra distribución. Si no son compatibles, compilar desde fuente. No se garantiza este binario en Ubuntu 22.04/24.04 ni Debian 12. Se esperan PR de beta testers con distribución, versión, escritorio, X11/Wayland y resultados; se integrarán tras revisión.

## Instalación y ejecución

En Debian 13: `sudo apt install ./cgphone_0.3.2-rc.1_linux-experimental_amd64.deb`.

Ejecutar `CgPhone` como usuario normal. Para probar el tar.xz, extraerlo y ejecutar `./usr/bin/CgPhone` con las dependencias previamente instaladas. No ejecutar la telefonía cotidiana como root.

## Credenciales y administración: limitación experimental

Linux guarda la contraseña SIP sin cifrar mediante QSettings SystemScope (normalmente /etc/xdg/CgPhone/CgPhone.ini; la ubicación depende de Qt y del entorno). No hay integración con Secret Service/KWallet ni protección equivalente a DPAPI. Los usuarios que puedan leer ese archivo pueden conocer la contraseña. No usar credenciales productivas: utilizar una cuenta de laboratorio con permisos limitados. No distribuir archivos INI ni logs SIP personales.

La edición actual requiere privilegios root para guardar ajustes globales y el autoinicio; el engranaje NO implementa elevación gráfica automática con polkit. El modo `CgPhone --configure` requiere un proceso previamente elevado, y su apertura depende de las restricciones del servidor gráfico. No se ha validado bajo Wayland; no se recomienda desactivar controles del servidor gráfico para habilitarlo. La configuración administrativa cómoda y el almacenamiento seguro quedan pendientes.

## Alcance de las pruebas

El workflow compila PJSUA2 real, ejecuta ctest y comprueba arranque QML sin cuenta SIP y contenido de los paquetes en un entorno limpio. No valida registro, llamadas, audio físico, micrófono, grabación, cambio de sesión ni tray en un escritorio real. La validación Asterisk/Neotel de Windows NO se atribuye a Linux.

Volumen: requiere pactl y un servidor PulseAudio o compatibilidad pipewire-pulse. El tray depende del escritorio. Grabación: usar WAV; el localizador MP3 actual busca DLL de Windows. La contraseña no se cifra y no hay UAC de Windows en Linux.

## Seguridad y evidencias

PJPROJECT se fija al commit a1b707c0c9b0506faf2a8a438b60f11ffd6a6fd9 utilizado para los parches de beta.38/39. Se deshabilitan vídeo, SRTP y el mapeo remoto de payload types como en la base Windows. Las revisiones Debian completas están en Linux-build-packages.tsv; no asumir que su versión upstream describe todos los parches Debian.

El SBOM Linux identifica los archivos distribuidos, PJPROJECT y el inventario del entorno de compilación. No equivale a una certificación ni a un análisis completo de CVE/licencias. Las bibliotecas dinámicas las proporciona y actualiza la distribución. El reporte Defender de la release corresponde exclusivamente a Windows. SHA-256 y atestaciones no son Authenticode ni firmas de un repositorio APT.
