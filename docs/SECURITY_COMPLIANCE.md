# Seguridad y compliance de CgPhone Free

Última revisión: 2026-09-08 (beta.37 y validación Trellix sanitizada).

## Alcance actual

CgPhone Free v0.3.1-beta.37 es una pre-release para laboratorio controlado con
centrales SIP como Asterisk o Neotel. Windows x64 dispone de instalador y
portable. La versión nativa para Linux continúa en desarrollo y todavía no se
distribuye como paquete soportado.

CgPhone Free es la única línea mantenida. No integra Qt WebEngine ni Qt WebView
y no existe una edición Pro/Full mantenida.

## Evidencia del build v0.3.1-beta.37

| Campo | Valor |
|---|---|
| Workflow | [Windows Free installer #37](https://github.com/DarkCristian/CgPhone/actions/runs/34141605443) |
| Commit exacto del build/tag | `bbd61191872e91f94eb0be53b1c5078bc47e3b1f` |
| Tag publicado | `v0.3.1-beta.37` |
| Instalador | `CgPhone-Setup-0.3.1-x64.exe` |
| SHA-256 instalador | `9cfd527a5991490f34a87e000439e0b7fa7385a8db8c41eb7355b1fe7b21c900` |
| Portable | `CgPhone-0.3.1-windows-x64-portable.zip` |
| SHA-256 portable | `3fbff9df4abf20d42e1af0e08dfbe770e19b5a919e2b7f8f2452dd452b0f6241` |
| SBOM | `CgPhone-0.3.1-sbom.spdx.json` (SPDX 2.3) |
| Alcance del SBOM | 231 archivos; 16 paquetes |
| SHA-256 SBOM | `4c6c5a3485be72e941d8c2ff7912dd80aa75bc016f1ecc4d7c63471e4b54166f` |
| SHA-256 reporte Defender | `2e5a8169087f4410dc6eab98ad94baad77ba33221effe27d6a505f02e79e6ccf` |
| SHA-256 `SHA256SUMS.txt` | `1572fa0f2b4f54106e3ebff530254830c5de9cc3f85e6775bb251fb9ac022832` |
| Firma Authenticode | No aplicada |
| Clasificación | Beta de laboratorio / pre-release |

Los hashes del instalador, portable, SBOM y reporte Defender fueron recalculados
y coincidieron con el manifiesto. Defender finalizó con cero detecciones. La
prueba manual con Trellix cubrió portable, instalación y ejecución desde la
ubicación estándar de aplicaciones; el análisis rápido examinó 302.857 elementos
con cero detecciones y no se observó un evento nuevo atribuible a beta.37.

Esto no demuestra ausencia total de vulnerabilidades. El resultado se limita a
los motores, firmas, políticas, fecha y archivos exactos evaluados. La matriz funcional validada en beta.35 se replicó satisfactoriamente en
beta.37 contra Asterisk y Neotel. Esta evidencia corresponde a los artefactos y
entornos ensayados y debe repetirse para cada release posterior.

## Evidencia histórica

Beta.35 conserva su Release, hashes, SBOM y validación funcional con Asterisk y
Neotel como evidencia histórica independiente. Beta.37 cuenta con su propia
replicación satisfactoria de esa matriz funcional.

## Baseline observada en el artefacto #37

| Componente | Versión observada | Evaluación actual |
|---|---:|---|
| Qt Base | 6.11.2-2 | Reevaluar advisories en cada build |
| Qt Declarative / Multimedia / SVG | 6.11.2-1 | Sólo módulos efectivamente incluidos |
| PJPROJECT/PJSIP | 2.17, commit `5a457451fa2712ba18e12b01738e8ff3af2b26fd` | Revisar y aplicar parches antes de producción |
| FFmpeg | 9.0.1-3 | Revisar componentes transitivos incluidos por Qt |
| OpenSSL | 3.6.4-1 | Registrar y verificar en cada build |
| Opus | 1.6.1-1 | Incluido para audio |
| LAME | 3.100-3 | MP3 requiere binario x64 y licencia verificados |
| GCC/MinGW | 16.2.0-3 | Toolchain del build #37 |
| CMake | 4.4.3-2 | Herramienta del build #37 |

La evidencia proviene de `COMPLIANCE/BUILD-INFO.txt` y
`COMPLIANCE/DEPENDENCY-VERSIONS.txt` incluidos en el portable de beta.37. MSYS2 se
actualiza durante el workflow: esta tabla registra lo utilizado, pero no fija
versiones de forma reproducible. El SBOM publicado complementa esta evidencia:
enumera el árbol final, sus checksums y las relaciones conocidas. Los campos de
licencia que todavía no cuentan con evidencia suficiente se declaran
`NOASSERTION`; eso evita afirmar licencias no verificadas y mantiene pendiente
la revisión legal de los textos redistribuidos.

## Advisories analizados

### Qt

- Las vulnerabilidades deben evaluarse según los módulos realmente enlazados y
  las entradas que CgPhone procese.
- CgPhone Free no integra Qt WebEngine ni Qt WebView; los CVE de Chromium no
  deben marcarse como aplicables sin que esos módulos sean incorporados.
- Los avisos de Qt SVG tienen menor alcanzabilidad cuando la aplicación no
  procesa SVG aportado por usuarios, pero el módulo incluido debe mantenerse
  actualizado.
- CVE-2026-15037 corresponde a QtXml/QDom. La rama Free no enlaza QtXml
  actualmente; debe reevaluarse si eso cambia.

### PJPROJECT/PJSIP

- CVE-2026-57160 / GHSA-277r-3q2j-mxcw afecta versiones hasta 2.17 inclusive.
- CVE-2026-57162 / GHSA-m9g3-jcj8-qjfm se relaciona con negociación
  SRTP/SDES. Esa función no debe habilitarse sin integrar y probar el parche.
- GHSA-rfwg-w9gq-9mw2 se relaciona con payload types mantenidos remotamente. La
  opción debe permanecer deshabilitada.
- Video, CLI Telnet, cliente HTTP y otros componentes no utilizados deben
  eliminarse del build cuando sea posible para reducir superficie.

La falta de alcanzabilidad demostrada puede reducir el riesgo operativo, pero
no corrige una dependencia vulnerable ni evita hallazgos de un scanner.

## Estado de los controles

| Control | Estado |
|---|---|
| Build automatizado Windows x64 | Implementado |
| Instalador Inno Setup | Implementado y probado en #37 |
| SHA-256 de instalador, portable, SBOM y reporte | Publicados en beta.37 |
| Portable con información de build | Implementado |
| Instalación, desinstalación y limpieza opcional | Validadas manualmente en beta.37 |
| Pruebas funcionales SIP | Matriz de beta.35 replicada satisfactoriamente en beta.37 con Asterisk y Neotel |
| Firma Authenticode pública | Pendiente |
| Timestamp RFC 3161 | Pendiente |
| SBOM SPDX 2.3 de beta.37 | Generado y publicado como asset independiente |
| Generación automática de SBOM | Incorporada al workflow para futuros builds |
| SHA256SUMS y atestaciones de artefactos | Workflow validado correctamente en run 33923023903 |
| OSV-Scanner o Trivy | Pendiente |
| CodeQL C/C++ | No existe workflow activo |
| Microsoft Defender sobre paquete final | Validado en workflow #37: 0 detecciones |
| Trellix Endpoint Security | Portable, instalado y análisis rápido: 0 detecciones; sin evento nuevo atribuible |
| Reproducibilidad con dependencias fijadas | Pendiente |
| Paquete Linux | En desarrollo |

## Gate para una release estable

- [ ] Fijar Qt, toolchain y paquetes MSYS2 por versión y registrar hashes.
- [x] Registrar la revisión exacta de PJPROJECT usada en el build #37.
- [ ] Validar y mergear la candidata beta.38, que integra y documenta los parches PJSIP aplicables.
- [ ] Mantener SRTP/SDES deshabilitado hasta parchearlo y probarlo.
- [ ] Deshabilitar video, CLI Telnet, HTTP client y módulos no usados.
- [x] Generar y publicar SBOM SPDX 2.3 del runtime de beta.37.
- [x] Automatizar el SBOM dentro de `COMPLIANCE/` y como artefacto independiente.
- [ ] Ejecutar OSV-Scanner o Trivy y resolver hallazgos altos/críticos.
- [ ] Incorporar CodeQL para C/C++ y revisar sus resultados.
- [x] Validar por workflow el análisis de Defender sobre instalador, portable, ZIP y árbol instalado (#37).
- [x] Publicar SHA-256 del instalador, portable, SBOM, reporte Defender y manifiesto de beta.37.
- [ ] Firmar ejecutables propios e instalador con Authenticode.
- [ ] Aplicar timestamp de una autoridad confiable.
- [ ] Verificar firma mediante `signtool verify /pa /all /v`.
- [x] Completar la matriz formal de pruebas SIP de beta.37 con Asterisk y Neotel.
- [x] Verificar instalación, desinstalación y limpieza opcional de configuración.
- [ ] Sanitizar logs y comprobar que no contengan secretos.
- [ ] Revisar ACL de configuración, logs y grabaciones.
- [x] Publicar código fuente correspondiente, licencias y avisos.
- [ ] Obtener aceptación formal antes de un despliegue productivo.

## Controles pendientes que no requieren pagar

Estos controles pueden completarse con GitHub Actions, herramientas libres y
pruebas internas. No eliminan SmartScreen ni sustituyen Authenticode:

- [ ] Elegir y publicar una licencia explícita para el código propio de CgPhone.
- [ ] Adjuntar los textos completos y avisos de todas las dependencias
  redistribuidas; resolver cada `NOASSERTION` del SBOM.
- [ ] Fijar acciones de GitHub por SHA de commit y activar Dependabot para
  mantenerlas actualizadas.
- [ ] Activar CodeQL C/C++ y corregir o justificar sus hallazgos.
- [ ] Analizar el SBOM con OSV-Scanner, Grype o Trivy y documentar el triage de
  CVE según versión, módulo y alcanzabilidad real.
- [x] Validar el gate automatizado de Microsoft Defender y conservar su reporte
  sanitizado (workflow #37).
- [x] Probar beta.37 con Trellix Endpoint Security: portable, runtime instalado y
  análisis rápido con cero detecciones; sin publicar evidencia sensible.
- [x] Generar una atestación de procedencia del build y conservar el vínculo
  workflow → commit → artefacto → hash → SBOM (validado en run 33923023903).
- [ ] Automatizar la búsqueda de secretos y revisar también el historial Git.
- [ ] Sanitizar logs SIP y probar que nunca registren Authorization, contraseñas
  ni datos internos innecesarios.
- [ ] Verificar ACL de `ProgramData`, logs y grabaciones con usuario estándar y
  administrador.
- [ ] Restringir firewall a PBX/SBC y puertos SIP/RTP autorizados para el piloto.
- [x] Completar y conservar la matriz funcional SIP de beta.37 con Asterisk y Neotel.
- [ ] Documentar falso positivo/revisión del proveedor si Trellix o cualquier
  motor mantiene una detección sobre el hash exacto publicado.
- [ ] Probar instalación, actualización y desinstalación en una VM limpia y en
  una terminal piloto con las políticas corporativas activas.

El workflow de la rama `security/artifact-attestations` genera
`SHA256SUMS.txt` y atestaciones de GitHub Actions basadas en Sigstore para el
instalador, el portable, el SBOM y el manifiesto. El run
[33923023903](https://github.com/DarkCristian/CgPhone/actions/runs/33923023903)
finalizó correctamente: los hashes coincidieron y el paso de atestación fue
exitoso. Este control aporta integridad y procedencia pública, pero **no es una
firma Authenticode** y no crea reputación en Microsoft SmartScreen.

## Remediación PJPROJECT preparada en beta.38

La candidata `v0.3.2-beta.38` fija PJPROJECT al commit upstream
[`a1b707c0c9b0506faf2a8a438b60f11ffd6a6fd9`](https://github.com/pjsip/pjproject/commit/a1b707c0c9b0506faf2a8a438b60f11ffd6a6fd9).
Ese punto contiene las correcciones oficiales para CVE-2026-57159,
CVE-2026-57160 y CVE-2026-57162. El workflow comprueba el commit exacto antes
de compilar y registra esa revisión en el SBOM y
`COMPLIANCE/DEPENDENCY-VERSIONS.txt`.

Como defensa adicional, SRTP, vídeo y el remapeo remoto de payload types quedan
deshabilitados explícitamente.

El [workflow #38](https://github.com/DarkCristian/CgPhone/actions/runs/34388740867)
completó compilación, pruebas automáticas, validación del árbol instalado,
Defender `CLEAN`, SBOM, SHA-256 y atestación. Los hashes principales son:
`cf3224819d10e892c250efb35f961b36caa9ac30c0dd377ef6d8e4777161eb09`
(instalador) y
`e6fb2a737a0729b642d5d3e4d7285d8ef153d5586be03493536d9d255a50c7d3`
(portable). Antes de fusionar o publicar todavía se debe repetir la matriz
funcional con Asterisk y Neotel.

## Gate automatizado de Microsoft Defender

El [workflow #37](https://github.com/DarkCristian/CgPhone/actions/runs/34141605443)
validó el gate: motor 1.1.26080.3, plataforma 4.18.26080.3, inteligencia
1.459.97.0, cuatro objetivos analizados y cero detecciones (`CLEAN`). El reporte
sanitizado tiene SHA-256
`2e5a8169087f4410dc6eab98ad94baad77ba33221effe27d6a505f02e79e6ccf`.
Ante una detección, ausencia del motor o error, el workflow falla y aun así
publica el reporte sin hostname, usuario, rutas, logs SIP ni datos de la empresa.

El resultado corresponde al commit
`bbd61191872e91f94eb0be53b1c5078bc47e3b1f` y no se atribuye
retroactivamente a `v0.3.1-beta.35`. Tampoco sustituye Trellix, VirusTotal,
revisión de CVE, firma Authenticode ni pruebas funcionales.

## Firma, SmartScreen y antivirus

El certificado y su clave privada nunca deben guardarse en el repositorio.
Cuando se implemente un servicio de firma o una CA compatible, el orden debe ser:

1. compilar y desplegar las dependencias;
2. firmar los ejecutables propios;
3. verificar las firmas;
4. generar el instalador;
5. firmar y verificar el instalador;
6. calcular hashes y publicar artefactos.

Mientras no exista una firma pública, la beta debe advertir claramente sobre
SmartScreen y publicar hashes verificables. Un certificado autofirmado puede
servir dentro de una PKI controlada, pero no crea confianza pública.

Un motor de VirusTotal marcó heurísticamente el instalador #35. Una detección
aislada no permite concluir que el archivo sea malware ni confirmar un falso
positivo. Debe conservarse el nombre del motor, la firma de detección y la fecha
del análisis, y solicitar revisión al proveedor si el hallazgo persiste.

### Trellix Endpoint Security

La validación de beta.37 no reprodujo un evento nuevo atribuible al portable o
al runtime instalado y el análisis rápido finalizó con cero detecciones. Un
evento histórico correspondía a una regla corporativa de Protección de acceso
para archivos iniciados por un navegador desde Descargas, configurada en modo
auditoría (“bloquearía”). No fue una detección de malware.

La instalación en la ubicación estándar de aplicaciones separa la ejecución
cotidiana de esa regla específica. Aun así, el instalador descargado, un cambio
de hash o una política diferente pueden producir avisos. Procedimiento:

1. verificar SHA-256, tag y origen oficial;
2. identificar módulo, regla, acción y hash sin divulgar datos internos;
3. no desactivar Trellix ni excluir Descargas o la carpeta completa;
4. distribuir mediante una ubicación/herramienta corporativa controlada;
5. si es imprescindible para el piloto, autorizar sólo el hash exacto;
6. ante detección o cuarentena, detener el despliegue y remitir la muestra a
   Trellix para análisis.

Los EDR corporativos, incluido Trellix, pueden generar alertas según las
políticas de seguridad de cada organización y por tratarse de binarios todavía
no firmados. Una alerta de política, ejecución desde Descargas o reputación no
equivale por sí sola a una detección de malware. Los logs completos de endpoints
no deben publicarse porque pueden revelar usuarios, dominios, rutas, direcciones
de red y políticas internas; para evidencia pública sólo deben usarse resúmenes
sanitizados vinculados al SHA-256 exacto del artefacto.

## Publicación y licencias

La edición Free debe publicarse con el código fuente correspondiente al binario,
los textos de licencia y las modificaciones realizadas. Esto es especialmente
relevante para PJPROJECT/PJSUA2 y para los términos aplicables de Qt.

Los aportes mediante Ko-fi o Tecito financian el mantenimiento, pero no
convierten el software libre en una licencia comercial ni eliminan las
obligaciones de redistribución.

## Evidencia pendiente por release

Cada release debe conservar:

- commit exacto del build y commit del tag;
- fecha UTC;
- versiones de compilador y dependencias;
- SBOM;
- resultados de escaneo;
- SHA-256 de cada binario distribuido;
- resultado de firma;
- resultado de antivirus;
- checklist funcional;
- notas de cambios y limitaciones conocidas.

Los assets de beta.37, incluido el SBOM, el manifiesto y el reporte Defender,
se publican en la [Release v0.3.1-beta.37](https://github.com/DarkCristian/CgPhone/releases/tag/v0.3.1-beta.37). Beta.35 permanece disponible como evidencia histórica.
