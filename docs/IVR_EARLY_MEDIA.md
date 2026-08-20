# Anuncios de campaña, IVR y early media

## Decisión de arquitectura

CgPhone no contiene ni sintetiza la locución “Campaña X, llamada entrante”. La
central Asterisk o Neotel conserva la lógica de campaña, cola, horarios y
anuncios. El softphone se limita a reproducir el audio SIP/RTP negociado y no
debe mezclarlo con sonidos locales.

## Comportamiento implementado

- Autorespuesta envía `200 OK` sin demora artificial y sin iniciar ringtone.
- Si llega `183 Session Progress` con SDP y el medio de audio queda activo,
  PJSUA2 conecta inmediatamente RTP remoto al dispositivo de reproducción.
- Al activarse medio remoto se detienen ringtone y ringback locales.
- Durante early media solo se habilita central → headset. El micrófono del
  agente se conecta al RTP después de que la sesión queda `CONFIRMED`.
- No existe timeout local para el anuncio: puede durar lo que defina la central.
- El mismo stream y dispositivo de audio continúan al producirse el bridge con
  el cliente; no se reinicia el headset entre anuncio y conversación.

Los callbacks son asíncronos. “Inmediatamente” significa en el siguiente ciclo
del event loop de Qt/PJSIP, sin un `sleep` ni un temporizador funcional agregado
por CgPhone. El objetivo de laboratorio es que la interrupción de audio local
ocurra dentro de 100 ms desde el callback de medio activo.

## Asterisk

Para una cola, usar el anuncio al agente configurado por la central. La
aplicación `Queue()` admite `announceoverride` para reproducir archivos al
agente antes de puentearlo con quien llama. CgPhone debe recibir ese audio como
RTP normal después de Autoanswer.

También se valida `183 + SDP` porque algunos dialplans, troncales o integraciones
entregan audio antes del `200 OK`. No debe configurarse un WAV equivalente en
CgPhone como solución alternativa.

## Neotel

La campaña debe conservar su anuncio/pre-atención del lado de Neotel. En el
piloto se debe identificar si lo entrega después del `200 OK` o como early
media. CgPhone acepta ambos flujos, pero la central debe enviar SDP válido, un
codec habilitado y RTP alcanzable desde el box.

No se presupone que todas las versiones o configuraciones de Neotel se comporten
igual. La compatibilidad se aprueba únicamente con una llamada real de cada
campaña y captura de señalización/RTP.

## Criterios de aceptación

1. Autoanswer no muestra diálogo ni reproduce ringtone local.
2. La locución se escucha completa por el headset configurado.
3. No hay mezcla de ringback/ringtone con audio de la central.
4. El micrófono no transmite durante early media previo al `200 OK`.
5. No hay silencio ni cambio de dispositivo al pasar del anuncio al cliente.
6. Anuncios de 2, 10, 30 y 120 segundos no son cortados por CgPhone.
7. Si no llega RTP, se diagnostican SDP, codec, NAT y firewall; no se sustituye
   el anuncio con un archivo local.

## Evidencia requerida para producción

- log SIP con `INVITE`, respuestas provisionales, `200 OK`, `ACK` y `BYE`;
- SDP ofrecido/aceptado y codec finalmente negociado;
- captura RTP que demuestre audio antes y después del bridge;
- prueba con Autorespuesta habilitada y deshabilitada;
- una prueba Asterisk y al menos una por tipo de campaña/cola Neotel.
