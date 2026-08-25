# Dossier técnico de seguridad y compliance para presentar CgPhone al CTO

## 1. Resumen ejecutivo

CgPhone es un softphone SIP de escritorio, multiplataforma, desarrollado con Qt y PJSIP para operar contra Asterisk y Neotel. El diseño separa dos contextos:

- **Operación diaria:** usuario estándar, sin elevación, para registrar la cuenta, emitir y recibir llamadas.
- **Instalación y configuración administrativa:** elevación controlada para instalar en `Program Files`, establecer la configuración común del equipo y registrar controles del sistema como Windows Defender Firewall.

El objetivo no es evadir controles corporativos. La propuesta es desplegar una aplicación identificable, inventariada, firmada y gobernada por políticas de dominio. La beta actual demuestra funcionalidad, pero la aceptación de producción queda condicionada a los controles marcados como **obligatorios antes de producción** en este documento.

## 2. Alcance y límites de confianza

### 2.1 Componentes principales

| Componente | Función | Límite de confianza |
|---|---|---|
| CgPhone/Qt | Interfaz, ciclo de vida, configuración y audio | Proceso local del usuario |
| PJSIP/PJPROJECT | Señalización SIP, SDP, RTP y dispositivos multimedia | Biblioteca nativa dentro del proceso |
| Asterisk/Neotel o SBC | Registro, autenticación, enrutamiento e IVR/early media | Infraestructura de telefonía corporativa |
| Windows Defender Firewall | Control de tráfico entrante | Política del endpoint/dominio |
| DPAPI de Windows | Protección de credenciales locales | Equipo Windows y sus ACL |
| LAME nativo opcional | Conversión local a MP3 | Dependencia binaria verificada |
| GitHub Actions | Compilación y generación del artefacto | Cadena de suministro de software |

### 2.2 Activos que se protegen

- Usuario y contraseña SIP.
- Audio de llamadas y grabaciones locales.
- Configuración SIP común del terminal.
- Integridad del ejecutable y sus DLL.
- Historial y registros de diagnóstico.
- Disponibilidad del puesto de atención.
- Infraestructura PBX/SBC y red corporativa.

### 2.3 Fuera del alcance de la aplicación

CgPhone no reemplaza:

- Segmentación de voz, SBC, ACL o protección antifraude de la central.
- Políticas corporativas de retención, consentimiento y acceso a grabaciones.
- EDR/antivirus, gestión de parches o hardening del endpoint.
- PKI corporativa y proceso de firma.
- Gestión central de identidad de agentes en Asterisk/Neotel.

Un administrador local o un endpoint ya comprometido permanece fuera del límite de protección efectivo de una aplicación de escritorio.

## 3. Flujos de red

| Flujo | Dirección | Protocolo | Destino | Motivo |
|---|---|---|---|---|
| Registro y señalización SIP | Saliente y respuestas entrantes | UDP; configurable según central | PBX, proxy o SBC | REGISTER, INVITE, respuestas y control de sesión |
| Audio RTP/RTCP | Bidireccional | UDP dinámico negociado por SDP | PBX, SBC o extremo autorizado | Voz, IVR, early media y audio de llamada |
| Actualizaciones | No implementadas automáticamente en esta beta | N/A | N/A | Evita tráfico externo no documentado |

Los puertos exactos dependen de la configuración de Asterisk/Neotel, del SBC y de la negociación SDP. El IVR previo a la atención y el early media deben provenir de la central; CgPhone no simula ese audio localmente.

## 4. Modelo de privilegios

1. El instalador se ejecuta una vez con privilegios administrativos.
2. Los binarios se instalan bajo `C:\Program Files\CgPhone`.
3. La configuración por terminal se administra en una sesión elevada.
4. Al guardar o descartar, la sesión administrativa termina.
5. La operación normal vuelve a ejecutarse sin privilegios elevados.
6. Las llamadas no deben solicitar UAC.

**Control:** no se incorpora un bypass de UAC. Si un endpoint solicita elevación durante una llamada, debe considerarse un defecto de despliegue o política y no resolverse deshabilitando seguridad.

## 5. Windows Defender Firewall y UDP

### 5.1 Implementación actual

El instalador crea esta regla:

| Campo | Valor |
|---|---|
| Nombre | `CgPhone SIP-RTP` |
| Dirección | Entrante |
| Acción | Permitir |
| Protocolo | UDP |
| Programa | `C:\Program Files\CgPhone\bin\CgPhone.exe` |
| Perfiles | Dominio y Privado |
| Perfil Público | No habilitado |

La desinstalación elimina la regla para evitar excepciones huérfanas. La regla está vinculada al ejecutable, por lo que no abre UDP globalmente para cualquier programa.

### 5.2 Riesgo residual declarado

