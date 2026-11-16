# TaxiGraph · Dirección de diseño

## Diagnóstico de la captura

La ciudad ya le da identidad a TaxiGraph. Tiene manzanas, puntos reconocibles y taxis que se mueven sobre un grafo editable. El efecto de plantilla aparece en la jerarquía que rodea ese mapa. La cabecera, el título del grafo, la barra de modo y la leyenda son cuatro franjas de peso similar. En la derecha, tres bandas explicativas desplazan la tabla y repiten información disponible en la Guía. Dentro del mapa, el rosa de las calles y sus flechas repetidas tiene más peso que el taxi manual y el destino.

La interfaz debe responder en unos segundos a tres preguntas de una demostración: qué está ocurriendo en la ciudad, qué puede hacer el usuario ahora y cómo se comporta la flota. TaxiGraph tiene dos tareas distintas sobre ese mismo mapa, conducir y editar. El control de modo necesita mostrar cuál está activa y el texto contextual debe indicar el siguiente clic, en especial cuando ya hay un origen elegido para crear o editar una calle.

## Composición propuesta

| Zona | Propuesta y criterio |
| --- | --- |
| Cabecera | Agrupa Pausar/Reanudar y velocidad. Junta el reloj, el período activo y el selector de período sin confundir el modo automático con el estado actual. Baja el peso visual de Reiniciar y conserva su comportamiento. |
| Franja del mapa | Reúne el modo, la instrucción contextual y Mostrar pesos junto al mapa. Mantén visibles los conteos de nodos y aristas con peso de metadato. Deja la leyenda de sentidos accesible y evita que un cambio de ayuda desplace el lienzo. |
| Ciudad | Construye una jerarquía entre manzanas y agua, red vial, congestión, rutas y entidades. Usa amarillo para el taxi manual, su ruta y destino con marcas diferenciables. Mantén neutral la vía base y conserva una escala de congestión reconocible. Evita que el aspecto nocturno borre flechas, pesos o destinos. |
| Panel derecho | Pon primero el estado del taxi manual y la información que cambia la siguiente decisión, por ejemplo, destino durante la carrera. Conserva la tabla como herramienta de comparación. Lleva la explicación detallada de BFS, DFS y Dijkstra a Guía y deja una referencia breve cerca de la tabla si hace falta. |
| Tabla | Alinea importes y conteos a la derecha y conserva los nombres de taxi. Comprueba en código qué mide «Nodos» antes de relabelar. Si cuenta nodos explorados o acumula visitas, aclara el alcance y la unidad. Evita que el color por sí solo codifique el método. |

### Mapa y comparabilidad

- En conducción, orden de atención: taxi manual y siguiente decisión, destino, taxis/pasajeros relevantes, red de fondo. En edición, el origen seleccionado, el siguiente nodo y el sentido válido pasan al primer plano.
- Mantén las flechas informativas y su dirección correcta. Reduce su repetición visual cuando el espacio lo permita. Verifica calles de ida, vuelta y doble sentido con el zoom o las escalas disponibles.
- Segmentos que comparten rutas necesitan ser distinguibles. Si existe selección de taxi o resaltado de ruta, úsalo para reducir información simultánea. Valida antes si añadir selección forma parte del cambio.
- Trata el amarillo como acento del conductor, no como borde genérico. Combina letras y colores para identificar BFS, DFS y Dijkstra. Dado que los ingresos totales dependen de viajes y condiciones, evita presentarlos como prueba de superioridad de un algoritmo.
- Usa una escala tipográfica pequeña y consistente, y reserva cifras monoespaciadas o tabulares para reloj y métricas. Aprovecha los controles nativos de Qt y personaliza estados, densidad y jerarquía con QSS. Comprueba menús, selección, foco y contraste.

## Secuencia de implementación

1. **Mapa y conducción.** Captura el estado de partida en pausa. Reduce franjas y competencia visual, aclara el siguiente clic y ajusta la prioridad de la ruta manual, conservando los controles y sentidos. Compara escenas equivalentes de día y noche.
2. **Panel de flota.** Reorganiza el resumen del taxi, la tabla y las explicaciones. Prueba viajes, montos, nombres largos y pestañas en una ventana estrecha y otra amplia.
3. **Edición y repaso.** Comprueba selección de origen y destino, cancelación, pesos y diálogo. Ajusta visualización de ingresos y eventos después de estabilizar el recorrido principal.

## Puntos de entrada confirmados

- `mainwindow.ui` define los layouts, controles y splitter. `MainWindow.cpp` conecta los eventos y presenta estado, tablas y ayuda.
- `interfaz.qss` es la hoja de estilos cargada por `main.cpp` desde los recursos registrados en CMake.
- `MapaWidget.cpp` dibuja calles, rutas, pesos, nodos, pasajeros, taxis y efectos nocturnos. También gestiona coordenadas y eventos del ratón.
- `Simulacion.cpp`, `Grafo.cpp`, `RelojSimulacion.cpp` y las clases de taxi definen reglas de negocio. Comprueba sus contratos si cambia su representación.

## Aceptación de la primera entrega

- En capturas comparables, se puede señalar el taxi manual, el destino y un nodo válido sin leer el panel de flota. Una calle de sentido único conserva su dirección reconocible y Mostrar pesos sigue ofreciendo la información correcta.
- El modo de conducción y un modo de edición indican qué acción toca. Un clic válido y la cancelación muestran estados coherentes.
- Pausa, reanudación, 1×/4× y cambio de período conservan su comportamiento. A 4× no aparece un parpadeo nuevo en estados comparables.
- Aproximadamente a 1280×720 y 1920×1080, el layout sigue dando acceso a los controles. Revisa además escalado de Windows al 125 % o 150 % cuando esté disponible. Si una captura exacta es imposible, registra dimensiones y escala reales.
- Adjunta evidencia del build y del ejecutable probado. Documenta hallazgos con pasos, esperado y observado. La captura de referencia muestra una vista nocturna y no permite inferir resultados de ejecución.
