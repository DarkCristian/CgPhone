# Seguridad y compliance de CgPhone Free

Última revisión: 2026-09-04 (SBOM de beta.35 incorporado).

## Alcance actual

CgPhone Free v0.3.1-beta.35 es una pre-release para laboratorio controlado con
centrales SIP como Asterisk o Neotel. Windows x64 dispone de instalador y
portable. La versión nativa para Linux continúa en desarrollo y todavía no se
distribuye como paquete soportado.

CgPhone Free es la única línea mantenida. No integra Qt WebEngine ni Qt WebView
y no existe una edición Pro/Full mantenida.

## Evidencia del build v0.3.1-beta.35

| Campo | Valor |
|---|---|
| Workflow | Windows Free installer |
| Run exitoso y probado | #35 |
| Rama del build | `fix/uninstaller-remove-settings` |
| Commit exacto del build | `e4fbcb2623cc58dd2676fffeaa1cf73afbe14d41` |
| Tag publicado | `v0.3.1-beta.35` |
| Commit del tag/merge | `a6dba9e96240405b7725fb4fcf60e89554ee9bd0` |
| Instalador | `CgPhone-Setup-0.3.1-x64.exe` |
| SHA-256 instalador | `40dfdf77fac4cbb98d3131c0835445da4db77cb9da3ed45c229850f4da7f43c1` |
| Portable | `CgPhone-Free-0.3.1-Windows-x64-portable.zip` |
| SHA-256 portable | `70052f48df73a62670831760e36bb7366e6e62cc8d8012a719cc6943e4f8aa09` |
| SBOM | `CgPhone-0.3.1-beta.35-sbom.spdx.json` (SPDX 2.3) |
| Alcance del SBOM | 231 archivos del runtime, 17 paquetes/componentes |
| SHA-256 SBOM | `bc2031ea10e0a59209c01c38abd2dd696969e66ca69e0b4f943b9a75b476ca5a` |
| Firma Authenticode | No aplicada |
| Clasificación | Beta de laboratorio / pre-release |

El run #35 completó la aplicación, el instalador y los artefactos. La instalación
y desinstalación fueron verificadas manualmente, incluida la eliminación
opcional de la configuración SIP. Un workflow exitoso y una prueba funcional no
sustituyen una auditoría de seguridad.

## Baseline observada en el artefacto #35

| Componente | Versión observada | Evaluación actual |
|---|---:|---|
| Qt Base | 6.11.2-2 | Reevaluar advisories en cada build |
| Qt Declarative / Multimedia / SVG | 6.11.2-1 | Sólo módulos efectivamente incluidos |
| PJPROJECT/PJSIP | 2.17, commit `5a457451fa2712ba18e12b01738e8ff3af2b26fd` | Revisar y aplicar parches antes de producción |
| FFmpeg | 9.0.1-3 | Revisar componentes transitivos incluidos por Qt |
| OpenSSL | 3.6.4-1 | Registrar y verificar en cada build |
| Opus | 1.6.1-1 | Incluido para audio |
| LAME | 3.100-3 | MP3 requiere binario x64 y licencia verificados |
| GCC/MinGW | 16.2.0-3 | Toolchain del build #35 |
| CMake | 4.4.2-2 | Herramienta del build #35 |

La evidencia proviene de `COMPLIANCE/BUILD-INFO.txt` y
`COMPLIANCE/DEPENDENCY-VERSIONS.txt` incluidos en el portable. MSYS2 se
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
| Instalador Inno Setup | Implementado y probado en #35 |
| SHA-256 de instalador y portable | Implementado y publicado |
| Portable con información de build | Implementado |
| Instalación y desinstalación | Probadas manualmente en #35 |
| Pruebas funcionales SIP | Manuales, requieren repetición por release |
| Firma Authenticode pública | Pendiente |
| Timestamp RFC 3161 | Pendiente |
| SBOM SPDX 2.3 de beta.35 | Generado y publicado como asset independiente |
| Generación automática de SBOM | Incorporada al workflow para futuros builds |
| OSV-Scanner o Trivy | Pendiente |
| CodeQL C/C++ | No existe workflow activo |
| Microsoft Defender sobre paquete final | Pendiente de evidencia automatizada |
| Reproducibilidad con dependencias fijadas | Pendiente |
| Paquete Linux | En desarrollo |

## Gate para una release estable

- [ ] Fijar Qt, toolchain y paquetes MSYS2 por versión y registrar hashes.
- [x] Registrar la revisión exacta de PJPROJECT usada en el build #35.
- [ ] Integrar y documentar todos los parches PJSIP aplicables.
- [ ] Mantener SRTP/SDES deshabilitado hasta parchearlo y probarlo.
- [ ] Deshabilitar video, CLI Telnet, HTTP client y módulos no usados.
- [x] Generar y publicar SBOM SPDX 2.3 del runtime de beta.35.
- [x] Automatizar el SBOM dentro de `COMPLIANCE/` y como artefacto independiente.
- [ ] Ejecutar OSV-Scanner o Trivy y resolver hallazgos altos/críticos.
- [ ] Incorporar CodeQL para C/C++ y revisar sus resultados.
- [ ] Analizar el árbol final y el instalador con Microsoft Defender.
- [x] Generar y publicar SHA-256 del instalador y portable #35.
- [ ] Firmar ejecutables propios e instalador con Authenticode.
- [ ] Aplicar timestamp de una autoridad confiable.
- [ ] Verificar firma mediante `signtool verify /pa /all /v`.
- [ ] Completar una matriz formal de pruebas SIP por release.
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
- [ ] Ejecutar Microsoft Defender y Trellix ENS/EDR sobre el instalador y el
  runtime final; conservar fecha, motor, versión de firmas y resultado.
- [ ] Generar una atestación de procedencia del build y conservar el vínculo
  workflow → commit → artefacto → hash → SBOM.
- [ ] Automatizar la búsqueda de secretos y revisar también el historial Git.
- [ ] Sanitizar logs SIP y probar que nunca registren Authorization, contraseñas
  ni datos internos innecesarios.
- [ ] Verificar ACL de `ProgramData`, logs y grabaciones con usuario estándar y
  administrador.
- [ ] Restringir firewall a PBX/SBC y puertos SIP/RTP autorizados para el piloto.
- [ ] Completar y conservar la matriz funcional SIP por cada release.
- [ ] Documentar falso positivo/revisión del proveedor si Trellix o cualquier
  motor mantiene una detección sobre el hash exacto publicado.
- [ ] Probar instalación, actualización y desinstalación en una VM limpia y en
  una terminal piloto con las políticas corporativas activas.

Opcionalmente puede firmarse el SBOM y `SHA256SUMS.txt` con Sigstore/cosign de
forma gratuita. Esa firma aporta integridad y procedencia pública, pero **no es
una firma Authenticode** y no crea reputación en Microsoft SmartScreen.

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

El asset independiente de beta.35 es:
[`CgPhone-0.3.1-beta.35-sbom.spdx.json`](https://github.com/DarkCristian/CgPhone/releases/download/v0.3.1-beta.35/CgPhone-0.3.1-beta.35-sbom.spdx.json).
