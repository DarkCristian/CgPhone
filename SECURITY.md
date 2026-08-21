# Política de seguridad de CgPhone

## Estado soportado

CgPhone se encuentra en fase alpha/beta de laboratorio. Los artefactos actuales
no están aprobados para despliegue productivo ni para exposición directa a
Internet. La compatibilidad funcional con una central SIP no constituye una
certificación de seguridad.

## Reporte de vulnerabilidades

No publiques credenciales SIP, direcciones internas, capturas de tráfico ni
detalles explotables en un issue público. Reportá el hallazgo mediante un aviso
privado de seguridad de GitHub o al responsable interno del proyecto e incluí:

- versión y SHA-256 del artefacto;
- commit de Git utilizado;
- central y transporte SIP involucrados, sin contraseñas;
- pasos mínimos para reproducirlo;
- impacto observado y logs sanitizados.

## Criterio de release

Una versión final requiere completar el gate definido en
[`docs/SECURITY_COMPLIANCE.md`](docs/SECURITY_COMPLIANCE.md). En particular, no
se publicará una versión productiva basada en PJPROJECT 2.17 sin aplicar y
documentar los parches de seguridad correspondientes.

