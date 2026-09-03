# Política de seguridad de CgPhone Free

Última revisión: 2026-09-03.

## Versiones soportadas

| Versión | Estado | Actualizaciones de seguridad |
|---|---|---|
| 0.3.1 beta | Laboratorio controlado | Sí, durante la etapa beta |
| 0.3.0 y anteriores | Reemplazadas | No |

CgPhone Free 0.3.1 todavía no está aprobado para despliegue productivo ni para
exposición SIP directa a Internet. La compatibilidad con una central no
constituye una certificación de seguridad.

## Reportar una vulnerabilidad

No publiques en un issue abierto:

- credenciales o contraseñas SIP;
- direcciones, dominios o topología interna;
- capturas SIP/RTP sin sanitizar;
- logs reales de asesores o llamadas;
- certificados, tokens o datos personales;
- instrucciones explotables antes de disponer de una corrección.

Utilizá un **Private Vulnerability Report / Security Advisory** de GitHub cuando
el repositorio público tenga habilitada esa función. Mientras el proyecto
permanezca privado, reportalo directamente al responsable del repositorio.

Incluí, si es posible:

- versión de CgPhone;
- SHA-256 del instalador o portable;
- commit utilizado;
- Windows o distribución Linux y arquitectura;
- central y transporte SIP involucrados, sin secretos;
- pasos mínimos para reproducir;
- impacto observado;
- logs sanitizados.

No se promete una recompensa económica. Los reportes se evaluarán según
reproducibilidad, alcance e impacto.

## Estado de la beta 0.3.1

El instalador actual:

- fue generado correctamente mediante GitHub Actions;
- no posee todavía firma Authenticode pública;
- puede activar Microsoft SmartScreen;
- incluye un SHA-256 para comprobar integridad;
- está destinado a pruebas controladas.

SHA-256 publicado de `CgPhone-Setup-0.3.1-x64.exe`:

```text
85a4744787827e2816865b0538ffe4f8d7157dc1be79abb26c27007776375b73
```

Este hash identifica solamente ese archivo exacto. Toda recompilación produce
un archivo diferente y requiere un hash nuevo.

## Dependencias y límites conocidos

CgPhone procesa mensajes SDP/SIP y audio provenientes de la red. La versión
actual utiliza PJPROJECT/PJSUA2 2.17, que posee advisories posteriores
relevantes. Por eso no se considera una release productiva hasta integrar y
documentar los parches aplicables.

La ausencia de telemetría no elimina otros riesgos: la configuración, los logs,
las grabaciones y las credenciales requieren permisos de archivo apropiados.

## Criterio de publicación estable

Una versión estable requiere completar y documentar el gate definido en
[Seguridad y compliance](docs/SECURITY_COMPLIANCE.md), incluyendo:

- revisión y parcheo de dependencias;
- SBOM y escaneo de vulnerabilidades;
- análisis estático;
- análisis del paquete final con Microsoft Defender;
- pruebas SIP en laboratorio;
- revisión de logs y permisos;
- firma Authenticode y timestamp cuando esté disponible;
- publicación del código fuente y licencias correspondientes.

SmartScreen, una firma válida, un hash coincidente o un antivirus sin
detecciones son evidencias independientes; ninguna demuestra por sí sola que el
software sea seguro.
