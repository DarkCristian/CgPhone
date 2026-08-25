# CgPhone — firma, confianza y publicación segura

## Objetivo

Producir artefactos Windows verificables y reducir advertencias de Microsoft Defender SmartScreen sin desactivar ni eludir controles de seguridad.

## Hechos importantes

- Un certificado autofirmado no establece confianza pública ni elimina SmartScreen.
- Cambiar el nombre, comprimir el ejecutable o indicar al usuario que ignore la advertencia no es una solución aceptable.
- SmartScreen evalúa reputación de archivo, aplicación, origen y certificado. Incluso un binario correctamente firmado puede mostrar advertencias durante sus primeras distribuciones.
- La identidad firmante debe pertenecer legalmente al titular que publicará y mantendrá CgPhone. La clave privada nunca debe almacenarse en Git ni incorporarse al portable.

## Estrategia recomendada

1. Contratar Microsoft Artifact Signing o un certificado Authenticode emitido por una CA pública.
2. Mantener la clave en HSM o servicio de firma; GitHub Actions sólo recibe autorización temporal.
3. Compilar en un runner limpio y con dependencias fijadas.
4. Generar SBOM, inventario de Qt/PJPROJECT, hashes SHA-256 y resultados de pruebas.
5. Firmar CgPhone.exe, DLL propias y el instalador final con SHA-256.
6. Aplicar sello de tiempo RFC 3161.
7. Verificar todas las firmas después de empaquetar.
8. Analizar el artefacto con Microsoft Defender y conservar los resultados del workflow.
9. Publicar siempre desde un origen estable y conservar el mismo publisher/certificado para acumular reputación.
10. Firmar la release/tag y adjuntar hashes, SBOM y notas de seguridad.

## Controles obligatorios del workflow

- Build reproducible en lo posible.
- Versiones de dependencias fijadas.
- Tests unitarios y smoke test del portable.
- CodeQL y análisis de secretos.
- SBOM CycloneDX o SPDX.
- SHA-256 de EXE, DLL, ZIP e instalador.
- Firma Authenticode y timestamp.
- Verificación con SignTool antes de publicar.
- Firma de tags/releases.
- Retención de logs y attestations de GitHub Actions.

## Comando de verificación esperado

```powershell
signtool verify /pa /all /v CgPhone.exe
Get-FileHash CgPhone.exe -Algorithm SHA256
```

## Secretos

No guardar PFX, contraseña, token de firma ni clave privada en el repositorio. Para firma cloud usar OIDC cuando el proveedor lo permita. Si se usa un certificado local, almacenarlo en un HSM/token administrado y firmar mediante un runner controlado.

## Estado

Este branch prepara compliance y firma. La firma efectiva queda bloqueada hasta elegir y validar la identidad de publicación y el proveedor de firma.