La regla actual permite UDP entrante al ejecutable en los perfiles indicados, sin restringir todavía:

- Direcciones remotas a las subredes de PBX/SBC.
- Puerto SIP local.
- Rango RTP local.

Esto facilita una beta portable con puertos dinámicos, pero **no es el estado recomendado para producción**.

### 5.3 Hardening obligatorio antes de producción

- Fijar y documentar el puerto de transporte SIP.
- Configurar y documentar un rango RTP acotado.
- Restringir `RemoteAddress` a PBX, SBC y subredes de voz autorizadas.
- Mantener bloqueado el perfil Público.
- Distribuir la regla mediante GPO/Intune cuando corresponda.
- Validar si la política de dominio permite combinar reglas locales. Si la combinación local está deshabilitada, la regla del instalador puede ser ignorada y debe existir una regla corporativa equivalente.
- Registrar eventos permitidos/bloqueados durante la prueba piloto.
- No hardcodear direcciones internas de una empresa en el paquete genérico; deben ser parámetros de despliegue.

### 5.4 Evidencia PowerShell

```powershell
Get-NetFirewallRule -DisplayName "CgPhone SIP-RTP" |
    Format-List DisplayName, Enabled, Direction, Action, Profile, PolicyStoreSource

Get-NetFirewallRule -DisplayName "CgPhone SIP-RTP" |
    Get-NetFirewallApplicationFilter

Get-NetFirewallRule -DisplayName "CgPhone SIP-RTP" |
    Get-NetFirewallPortFilter

gpresult /h "$env:TEMP\CgPhone-GPResult.html"
```

## 6. Configuración y credenciales SIP

La configuración pretende ser **por terminal**, no por usuario de dominio, para conservar el mismo interno SIP cuando cambia el operador del box.

- Los cambios de cuenta requieren sesión administrativa.
- La contraseña no debe almacenarse en texto plano.
- En Windows se usa DPAPI con alcance de máquina para permitir el uso por distintos usuarios del mismo terminal.
- El archivo de configuración debe tener ACL verificadas: escritura administrativa y sólo el acceso mínimo requerido por el proceso estándar.
- DPAPI de máquina protege contra copia offline a otro equipo, pero no protege frente a administrador local, malware con privilegios o un endpoint comprometido.
- Los logs nunca deben imprimir contraseña, Authorization, tokens ni secretos SIP completos.

**Pendiente de aceptación:** comprobar en una máquina unida al dominio la ruta real del archivo, propietario, herencia y ACL efectivas con usuarios estándar diferentes.

## 7. Grabaciones y privacidad

- La ruta predeterminada debe ser `Documentos\CgPhone\Grabaciones` del usuario.
- WAV es el formato de respaldo obligatorio por ser nativo y reproducible sin codec externo.
- MP3 es opcional y sólo se habilita si se encuentra un LAME nativo x64 compatible y verificado.
- No se busca una DLL arbitraria en directorios del sistema ni se descarga en ejecución.
- Si LAME falta o falla, la llamada no debe perderse: se conserva WAV y se informa el fallback.
- Deben definirse retención, cifrado en reposo, acceso, borrado, consentimiento y auditoría según la política del call center y la legislación aplicable.
- La grabación local debe poder deshabilitarse cuando la central ya graba.
- El botón Grabar debe ocultarse o quedar deshabilitado cuando la grabación local no está permitida.

## 8. Cadena de suministro y dependencias

Dependencias relevantes: Qt, PJPROJECT/PJSIP, compilador/toolchain y LAME opcional.

### Controles obligatorios

- Fijar PJPROJECT a un **commit SHA exacto**, no sólo a una rama o versión nominal.
- Fijar acciones de GitHub Actions a SHA completos.
- Registrar versiones exactas de paquetes MSYS2/Qt y sus hashes.
- Generar SBOM CycloneDX o SPDX por release.
- Conservar avisos de licencia y procedencia de cada dependencia.
- Ejecutar análisis de CVE contra el SBOM y documentar triage, severidad, aplicabilidad y remediación.
- Activar revisión de dependencias, CodeQL/Cppcheck y escaneo de secretos.
- Generar manifiesto SHA-256 del artefacto completo.
- Mantener trazabilidad: release → workflow → commit → dependencias → hash → firma.
- No incorporar DLL descargadas manualmente sin fuente, licencia, versión, arquitectura y hash verificables.

Una ausencia de alertas no equivale a ausencia de vulnerabilidades. El control válido es un proceso continuo de inventario, monitoreo, triage y actualización.

## 9. Firma, SmartScreen e integridad

La beta actual puede activar Microsoft Defender SmartScreen porque es nueva y todavía no posee reputación/firma corporativa. Esto no demuestra malware, pero tampoco debe ignorarse.

