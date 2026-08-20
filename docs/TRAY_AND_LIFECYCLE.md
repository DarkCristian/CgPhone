# Segundo plano y ciclo de vida

La alpha usa un controlador PowerShell con `NotifyIcon` porque HTA no expone
una API nativa para el área de notificación. El producto Qt debe reemplazarlo
por `QSystemTrayIcon` dentro del mismo proceso.

Comportamiento previsto:

- minimizar oculta la ventana y conserva registro SIP, audio y señalización;
- clic izquierdo abre Atender, Abrir CgPhone, Desloguearse y Cerrar;
- Atender solo está habilitado mientras existe un `INVITE` pendiente;
- Cerrar exige confirmación y luego termina registro SIP de manera ordenada;
- Desloguearse marca la feature code administrable, `*02` solo como valor inicial.

El código no debe asumirse universal. En producción se guarda por perfil de
central y el administrador puede bloquear su edición. Si existe una llamada
activa, el deslogueo se rechaza hasta finalizarla para evitar estados ambiguos.

El IPC por archivos de la alpha vive bajo `%LOCALAPPDATA%\CgPhoneAlpha` y está
aislado por sesión de usuario. Es únicamente un puente de demostración; la
aplicación Qt funcional manejará menú y estado internamente.

## Separación de privilegios

El proceso principal nunca debe elevarse. El engranaje inicia un proceso de
configuración temporal mediante UAC; Guardar o Descartar lo cierran y devuelven
el foco al proceso estándar. La presencia de un token elevado no debe habilitar
una solapa persistente dentro del softphone operativo.

La configuración se almacena con alcance de equipo (`QSettings::SystemScope` en
Qt; `%ProgramData%\CgPhone` en la alpha). Los usuarios estándar solo necesitan
lectura. En producción, la contraseña SIP no debe quedar en INI/JSON: deberá
cifrarse con DPAPI de máquina y restringirse mediante ACL, o administrarse desde
un servicio privilegiado.

## Identidad del box e inicio con el SO

El encabezado, la ventana de configuración y el tooltip del tray muestran el
usuario/interno SIP guardado en `SystemScope`. No se toma del usuario de dominio.
Si no existe una cuenta configurada, el badge muestra `SIN SIP`.

`Iniciar con el SO` se modifica únicamente desde la sesión administrativa:

- Windows: valor `CgPhone` en
  `HKLM\Software\Microsoft\Windows\CurrentVersion\Run`, apuntando al ejecutable
  instalado. El desinstalador elimina el valor para evitar rutas huérfanas.
- Linux: `/etc/xdg/autostart/CgPhone.desktop`, apuntando al ejecutable actual.

En ambos sistemas significa iniciar CgPhone al abrir una sesión gráfica. No se
ejecuta como servicio antes del login, porque el softphone necesita el contexto
de escritorio y los dispositivos de audio del usuario conectado.
