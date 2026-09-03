# Seguridad y compliance de CgPhone Free

Última revisión: 2026-09-03.

## Alcance actual

CgPhone Free 0.3.1 es una beta para laboratorio controlado con centrales SIP
como Asterisk o Neotel. Windows x64 dispone de instalador y portable. La versión
nativa para Linux continúa en desarrollo y todavía no se distribuye como
paquete soportado.

No existe actualmente una release productiva aprobada. El proyecto Pro quedó
fuera del alcance de mantenimiento y sus ramas experimentales no deben
publicarse como versiones soportadas.

## Evidencia del build 0.3.1

| Campo | Valor |
|---|---|
| Workflow | Windows Free installer |
| Run exitoso | #30 |
| Rama | `release/free-0.3.1` |
| Commit | `c548e15b2545ab79b2eeea14c5dd2767d0b6a4ef` |
| Instalador | `CgPhone-Setup-0.3.1-x64.exe` |
| SHA-256 | `85a4744787827e2816865b0538ffe4f8d7157dc1be79abb26c27007776375b73` |
| Firma Authenticode | No aplicada |
| Clasificación | Beta de laboratorio |

El run #29 falló únicamente durante la compilación del script de Inno Setup.
Después de corregir las comillas de la regla de desinstalación, el run #30
completó la aplicación, el instalador, el hash y la subida de artefactos.

Un workflow exitoso confirma que el paquete fue generado, pero no sustituye las
pruebas funcionales ni una auditoría de seguridad.

## Baseline observada

| Componente | Versión observada | Evaluación actual |
|---|---:|---|
| Qt Base / QML / Multimedia | 6.11.2 | Reevaluar advisories en cada build |
| Qt WebEngine | No incluido | Sus CVE no son automáticamente aplicables |
| PJPROJECT/PJSIP | 2.17 | Bloqueante para producción sin parches revisados |
| FFmpeg | 9.0.1 | Revisar componentes transitivos incluidos por Qt |
| OpenSSL | 3.6.3 | Registrar y verificar en cada build |
| LAME | 3.100 | WAV es la opción segura predeterminada; MP3 exige binario x64 validado |

MSYS2 se actualiza durante el workflow. Esta tabla registra una observación,
pero no fija versiones de forma reproducible ni reemplaza el SBOM del
artefacto.

## Advisories analizados

### Qt

- Las vulnerabilidades deben evaluarse según los módulos realmente enlazados y
  las entradas que CgPhone procese.
- Los avisos de Qt SVG o VectorImage tienen menor alcanzabilidad cuando la
  aplicación no procesa SVG aportado por usuarios, pero el módulo incluido debe
  mantenerse actualizado.
- CVE-2026-15037 corresponde a QtXml/QDom. La rama Free no enlaza QtXml
  actualmente; debe reevaluarse si eso cambia.
- La rama Free no incluye Qt WebEngine ni Chromium embebido. La futura
  publicación Linux tampoco debe asumir aplicabilidad sin revisar el paquete
  concreto.

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
| Instalador Inno Setup | Implementado en rama de release |
| SHA-256 del instalador | Implementado |
| Portable con información de build | Implementado |
| Pruebas funcionales SIP | Manuales, requieren repetición por release |
| Firma Authenticode pública | Pendiente |
| Timestamp RFC 3161 | Pendiente |
| SBOM CycloneDX/SPDX | Pendiente |
| OSV-Scanner o Trivy | Pendiente |
| CodeQL C/C++ | No existe workflow activo |
| Microsoft Defender sobre paquete final | Pendiente de evidencia automatizada |
| Reproducibilidad con dependencias fijadas | Pendiente |
| Paquete Linux | En desarrollo |

## Gate para una release estable

- [ ] Fijar Qt, toolchain y paquetes MSYS2 por versión y registrar hashes.
- [ ] Fijar PJPROJECT a una revisión concreta.
- [ ] Integrar y documentar todos los parches PJSIP aplicables.
- [ ] Mantener SRTP/SDES deshabilitado hasta parchearlo y probarlo.
- [ ] Deshabilitar video, CLI Telnet, HTTP client y módulos no usados.
- [ ] Generar SBOM CycloneDX o SPDX por artefacto.
- [ ] Ejecutar OSV-Scanner o Trivy y resolver hallazgos altos/críticos.
- [ ] Incorporar CodeQL para C/C++ y revisar sus resultados.
- [ ] Analizar el árbol final y el instalador con Microsoft Defender.
- [x] Generar SHA-256 del instalador 0.3.1.
- [ ] Firmar ejecutables propios e instalador con Authenticode.
- [ ] Aplicar timestamp de una autoridad confiable.
- [ ] Verificar firma mediante `signtool verify /pa /all /v`.
- [ ] Probar registro, entrantes, salientes, DTMF, hold, transferencia y audio.
- [ ] Verificar instalación, actualización y desinstalación.
- [ ] Sanitizar logs y comprobar que no contengan secretos.
- [ ] Revisar ACL de configuración, logs y grabaciones.
- [ ] Publicar código fuente correspondiente, licencias y avisos.
- [ ] Obtener aceptación formal antes de un despliegue productivo.

## Firma y SmartScreen

El certificado y su clave privada nunca deben guardarse en el repositorio.
Cuando se implemente Azure Artifact Signing o una CA compatible, el orden debe
ser:

1. compilar y desplegar las dependencias;
2. firmar los ejecutables propios;
3. verificar las firmas;
4. generar el instalador;
5. firmar y verificar el instalador;
6. calcular hashes y publicar artefactos.

El plan de firma administrada tiene un costo recurrente. Mientras el proyecto
se financie mediante aportes y no disponga de esa firma, la beta debe advertir
claramente sobre SmartScreen y publicar hashes verificables.

Un certificado autofirmado puede servir dentro de una PKI controlada, pero no
crea confianza pública ni garantiza eliminar SmartScreen.

## Publicación y licencias

La edición Free debe publicarse con el código fuente correspondiente al binario,
los textos de licencia y las modificaciones realizadas. Esto es especialmente
relevante para PJPROJECT/PJSUA2 y para los términos aplicables de Qt.

Los aportes mediante Ko-fi o Tecito financian el mantenimiento, pero no
convierten el software libre en una licencia comercial ni eliminan las
obligaciones de redistribución.

## Evidencia pendiente por release

Cada release debe conservar:

- commit exacto;
- fecha UTC;
- versiones de compilador y dependencias;
- SBOM;
- resultados de escaneo;
- SHA-256;
- resultado de firma;
- resultado de antivirus;
- checklist funcional;
- notas de cambios y limitaciones conocidas.