### Antes de producción

- Firmar EXE, DLL propias e instalador con Authenticode mediante certificado de firma de código aprobado.
- Aplicar sello de tiempo confiable.
- Verificar la firma después de empaquetar; cualquier cambio posterior la invalida.
- Publicar SHA-256 por release.
- Someter el artefacto final a Microsoft Defender y al proceso EDR corporativo.
- Mantener protegida la clave de firma, idealmente en HSM o servicio de firma, con acceso mínimo y auditoría.

```powershell
Get-AuthenticodeSignature "C:\Program Files\CgPhone\bin\CgPhone.exe" |
    Format-List Status, StatusMessage, SignerCertificate, TimeStamperCertificate

Get-FileHash "C:\Program Files\CgPhone\bin\CgPhone.exe" -Algorithm SHA256
```

El hash prueba integridad respecto de un valor publicado; la firma agrega identidad del editor. Ninguno reemplaza análisis de seguridad.

## 10. Diagnóstico, telemetría y datos

- La consola de diagnóstico permanece oculta en operación normal.
- Shift+F12 abre/cierra una ventana de diagnóstico interna; cerrarla no debe terminar CgPhone.
- El log debe aplicar redacción de secretos y minimizar datos personales.
- Definir nivel, rotación, tamaño máximo, ruta y periodo de retención.
- No hay telemetría externa automática declarada en esta beta.
- Para soporte, exportar un paquete explícito y revisable por el usuario/administrador.

## 10.1 Política de ausencia de telemetría

CgPhone no debe enviar información operativa al autor ni a infraestructura controlada por el autor.

### Prohibido por diseño

- Analítica de uso o seguimiento de botones.
- Identificadores de instalación, equipo, dominio, usuario o cuenta SIP.
- Inventario remoto de versiones instaladas.
- Envío automático de logs, diagnósticos, crash dumps, contactos, historial o grabaciones.
- Reportes automáticos de errores a servicios externos.
- Consultas periódicas a servidores personales del autor.
- Publicidad, trackers o SDK de métricas.

Los logs de SIP y diagnóstico permanecen locales. Si Soporte necesita analizarlos, el usuario o administrador debe exportarlos y entregarlos mediante un canal corporativo autorizado, luego de revisar y sanear datos sensibles.

### Actualizaciones

La vigilancia de nuevas versiones y vulnerabilidades de Qt WebEngine se ejecuta del lado del proyecto/autor y no desde las terminales donde se instala CgPhone. Los endpoints no reportan su versión ni estado.

Por defecto, CgPhone no realiza comprobaciones de actualización. En entornos corporativos, las nuevas versiones se distribuyen mediante el mecanismo aprobado por IT, como GPO, Intune, SCCM o instalación manual. Si en el futuro se incorpora una consulta opcional a un manifiesto firmado, deberá estar deshabilitada por defecto, documentar exactamente qué solicitud realiza y no incluir identificadores ni datos SIP.

### Servicios web Pro

Cuando el usuario abre Google Meet o Microsoft Teams mediante Qt WebEngine, existe tráfico directo entre la terminal y Google/Microsoft. Cookies, autenticación, WebRTC y notificaciones pertenecen a esos servicios y se rigen por sus políticas. Esto no constituye telemetría enviada al autor de CgPhone, pero sí debe figurar en la documentación de privacidad y en la matriz de destinos de red.

## 11. Matriz de controles

| Riesgo | Control | Evidencia | Estado |
|---|---|---|---|
| UAC durante llamadas | Instalación/configuración separadas de runtime estándar | Prueba con usuario no administrador | Implementado; validar en dominio |
| UDP entrante excesivo | Regla por programa, sólo Dominio/Privado | PowerShell y captura de firewall | Parcial |
| Origen UDP no restringido | ACL de IP PBX/SBC y rango RTP fijo | GPO y prueba de conectividad | Obligatorio antes de producción |
| Robo de credencial SIP | DPAPI máquina + ACL + logs redactados | Inspección de config y ACL | Parcial; auditar |
| Binario alterado/suplantado | Authenticode + timestamp + SHA-256 | Firma válida y manifiesto | Pendiente release final |
| Dependencia vulnerable | SBOM + CVE scan + pinning | SBOM, reporte y triage | Pendiente automatización |
| DLL no confiable | Procedencia, licencia, hash y arquitectura | Manifiesto de dependencias | Parcial |
| Exposición de grabaciones | Política de acceso/retención y fallback controlado | ACL y prueba de borrado | Pendiente política corporativa |
| Secretos en logs | Sanitización y revisión automatizada | Casos de prueba | Debe validarse |
| Regla local anulada por GPO | Despliegue central y RSoP | `gpresult` y PolicyStoreSource | Debe validarse en dominio |
| Micrófono bloqueado | Política corporativa de privacidad del dispositivo | Prueba con usuario estándar | Debe validarse |
| Fraude SIP | Segmentación, SBC, límites y política PBX | Evidencia de infraestructura | Responsabilidad compartida |

