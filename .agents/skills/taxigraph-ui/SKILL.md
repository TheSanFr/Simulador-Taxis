---
name: taxigraph-ui
description: Coordina el diseño, implementación y revisión de TaxiGraph en Qt Widgets. Aplica a cambios visuales o de interacción del mapa, conducción, editor, panel de flota, ingresos y controles.
---

# Dirección de interfaz de TaxiGraph

Lee [Dirección de diseño](../../../docs/diseno-interfaz.md) y las instrucciones existentes del repositorio. Ubica la tarea en la pantalla actual y confirma los puntos de entrada antes de proponer o editar. Trabaja por un recorrido comprobable y reconoce los cambios locales del usuario.

## Equipo

- [Diseño de interacción](../../agents/taxigraph-product-ux.md) define jerarquía, controles y estados.
- [Cartografía](../../agents/taxigraph-map-design.md) define la lectura de calles, rutas y entidades.
- [Implementación Qt](../../agents/taxigraph-qt-ui.md) traduce la especificación a un cambio acotado.
- [Verificación](../../agents/taxigraph-ui-qa.md) revisa evidencia visual y regresiones.

Cuando el entorno permita invocar estos subagentes, delega a producto y cartografía en paralelo si ambas preguntas son independientes. Entrega a cada uno el objetivo por separado, los archivos, el estado de referencia y el resultado esperado. Si la sesión solo ofrece skills, aplica directamente las definiciones pertinentes como instrucciones de trabajo.

Unifica la especificación antes de editar. Encarga la implementación a un único responsable para evitar ediciones simultáneas del layout y el mapa. Pasa a revisión el diff, la escena de referencia y la configuración de compilación. Para cambios pequeños, usa solo las responsabilidades que aporten una decisión o una comprobación.

Entrega el comportamiento observado, archivos tocados, tamaño de ventana y escenarios probados. Ante diferencias entre la captura y el código, toma una referencia nueva y conserva los hallazgos de la captura como hipótesis.
