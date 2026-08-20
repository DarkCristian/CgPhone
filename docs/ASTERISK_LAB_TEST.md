# Prueba controlada contra Asterisk

No conectar primero a una campaña activa. Cree un endpoint de laboratorio y
limite su contexto de marcado.

## Datos necesarios

- interno/usuario de autenticación;
- contraseña;
- IP o FQDN y puerto del servidor;
- proxy solo si la topología lo requiere;
- UDP permitido entre el box y Asterisk;
- rango RTP de Asterisk permitido en firewall.

## Orden de validación

1. Guardar la cuenta mediante el engranaje/UAC.
2. Confirmar `200 OK` de REGISTER y estado Online.
3. Llamar a otro interno y comprobar audio bidireccional.
4. Hacer una llamada entrante, atender y rechazar.
5. Probar DTMF contra un IVR.
6. Transferir mediante REFER a un interno de laboratorio.
7. Probar DND (`486 Busy Here`) y autorespuesta.
8. Configurar una cola con anuncio al agente (`announce`/`announceoverride`) y
   comprobar que Autorespuesta reproduce la locución completa antes del bridge,
   sin ringtone local superpuesto.
9. Probar `183 Session Progress` con SDP/RTP y verificar que CgPhone detiene el
   ringback local al recibir el primer medio remoto activo.
10. Cerrar sesión de Windows, entrar con otro usuario y confirmar que el mismo
   interno del box vuelve a registrarse.

## Diagnóstico Asterisk

En la CLI de un laboratorio:

```text
pjsip set logger on
core set verbose 5
rtp set debug on
```

No deje esos niveles habilitados indefinidamente en producción. Si registra
pero no hay audio, el problema suele estar en RTP/NAT/firewall, no en REGISTER.

## Criterio de rechazo

No avanzar a piloto si falla cualquiera de estos puntos: registro persistente,
audio en ambos sentidos, DTMF, rechazo/corte correcto o recuperación después de
reiniciar/cambiar de usuario. Un softphone que solo “marca” no está listo para
call center.

Tampoco avanzar si el anuncio de campaña queda truncado, se mezcla con el
ringtone, abre el micrófono antes del `200 OK` o cambia de dispositivo de audio
entre la locución y la conversación.
