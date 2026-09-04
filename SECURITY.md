# Política de seguridad de CgPhone Free

Última revisión: 2026-09-04 (SBOM de beta.35 incorporado).

## Versiones soportadas

| Versión | Plataforma | Estado | Actualizaciones de seguridad |
|---|---|---|---|
| [v0.3.1-beta.35](https://github.com/DarkCristian/CgPhone/releases/tag/v0.3.1-beta.35) | Windows x64 | Laboratorio controlado | Sí, durante la etapa beta |
| Linux | Ubuntu, Debian, Mint, LMDE, Zorin, Arch y derivados | En desarrollo, sin paquete soportado | Evaluación comunitaria |

Versiones anteriores y artefactos generados por otros workflows no deben
considerarse equivalentes aunque conserven el nombre 0.3.1. Cada archivo debe
validarse mediante su SHA-256.

## Reportar una vulnerabilidad

No publiques en un issue abierto:

- credenciales o contraseñas SIP;
- direcciones, dominios o topología interna;
- capturas SIP/RTP sin sanitizar;
- logs reales de asesores o llamadas;
- certificados, tokens o datos personales;
- instrucciones explotables antes de disponer de una corrección.

Utilizá un **Private Vulnerability Report / Security Advisory** de GitHub. Si
esa función no estuviera disponible, contactá directamente al responsable del
repositorio antes de divulgar información sensible.

Incluí, si es posible:

- versión y tag de CgPhone;
- SHA-256 del instalador o portable;
- commit utilizado;
- Windows o distribución Linux y arquitectura;
- central y transporte SIP involucrados, sin secretos;
- pasos mínimos para reproducir;
- impacto observado;
- logs sanitizados.

No se promete una recompensa económica. Los reportes se evaluarán según
reproducibilidad, alcance e impacto.

## Evidencia de v0.3.1-beta.35

| Campo | Valor |
|---|---|
| Workflow validado | Windows Free installer, run #35 |
| Commit exacto del build | `e4fbcb2623cc58dd2676fffeaa1cf73afbe14d41` |
| Commit del tag/merge en `main` | `a6dba9e96240405b7725fb4fcf60e89554ee9bd0` |
| Instalador | `CgPhone-Setup-0.3.1-x64.exe` |
| Portable publicado | `CgPhone-Free-0.3.1-Windows-x64-portable.zip` |
| SBOM publicado | `CgPhone-0.3.1-beta.35-sbom.spdx.json` (SPDX 2.3) |
| SHA-256 SBOM | `bc2031ea10e0a59209c01c38abd2dd696969e66ca69e0b4f943b9a75b476ca5a` |
| Firma Authenticode | No aplicada |
| Clasificación | Beta de laboratorio / pre-release |

### SHA-256 del instalador

```text
40dfdf77fac4cbb98d3131c0835445da4db77cb9da3ed45c229850f4da7f43c1
```

### SHA-256 del portable publicado

```text
70052f48df73a62670831760e36bb7366e6e62cc8d8012a719cc6943e4f8aa09
```

### SHA-256 del SBOM SPDX 2.3

```text
bc2031ea10e0a59209c01c38abd2dd696969e66ca69e0b4f943b9a75b476ca5a
```

El [SBOM de beta.35](https://github.com/DarkCristian/CgPhone/releases/download/v0.3.1-beta.35/CgPhone-0.3.1-beta.35-sbom.spdx.json)
fue generado a partir del árbol final del portable: inventaría 231 archivos y
17 paquetes/componentes. No es un certificado, no se ejecuta y no reemplaza la
firma Authenticode ni el análisis antivirus. Los campos `NOASSERTION` indican
licencias o metadatos que todavía requieren evidencia y revisión.

Estos hashes identifican únicamente los archivos exactos publicados en la
Release. Toda recompilación, reempaquetado o modificación produce un hash
diferente y requiere nueva evidencia.

El instalador y el portable:

- fueron generados mediante GitHub Actions;
- no poseen firma Authenticode pública;
- pueden activar Microsoft SmartScreen;
- están destinados a pruebas controladas;
- deben descargarse únicamente desde la Release oficial.

Un motor de VirusTotal marcó heurísticamente el instalador. Una detección
aislada no confirma malware ni permite declarar por sí sola un falso positivo.
La procedencia reproducible, los hashes, la firma y los resultados antivirus
son controles separados.

## Dependencias observadas en el build #35

Estas versiones provienen del archivo
`COMPLIANCE/DEPENDENCY-VERSIONS.txt` incluido en el portable:

| Componente | Versión/revisión observada |
|---|---|
| Qt Base | 6.11.2-2 |
| Qt Declarative / Multimedia / SVG | 6.11.2-1 |
| PJPROJECT/PJSUA2 | 2.17, commit `5a457451fa2712ba18e12b01738e8ff3af2b26fd` |
| OpenSSL | 3.6.4-1 |
| FFmpeg | 9.0.1-3 |
| Opus | 1.6.1-1 |
| LAME | 3.100-3 |
| GCC/MinGW | 16.2.0-3 |
| CMake | 4.4.2-2 |

MSYS2 utiliza paquetes móviles. Esta evidencia describe el build #35, pero no
garantiza que una ejecución futura del workflow obtenga las mismas versiones.

## Dependencias y límites conocidos

CgPhone procesa mensajes SDP/SIP y audio provenientes de la red. La versión
actual utiliza PJPROJECT/PJSUA2 2.17, que requiere revisar advisories y parches
aplicables antes de declarar una release productiva.

La edición Free no integra Qt WebEngine ni Qt WebView. Los avisos de Chromium o
Qt WebEngine no son automáticamente aplicables: sólo deben reevaluarse si esos
módulos vuelven a incorporarse.

La ausencia de telemetría no elimina otros riesgos: la configuración, los logs,
las grabaciones y las credenciales requieren permisos de archivo apropiados.

## Criterio de publicación estable

Una versión estable requiere completar y documentar el gate definido en
[Seguridad y compliance](docs/SECURITY_COMPLIANCE.md), incluyendo:

- revisión y parcheo de dependencias;
- SBOM publicado y escaneo de vulnerabilidades pendiente;
- análisis estático;
- análisis del paquete final con Microsoft Defender;
- pruebas SIP en laboratorio;
- revisión de logs y permisos;
- firma Authenticode y timestamp cuando esté disponible;
- publicación del código fuente y licencias correspondientes.

SmartScreen, una firma válida, un hash coincidente o un antivirus sin
detecciones son evidencias independientes; ninguna demuestra por sí sola que el
software sea seguro.

Los controles gratuitos pendientes —licencia explícita del proyecto, textos de
terceros, CodeQL, análisis de CVE, secret scanning, ACL, sanitización de logs,
atestación del build y pruebas con Defender/Trellix— se mantienen en el
[checklist de compliance](docs/SECURITY_COMPLIANCE.md#controles-pendientes-que-no-requieren-pagar).
