# Seguridad y compliance de CgPhone

Última revisión: 2026-08-21.

## Alcance y clasificación

El portable actual es una beta para laboratorio controlado con Asterisk/Neotel.
No es una release aprobada para producción. SmartScreen, la firma digital y el
análisis de dependencias son controles diferentes: superar uno no demuestra el
cumplimiento de los demás.

## Baseline observado en GitHub Actions

| Componente | Versión observada | Estado inicial |
|---|---:|---|
| Qt Base / QML / Multimedia | 6.11.2 | Sin CVE crítico conocido aplicable al uso actual |
| PJPROJECT/PJSIP | 2.17 | Bloqueante de producción: existen advisories posteriores |
| FFmpeg | 9.0.1 | Revisar CVE y reducir componentes incluidos |
| OpenSSL | 3.6.3 | Verificar en cada build |
| LAME | 3.100 | Sólo usar una distribución x64 validada y con procedencia |

El workflow actualmente actualiza paquetes de MSYS2. Por lo tanto, esta tabla
no sustituye el manifiesto generado por cada ejecución.

## Advisories evaluados

### Qt

- El problema `dr_wav` de Qt Multimedia afecta 6.10.0 a 6.10.2; la baseline
  observada 6.11.2 queda fuera del rango.
- Los fallos de Qt SVG y VectorImage evaluados están corregidos en 6.11.2 y
  CgPhone no admite SVG aportado por usuarios.
- CVE-2026-9499 de Qt5Compat está corregido en 6.11.1; CgPhone no enlaza
  Qt5Compat.
- CVE-2026-15037 afecta Qt anterior a 6.12 en QtXml/QDom. CgPhone no enlaza
  QtXml actualmente. Debe reevaluarse si se incorpora ese módulo.

### PJPROJECT/PJSIP

- CVE-2026-57160 / GHSA-277r-3q2j-mxcw afecta hasta 2.17 inclusive.
- CVE-2026-57162 / GHSA-m9g3-jcj8-qjfm afecta SRTP/SDES y es alcanzable desde
  negociación SDP. CgPhone no debe habilitar SRTP hasta integrar el parche.
- GHSA-rfwg-w9gq-9mw2 afecta el mantenimiento remoto de payload types. La
  opción debe permanecer explícitamente deshabilitada.
- Funciones no usadas (CLI Telnet, cliente HTTP y video) deben eliminarse en
  compilación para reducir superficie y falsos positivos de auditoría.

Que una ruta vulnerable no sea alcanzable reduce el riesgo operativo, pero un
scanner puede seguir detectando la versión vulnerable. La aceptación requiere
parche, evidencia de compilación y análisis de alcanzabilidad documentado.

## Gate obligatorio para una versión final

- [ ] Fijar Qt, toolchain y paquetes MSYS2 por versión y conservar sus hashes.
- [ ] Fijar PJPROJECT a un commit revisado, no sólo al tag 2.17.
- [ ] Integrar todos los parches PJSIP aplicables y registrar sus commits.
- [ ] Deshabilitar SRTP/SDES hasta parchearlo y probarlo formalmente.
- [ ] Deshabilitar video, CLI Telnet, HTTP client y módulos no utilizados.
- [ ] Generar SBOM CycloneDX o SPDX por artefacto.
- [ ] Ejecutar OSV-Scanner o Trivy sobre el SBOM y bloquear severidad alta.
- [ ] Ejecutar CodeQL para C/C++ y revisar resultados.
- [ ] Ejecutar Microsoft Defender sobre el árbol portable final.
- [ ] Generar `SHA256SUMS.txt` y conservar el resultado del workflow.
- [ ] Firmar EXE, DLL propias e instalador con certificado corporativo.
- [ ] Aplicar timestamp de una autoridad confiable a la firma.
- [ ] Verificar la firma con `signtool verify /pa /all /v`.
- [ ] Probar registro, entrantes, salientes, DTMF, hold, transferencia y audio
      contra Asterisk y Neotel en una VLAN de laboratorio.
- [ ] Sanitizar logs y confirmar que no contienen contraseña SIP ni secretos.
- [ ] Obtener aceptación formal de Seguridad/Infraestructura.

## Evidencia producida por el build

Cada portable debe incluir `COMPLIANCE/` con:

- `BUILD-INFO.txt`: commit, fecha UTC, compilador y herramientas;
- `DEPENDENCY-VERSIONS.txt`: paquetes MSYS2 relevantes y revisión PJPROJECT;
- `SHA256SUMS.txt`: integridad de los archivos distribuidos.

El SBOM, los resultados de escaneo y la validación de firma deben adjuntarse al
release final o conservarse como artefactos del workflow.

## Firma digital

La firma requiere un certificado de code signing emitido o aprobado por la
empresa. El certificado y su clave privada nunca se guardan en el repositorio.
GitHub Actions debe recibirlos mediante secretos protegidos y sólo el workflow
de release puede utilizarlos. Un certificado autofirmado sirve para laboratorio
interno, pero no elimina SmartScreen ni constituye confianza pública.

