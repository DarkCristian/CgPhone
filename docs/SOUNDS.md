# Sonidos y señalización de llamada

Los WAV se incorporan como recursos de Qt bajo `assets/sounds`. La interfaz no
debe inferir el registro SIP desde el estado de una llamada: una cuenta puede
seguir registrada mientras un destino responde ocupado o no contesta.

| Evento | Señal SIP habitual | Audio/UI |
|---|---|---|
| Llamada entrante | `INVITE` | `ringtone.wav` en bucle y diálogo Atender/Rechazar |
| Progreso saliente sin RTP | `180 Ringing` | `ringing.wav` local |
| Early media remoto | `183 Session Progress` con SDP y RTP activo | detener audio local y reproducir exclusivamente el RTP de la central |
| Contestada | `200 OK` | detener timbre/ringback e iniciar contador |
| Ocupado | `486 Busy Here` | detener ringback, mostrar Ocupado y reproducir `hangup.wav` |
| Sin respuesta | timeout / `408` / `480` | mostrar causa y reproducir `hangup.wav` |
| Corte o rechazo | `BYE`, `CANCEL` o respuesta final | reproducir `hangup.wav` |

La autorespuesta acepta el `INVITE` sin abrir el diálogo. DND debe rechazarlo
con la política configurada (normalmente `486`) y registrar el evento. En una
implementación funcional, PJSIP entregará estos eventos al controlador y Qt
Multimedia reproducirá los recursos; la alpha HTA solo simula el flujo.

Los anuncios de campaña, cola o IVR nunca se almacenan ni reproducen como WAV
locales de CgPhone. Asterisk/Neotel controlan la locución y su duración. Cuando
existe RTP remoto, ese audio tiene prioridad y no se mezcla con ringtone o
ringback local. Durante early media el micrófono permanece desconectado hasta
que la sesión quede confirmada con `200 OK`.
