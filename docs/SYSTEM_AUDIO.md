# Controles de audio del sistema

Los controles del discador no aplican ganancia dentro del stream SIP. Modifican
el nivel maestro de los endpoints de audio del sistema para que el ajuste también
se refleje en Windows.

## Windows

`SystemAudioController` utiliza Core Audio (`IMMDeviceEnumerator` e
`IAudioEndpointVolume`). Busca primero los endpoints predeterminados con rol
`eCommunications` y utiliza `eConsole` como respaldo:

- salida: volumen maestro del headset/dispositivo de reproducción;
- micrófono: nivel maestro del dispositivo de captura.

El control de micrófono no sustituye Microphone Boost, AGC, cancelación de eco ni
procesamiento del driver. Esas funciones permanecen bajo Windows o el fabricante
del headset.

## Linux

Se consulta y modifica `@DEFAULT_SINK@` y `@DEFAULT_SOURCE@` mediante `pactl`,
compatible con PulseAudio y con PipeWire cuando está disponible la capa
`pipewire-pulse`.

## Operación

Los usuarios estándar pueden ajustar ambos niveles; no se requiere UAC. Si no
se detectan los dos endpoints, el panel aparece atenuado y deshabilitado. Antes
del piloto se debe comprobar que PJSIP y los controles apuntan al mismo headset,
especialmente cuando Windows tiene varios dispositivos predeterminados.
