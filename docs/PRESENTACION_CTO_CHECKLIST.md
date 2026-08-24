# Checklist para presentación de CgPhone al CTO

## Punto obligatorio: permisos UDP de SIP/RTP

Al preparar la defensa técnica, la presentación o el correo al CTO, incluir:

- CgPhone **no evade ni deshabilita** la seguridad de Windows.
- SIP y el audio RTP requieren tráfico UDP entrante.
- El instalador, ejecutado una única vez con privilegios administrativos, crea la regla de Firewall por programa:
  - Nombre: `CgPhone SIP-RTP`
  - Ejecutable: `C:\Program Files\CgPhone\bin\CgPhone.exe`
  - Dirección: entrante
  - Protocolo: UDP
  - Perfiles: Dominio y Privado
- No se habilita el perfil Público por defecto para limitar la superficie de ataque.
- La regla por ejecutable permite los puertos RTP dinámicos sin abrir rangos UDP globales para otras aplicaciones.
- Los agentes operan posteriormente como usuarios estándar y no necesitan aprobación administrativa para llamar o recibir llamadas.
- En un dominio de Active Directory se recomienda distribuir el instalador y/o la regla mediante GPO.
- La desinstalación elimina la regla para no dejar excepciones huérfanas.

## Mensaje ejecutivo sugerido

> La elevación se utiliza únicamente durante la instalación y la configuración administrativa. La operación diaria del agente funciona con privilegios estándar. El instalador registra una excepción acotada de Windows Defender Firewall para el ejecutable firmado de CgPhone, limitada a UDP entrante en perfiles Dominio y Privado, necesaria para señalización SIP y audio RTP.

## Evidencia para compliance

Adjuntar o demostrar:

1. Regla creada en Windows Defender Firewall.
2. Ruta fija bajo Program Files.
3. Hash SHA-256 y firma digital del ejecutable final.
4. SBOM y versiones de Qt, PJSIP y LAME.
5. Prueba de llamadas entrantes y salientes con usuario estándar.
6. Eliminación de la regla durante la desinstalación.
