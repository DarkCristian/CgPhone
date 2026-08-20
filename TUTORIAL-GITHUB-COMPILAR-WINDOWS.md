# Tutorial básico: compilar CgPhone para Windows con GitHub

Este procedimiento usa una computadora de GitHub para compilar CgPhone. No
necesitás instalar Qt, CMake, MSYS2 ni PJPROJECT localmente.

## Antes de comenzar

Necesitás:

1. Una cuenta en https://github.com/.
2. Git para Windows: https://git-scm.com/download/win.
3. El ZIP `CgPhone-0.2.10-functional-source.zip` descomprimido.

El proyecto debe quedar así:

```text
CgPhone\
├── .github\workflows\windows-portable.yml
├── assets\
├── qml\
├── scripts\
├── src\
├── CMakeLists.txt
└── README.md
```

La carpeta oculta `.github` es imprescindible. Si no se sube, GitHub no verá
el compilador automático.

## 1. Crear un repositorio vacío

1. Entrá a GitHub.
2. Arriba a la derecha, pulsá `+` y después `New repository`.
3. En `Repository name` escribí `CgPhone`.
4. Elegí `Private` si el código no debe ser público.
5. No marques `Add a README`, `.gitignore` ni `license`.
6. Pulsá `Create repository`.
7. Dejá abierta la página: vas a usar la dirección HTTPS mostrada.

Ejemplo:

```text
https://github.com/TU-USUARIO/CgPhone.git
```

Documentación oficial:
https://docs.github.com/repositories/creating-and-managing-repositories/creating-a-new-repository

## 2. Subir el código desde Windows

1. Descomprimí el ZIP.
2. Entrá a la carpeta `CgPhone` que contiene `CMakeLists.txt`.
3. Hacé clic derecho en un espacio vacío y elegí `Open Git Bash here`.
4. Ejecutá los siguientes comandos de a uno.

Configurá tu identidad. Reemplazá los datos de ejemplo:

```bash
git config --global user.name "Cristian Apellido"
git config --global user.email "tu-correo@example.com"
```

Creá el repositorio local:

```bash
git init
git add .
git commit -m "CgPhone 0.2.10"
git branch -M main
```

Vinculalo con GitHub. Reemplazá `TU-USUARIO`:

```bash
git remote add origin https://github.com/TU-USUARIO/CgPhone.git
git push -u origin main
```

Git puede abrir el navegador para iniciar sesión. No escribas tu contraseña de
GitHub dentro de los archivos del proyecto.

Documentación oficial:
https://docs.github.com/migrations/importing-source-code/using-the-command-line-to-import-source-code/adding-locally-hosted-code-to-github

## 3. Comprobar que el workflow existe

1. Actualizá la página del repositorio.
2. Comprobá que aparezcan las carpetas `src`, `qml`, `scripts` y `.github`.
3. Entrá en `.github/workflows/`.
4. Debe existir `windows-portable.yml`.

Si `.github` no aparece, repetí dentro de la carpeta correcta:

```bash
git add .github/workflows/windows-portable.yml
git commit -m "Agregar compilación automática"
git push
```

## 4. Ejecutar la compilación

1. Abrí la pestaña `Actions` del repositorio.
2. Si GitHub muestra una advertencia, habilitá los workflows del repositorio.
3. En la columna izquierda elegí `Windows portable SIP`.
4. Pulsá `Run workflow`.
5. Confirmá nuevamente con el botón verde `Run workflow`.
6. Aparecerá una ejecución con un círculo amarillo.
7. Esperá hasta que se convierta en una marca verde.

El botón manual existe porque el archivo utiliza `workflow_dispatch` y debe
estar en la rama principal.

Documentación oficial:
https://docs.github.com/actions/managing-workflow-runs/manually-running-a-workflow

## 5. Descargar el resultado

1. Abrí la ejecución que terminó con la marca verde.
2. Bajá hasta la sección `Artifacts`.
3. Pulsá `CgPhone-0.2.10-windows-x64-portable`.
4. GitHub descargará un ZIP contenedor.
5. Descomprimilo. Adentro encontrarás otro archivo:
   `CgPhone-0.2.10-windows-x64-portable.zip`.
6. Descomprimí también ese segundo ZIP.
7. Ejecutá `CgPhone-portable\bin\CgPhone.exe`.

Documentación oficial:
https://docs.github.com/actions/managing-workflow-runs/downloading-workflow-artifacts

## 6. Si aparece una pantalla azul de Windows

El ejecutable todavía no está firmado digitalmente. SmartScreen puede mostrar
`Windows protegió su PC`.

Para una prueba controlada:

1. Pulsá `Más información`.
2. Verificá que el archivo sea el que descargaste de tu propio workflow.
3. Pulsá `Ejecutar de todas formas`.

No distribuyas masivamente esta compilación sin firma de código y sin probarla
primero contra una central de laboratorio.

## 7. Configurar y probar con Asterisk o Neotel

1. Abrí CgPhone.
2. Pulsá el engranaje.
3. Aceptá UAC.
4. Completá usuario SIP, contraseña, servidor y proxy si corresponde.
5. Guardá y esperá el estado `Online` o `SIP 200`.
6. Probá registro, llamada saliente, entrante, audio en ambos sentidos, DTMF,
   hold, transferencia, autorespuesta y early media/IVR.

Que el programa compile no demuestra compatibilidad total con la central. Una
prueba real debe incluir NAT, codecs, señalización SIP y RTP.

## Errores frecuentes

### No aparece `Run workflow`

- Confirmá que `windows-portable.yml` esté en `.github/workflows/`.
- Confirmá que el archivo esté en la rama `main`.
- Revisá que Actions esté habilitado.

### `Repository not found` al hacer push

- Revisá el usuario y nombre de la URL.
- Confirmá que hayas iniciado sesión con la cuenta propietaria.

### `remote origin already exists`

Mostrá la dirección actual:

```bash
git remote -v
```

Si está equivocada, corregila:

```bash
git remote set-url origin https://github.com/TU-USUARIO/CgPhone.git
git push -u origin main
```

### La compilación termina en rojo

1. Abrí la ejecución fallida.
2. Entrá en el trabajo `build`.
3. Abrí el primer paso marcado en rojo.
4. Copiá el bloque final del log; no alcanza con una captura que solo muestre
   `Process completed with exit code 1`.

### GitHub no deja descargar el artefacto

Debés estar conectado y tener acceso de lectura al repositorio. Los artefactos
no son permanentes: este workflow los conserva durante 14 días.

