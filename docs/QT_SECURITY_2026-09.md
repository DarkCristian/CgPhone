# Estado de los avisos de Qt (23 de septiembre de 2026)

CgPhone Free Windows usa Qt 6.11.2 y el build exige al menos esa versión.
Qt confirma que 6.11.2 ya corrige los tres avisos indicados abajo.
El chequeo `tests/check_qt_advisory_entrypoints.py` examina el código C++ y QML;
si aparece alguna de estas APIs, detiene el workflow para revisión manual.
Esto verifica el código propio, no constituye una auditoría de todas las
rutas internas de bibliotecas de terceros.

| Aviso | Componente y entrada | Windows | Linux experimental Debian 13 |
| --- | --- | --- | --- |
| [CVE-2026-78253](https://www.qt.io/blog/security-advisory-cve-2026-78253) | Qt Core, `QXmlStreamReader::readElementText()` con XML anidado; baja, CVSS 4.0 2.3 | Corregida en Qt 6.11.2 | Qt 6.8.2 está en el rango afectado; CgPhone no invoca esa API |
| [CVE-2026-79616](https://www.qt.io/blog/security-advisory-cve-2026-79616) | Qt Quick, `Context2D.path`/`PathSvg.path` con rutas no confiables; baja, CVSS 4.0 0.6 | Corregida en Qt 6.11.2 | Qt 6.8.2 está en el rango afectado; CgPhone no asigna rutas a estas propiedades |
| [CVE-2026-76151](https://www.qt.io/blog/security-advisory-cve-2026-76151) | Qt Network, `QNetworkAccessManager` y respuesta HTTP Cache-Control maliciosa; media, CVSS 4.0 4.6 | Corregida en Qt 6.11.2 | Qt 6.8.2 está en el rango afectado; CgPhone usa PJSIP para SIP y no usa QNetworkAccessManager |

## Límite de Linux

El .deb y el tar.xz experimentales enlazan dinámicamente Qt del sistema.
Debian 13 (trixie) ofrece Qt 6.8.2 como versión base, con parches de la
distribución que deben comprobarse por paquete. No se afirma que esa versión
contenga los tres parches anteriores. Cambiar el mínimo a Qt 6.8.9 haría
ininstalable el .deb actual de Debian 13; mezclar paquetes de Debian unstable
o experimental con Debian 13 no es una corrección segura.

El código propio no expone las tres rutas descritas. Mantener Linux
experimental, probar sólo con cuentas SIP de laboratorio y revisar el
estado de los parches Debian antes de promover esa plataforma. Si se
introduce importación XML, rutas de gráficos desde fuentes externas o
consumo HTTP, actualizar Qt y volver a evaluar los CVE antes de distribuir.
Una compilación Linux con Qt actualizado necesita pruebas de arranque,
audio y registro SIP, además de SHA-256, SBOM y atestaciones propios.

Los hashes históricos de RC1 describen sólo los archivos de esa release.
Los artefactos nuevos se verifican con el SHA256SUMS.txt generado por su
workflow y no heredan las pruebas Asterisk/Neotel del build #39.