## 12. Plan mínimo de validación

### Funcional

- Registro, llamadas entrantes/salientes, rechazo, autoatención, DND, hold/reanudación, transferencia y early media.
- Audio bidireccional con G.711 A-law y μ-law por defecto.
- Historial completo y rellamada.
- Grabación WAV y fallback desde MP3.
- Segundo usuario de dominio usando la misma cuenta por terminal.

### Seguridad

- Ejecutar como usuario estándar y comprobar cero prompts UAC durante operación.
- Probar perfiles Dominio, Privado y Público.
- Confirmar que sólo PBX/SBC autorizados alcanzan SIP/RTP tras hardening.
- Verificar firma y hashes.
- Revisar ACL de instalación, configuración, logs y grabaciones.
- Buscar secretos en artefacto, repositorio y logs.
- Escanear SBOM y documentar CVE.
- Ejecutar Defender/EDR, análisis estático y prueba dinámica en VM limpia.
- Desinstalar y confirmar eliminación de regla, accesos directos y componentes.

### Resiliencia

- Reinicio del equipo, cambio de usuario, caída de PBX, contraseña incorrecta, 401/403/404/486/5xx, pérdida de red y reconexión.
- Múltiples llamadas consecutivas, llamada entrante mientras hay sesión activa y cierre accidental.
- Validar que una llamada fallida no deja el endpoint permanentemente ocupado.

## 13. Criterios de aprobación para producción

No promover a producción hasta contar con:

1. Firma Authenticode válida y timestamp.
2. SBOM, hashes y licencia de todas las dependencias.
3. PJPROJECT y acciones del workflow fijados a SHA.
4. Regla corporativa restringida a IP/rangos necesarios.
5. Prueba en dominio con GPO real y usuario estándar.
6. Evidencia EDR/Defender y análisis estático sin hallazgos críticos abiertos.
7. Threat model y riesgos residuales aceptados por Seguridad/CTO.
8. Política aprobada para grabaciones y datos personales.
9. Plan de actualización, rollback y respuesta a vulnerabilidades.
10. Prueba piloto documentada con Asterisk y, antes de afirmar compatibilidad productiva, con la versión concreta de Neotel utilizada.

## 14. Declaraciones que no deben hacerse todavía

Hasta completar los controles anteriores, no afirmar que:

- La beta está certificada, homologada o libre de vulnerabilidades.
- SmartScreen no aparecerá.
- El ejecutable está firmado.
- La regla actual está limitada a puertos o IP concretos.
- Se realizó un pentest independiente.
- Existe certificación formal de Neotel.
- Toda CVE de Qt/PJSIP es automáticamente no aplicable.

Sí puede afirmarse que el proyecto tiene código auditable, compilación trazable y un plan explícito para cerrar las brechas antes de producción.

## 15. Mensaje técnico sugerido para el CTO

> CgPhone adopta mínimo privilegio en operación: el agente trabaja como usuario estándar y la elevación queda limitada a instalación y configuración por terminal. SIP/RTP requieren UDP; la beta instala una regla entrante vinculada exclusivamente al ejecutable y a perfiles Dominio/Privado. Reconocemos que la regla todavía no restringe IP remota ni rango RTP, por lo que el hardening de producción contempla puertos determinísticos, PBX/SBC autorizados y distribución por GPO. La release candidata se entregará con Authenticode y timestamp, hashes SHA-256, SBOM, dependencias fijadas, análisis de CVE, escaneo EDR y evidencia de pruebas en dominio. Las credenciales se protegen mediante DPAPI de máquina para conservar la identidad SIP por terminal, complementado con ACL administrativas y validación explícita del riesgo local. Los riesgos residuales y controles pendientes se presentan sin ocultarlos para que Seguridad y Arquitectura puedan aprobarlos formalmente.

## 16. Paquete de evidencia para auditoría

Entregar en una carpeta versionada:

- Código fuente y commit exacto.
- Workflow y log de compilación.
- Artefacto original firmado.
- Firma, certificado, timestamp y SHA-256.
- SBOM SPDX/CycloneDX.
- Inventario de licencias y hashes de terceros.
- Reportes CodeQL/Cppcheck, secretos, CVE y Defender/EDR.
- Exportación de reglas de firewall/GPO y `gpresult`.
- Resultados de pruebas funcionales y de seguridad.
- Threat model, excepciones, riesgos aceptados y responsables.
- Procedimiento de instalación, actualización, rollback y desinstalación.
- Matriz de versiones Asterisk/Neotel probadas.
