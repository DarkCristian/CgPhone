# Política de seguridad de CgPhone Free

Última revisión: 2026-09-10 (beta.38 validada con Asterisk/Neotel y Defender).

## Versiones soportadas

| Versión | Plataforma | Estado | Actualizaciones de seguridad |
|---|---|---|---|
| v0.3.2-beta.38 ([workflow #38](https://github.com/DarkCristian/CgPhone/actions/runs/34388740867)) | Windows x64 | Beta actual validada para laboratorio | Sí, durante la etapa beta |
| [v0.3.1-beta.37](https://github.com/DarkCristian/CgPhone/releases/tag/v0.3.1-beta.37) | Windows x64 | Histórica; reemplazada por beta.38 | Sólo referencia |
| [v0.3.1-beta.35](https://github.com/DarkCristian/CgPhone/releases/tag/v0.3.1-beta.35) | Windows x64 | Histórica; reemplazada por beta.37 | Sólo referencia |
| Linux | Ubuntu, Debian, Mint, LMDE, Zorin, Arch y derivados | En desarrollo, sin paquete soportado | Evaluación comunitaria |

Versiones anteriores y artefactos generados por otros workflows no deben
considerarse equivalentes aunque compartan una misma versión base. Cada archivo debe
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

## Evidencia de v0.3.2-beta.38

| Campo | Valor |
|---|---|
| Workflow validado | [Windows Free installer #38](https://github.com/DarkCristian/CgPhone/actions/runs/34388740867) |
| Commit exacto del build | `6d752cbbf5115a3a1f1f30db6d4eeddc41c06cf5` |
| PJPROJECT/PJSUA2 | 2.17 más parches upstream, commit `a1b707c0c9b0506faf2a8a438b60f11ffd6a6fd9` |
| Instalador | `CgPhone-Setup-0.3.2-x64.exe` |
| SHA-256 instalador | `cf3224819d10e892c250efb35f961b36caa9ac30c0dd377ef6d8e4777161eb09` |
| Portable | `CgPhone-0.3.2-windows-x64-portable.zip` |
| SHA-256 portable | `e6fb2a737a0729b642d5d3e4d7285d8ef153d5586be03493536d9d255a50c7d3` |
| SBOM | `CgPhone-0.3.2-sbom.spdx.json` (SPDX 2.3; 231 archivos; 16 paquetes) |
| SHA-256 SBOM | `68b4afae287052cba2f0be05808a3b668c1e599d04e835798301e9d59e2d59aa` |
| SHA-256 reporte Defender | `0f100027a17286a6ec3b9cfe1619b784a963021740a3418dd0e837eacc32ce40` |
| SHA-256 manifiesto | `e1e9adbf001bdb6da0ab3efced9d3a9318723e4bf319f5a84b8c2024af505491` |
| Microsoft Defender | `CLEAN`, cero detecciones |
| Firma Authenticode | No aplicada; SmartScreen puede advertir en la primera ejecución |
| Validación funcional | Completada satisfactoriamente con Asterisk y Neotel |
| Clasificación | Beta validada para laboratorio controlado |

Beta.38 incorpora los parches oficiales de PJPROJECT para CVE-2026-57159,
CVE-2026-57160 y CVE-2026-57162. La validación manual confirmó que portable,
instalador y operación SIP mantienen el comportamiento esperado. La advertencia
inicial de SmartScreen responde a la ausencia de una firma Authenticode pública
y no queda resuelta por hashes, SBOM o atestaciones.

## Evidencia histórica de v0.3.1-beta.37

| Campo | Valor |
|---|---|
| Workflow validado | [Windows Free installer #37](https://github.com/DarkCristian/CgPhone/actions/runs/34141605443) |
| Commit exacto del build/tag | `bbd61191872e91f94eb0be53b1c5078bc47e3b1f` |
| Instalador | `CgPhone-Setup-0.3.1-x64.exe` |
| SHA-256 instalador | `9cfd527a5991490f34a87e000439e0b7fa7385a8db8c41eb7355b1fe7b21c900` |
| Portable | `CgPhone-0.3.1-windows-x64-portable.zip` |
| SHA-256 portable | `3fbff9df4abf20d42e1af0e08dfbe770e19b5a919e2b7f8f2452dd452b0f6241` |
| SBOM | `CgPhone-0.3.1-sbom.spdx.json` (SPDX 2.3; 231 archivos; 16 paquetes) |
| SHA-256 SBOM | `4c6c5a3485be72e941d8c2ff7912dd80aa75bc016f1ecc4d7c63471e4b54166f` |
| SHA-256 reporte Defender | `2e5a8169087f4410dc6eab98ad94baad77ba33221effe27d6a505f02e79e6ccf` |
| SHA-256 manifiesto | `1572fa0f2b4f54106e3ebff530254830c5de9cc3f85e6775bb251fb9ac022832` |
| Firma Authenticode | No aplicada |
| Clasificación | Beta de laboratorio / pre-release |
| Validación funcional | Matriz de beta.35 replicada satisfactoriamente en beta.37 contra Asterisk y Neotel |

Los hashes fueron recalculados desde los assets extraídos y coincidieron con
`SHA256SUMS.txt`. La atestación de GitHub vincula los archivos con el workflow
y commit de origen, pero no equivale a Authenticode. La matriz funcional
previamente validada en beta.35 se replicó satisfactoriamente en beta.37 contra
centrales Asterisk y Neotel.

### Evidencia antivirus/EDR

- Microsoft Defender analizó instalador, portable, ZIP y árbol instalado: cero
  detecciones (`CLEAN`).
- Trellix Endpoint Security: prueba manual del portable y del runtime instalado,
  seguida de análisis rápido de 302.857 elementos con cero detecciones.
- No se observó un evento nuevo atribuible a beta.37 durante esa validación.
- El resultado histórico de VirusTotal corresponde al hash de beta.35 y no debe
  trasladarse a beta.37; queda pendiente repetirlo sobre los hashes actuales.

Un evento previo de Trellix correspondía a una regla corporativa de Protección
de acceso asociada a ejecutar desde el navegador archivos ubicados en Descargas.
La política estaba en auditoría (“bloquearía”): no fue una detección de malware.
La ejecución desde la ubicación estándar de aplicaciones no reprodujo ese evento
en la prueba actual. El resultado puede variar según la configuración de cada
organización.

Ante una alerta: verificar hash y origen, no desactivar el EDR, no excluir
carpetas completas, distribuir desde una ubicación corporativa controlada,
solicitar a Seguridad el análisis del módulo/regla/acción/hash y limitar una
autorización temporal al hash exacto. Si hay detección o cuarentena, detener el
despliegue y enviar la muestra a Trellix. No publicar logs de endpoints, rutas,
usuarios, dominios, direcciones ni políticas internas.

## Evidencia histórica de v0.3.1-beta.35

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
| Validación SIP | Pruebas manuales satisfactorias con Asterisk y Neotel |

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

Los EDR corporativos, incluido Trellix, pueden generar alertas según las
políticas configuradas por cada organización y por tratarse de binarios todavía
no firmados. Una alerta de política o reputación no equivale por sí sola a una
detección de malware.

## Atestación de compilaciones

El [workflow de atestaciones](https://github.com/DarkCristian/CgPhone/actions/runs/33923023903)
se ejecutó correctamente sobre la rama `security/artifact-attestations`. El
manifiesto `SHA256SUMS.txt` coincidió con el instalador, portable y SBOM del
mismo run, y GitHub completó la atestación de procedencia. Beta.37 es la primera pre-release que publica el manifiesto, el SBOM y la
evidencia Defender generados por este flujo.

## Microsoft Defender

El [workflow #37](https://github.com/DarkCristian/CgPhone/actions/runs/34141605443)
validó el gate automatizado sobre el instalador, el runtime portable, el ZIP y
el árbol de una instalación silenciosa. Antes del análisis actualizó la
inteligencia de seguridad y comprobó que Defender estuviera activo.

| Campo | Resultado |
|---|---|
| Fecha UTC | 2026-09-07 16:18:51 |
| Commit analizado | `bbd61191872e91f94eb0be53b1c5078bc47e3b1f` |
| Motor | 1.1.26080.3 |
| Plataforma | 4.18.26080.3 |
| Inteligencia de seguridad | 1.459.97.0 |
| Detecciones | 0 |
| Resultado | `CLEAN` |
| SHA-256 del reporte | `2e5a8169087f4410dc6eab98ad94baad77ba33221effe27d6a505f02e79e6ccf` |

El reporte excluye hostname, usuario, rutas locales, registros SIP y datos
corporativos. El gate falla si Defender no está disponible, el análisis no
termina o aparece una detección. Esta evidencia corresponde al build #37 y a los hashes publicados en beta.37;
no modifica retroactivamente beta.35.

## Remediación de seguridad integrada en v0.3.2-beta.38

La versión v0.3.2-beta.38 se generó desde la rama
`security/pjproject-cve-fixes-beta-38` y reemplaza beta.37 como baseline. Mantiene PJPROJECT 2.17 como base compatible y fija el
código exactamente al commit oficial
[`a1b707c0c9b0506faf2a8a438b60f11ffd6a6fd9`](https://github.com/pjsip/pjproject/commit/a1b707c0c9b0506faf2a8a438b60f11ffd6a6fd9),
que contiene los parches publicados por upstream para:

- [CVE-2026-57159](https://github.com/pjsip/pjproject/security/advisories/GHSA-rfwg-w9gq-9mw2):
  validación de límites en payload types SDP dinámicos;
- [CVE-2026-57160](https://github.com/pjsip/pjproject/security/advisories/GHSA-277r-3q2j-mxcw):
  escritura fuera de límites al serializar encabezados SIP genéricos;
- [CVE-2026-57162](https://github.com/pjsip/pjproject/security/advisories/GHSA-m9g3-jcj8-qjfm):
  límite de atributos `a=crypto` en SRTP/SDES.

Además, el build fija explícitamente `PJMEDIA_HAS_SRTP=0`,
`PJMEDIA_HAS_VIDEO=0` y
`PJMEDIA_SDP_NEG_MAINTAIN_REMOTE_PT_MAP=0`. Estas defensas no sustituyen los
parches: ambos controles se aplican simultáneamente.

El [workflow #38](https://github.com/DarkCristian/CgPhone/actions/runs/34388740867)
finalizó correctamente sobre el commit
`6d752cbbf5115a3a1f1f30db6d4eeddc41c06cf5`. Compiló el PJPROJECT fijado,
validó el instalador contra el árbol portable, obtuvo cero detecciones de
Microsoft Defender (`CLEAN`), generó SBOM SPDX 2.3 y completó la atestación.

| Archivo beta.38 | SHA-256 |
|---|---|
| `CgPhone-Setup-0.3.2-x64.exe` | `cf3224819d10e892c250efb35f961b36caa9ac30c0dd377ef6d8e4777161eb09` |
| `CgPhone-0.3.2-windows-x64-portable.zip` | `e6fb2a737a0729b642d5d3e4d7285d8ef153d5586be03493536d9d255a50c7d3` |
| `CgPhone-0.3.2-sbom.spdx.json` | `68b4afae287052cba2f0be05808a3b668c1e599d04e835798301e9d59e2d59aa` |
| `Microsoft-Defender-scan-report.txt` | `0f100027a17286a6ec3b9cfe1619b784a963021740a3418dd0e837eacc32ce40` |

Beta.38 fue validada funcionalmente con Asterisk y Neotel. Los hashes de
beta.37 no son válidos para estos binarios.

## Dependencias observadas en el build #38

Estas versiones provienen del archivo
`COMPLIANCE/DEPENDENCY-VERSIONS.txt` incluido en el portable de beta.38:

| Componente | Versión/revisión observada |
|---|---|
| Qt Base | 6.11.2-2 |
| Qt Declarative / Multimedia / SVG | 6.11.2-1 |
| PJPROJECT/PJSUA2 | 2.17 con parches, commit `a1b707c0c9b0506faf2a8a438b60f11ffd6a6fd9` |
| OpenSSL | 3.6.4-1 |
| FFmpeg | 9.0.1-3 |
| Opus | 1.6.1-1 |
| LAME | 3.100-3 |
| GCC/MinGW | 16.2.0-3 |
| CMake | 4.4.3-2 |

MSYS2 utiliza paquetes móviles. Esta evidencia describe el build #38, pero no
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
- pruebas SIP en laboratorio con Asterisk y Neotel, repetibles por release;
- revisión de logs y permisos;
- firma Authenticode y timestamp cuando esté disponible;
- publicación del código fuente y licencias correspondientes.

SmartScreen, una firma válida, un hash coincidente o un antivirus sin
detecciones son evidencias independientes; ninguna demuestra por sí sola que el
software sea seguro.

Los controles gratuitos pendientes —licencia explícita del proyecto, textos de
terceros, CodeQL, análisis de CVE, secret scanning, ACL y sanitización de logs—
se mantienen en el
[checklist de compliance](docs/SECURITY_COMPLIANCE.md#controles-pendientes-que-no-requieren-pagar).
