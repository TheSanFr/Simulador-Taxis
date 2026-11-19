# Revisión final de TaxiGraph

Fecha de la revisión 18 de noviembre de 2026.

Actualización funcional del 19 de noviembre de 2026. Se incorporó el taxímetro por recorrido real y se calibraron los pesos y el techo de congestión. Los detalles vigentes se describen en [Arquitectura](arquitectura.md). El inventario y las pruebas siguientes corresponden a la revisión anterior a ese cambio.

Se leyeron los 28 encabezados, las 28 implementaciones C++, los dos formularios, la hoja de estilos, las rutas de recursos, CMake y la documentación de arquitectura. La revisión incluye constructores, destructores, getters, setters, métodos de negocio, dibujo, señales y auxiliares geométricos. La lista inferior identifica los 348 cuerpos revisados, que incluyen 329 métodos de clase, 18 auxiliares libres y main.

## Correcciones realizadas

- Las lecturas de pasos de ruta verifican el índice y devuelven -1 cuando está fuera del contenido válido.
- El taxi manual informa que rechazó un paso cuando su ruta ya contiene 100 pasos.
- Un taxi que ya está en movimiento conserva su tramo ante una llamada repetida a iniciarTramo.
- La llegada y el cobro se registran una sola vez. Las llamadas repetidas sobre un taxi detenido o sin pasajero carecen de efectos.
- Los taxis rechazan intervalos de tiempo inválidos y calculan el giro corto incluso después de acumular varias vueltas.
- Las variantes BFS, DFS y Dijkstra aceptan un grafo nulo devolviendo una ruta vacía.
- Dijkstra y la selección de pasajeros admiten costos superiores a 999999. La distancia desconocida se expresa mediante -1 en Dijkstra.
- Las operaciones de pesos, multiplicadores y coordenadas rechazan valores indefinidos, infinitos o fuera del rango de conversión.
- La consulta del nodo más cercano cubre las coordenadas aceptadas por el modelo.
- El formato monetario evita conversiones fuera del rango entero.
- Una carrera sin ruta devuelve una cotización cero, se muestra como «Sin ruta» y su pasajero permanece en espera.
- Simulacion valida los índices de taxis, evita iniciar dos veces la sesión y acepta las velocidades de la interfaz, 1×, 2× y 4×.
- El reloj rechaza tiempos negativos o indefinidos y modos fuera de 0 a 3.
- Cambiar el peso de una calle de un solo sentido funciona al seleccionar sus extremos en ambos órdenes. La dirección de la calle se conserva.
- Los componentes visuales desconectan la simulación anterior antes de conectarse a otra y resuelven la desaparición de la simulación.
- Reiniciar elimina los avisos y efectos visuales del viaje anterior.
- La generación de pasajeros utiliza bucles con índices y una lista de pesos de destino. Los destinos turísticos conservan su mayor probabilidad.
- La comprobación de capacidad, la búsqueda de divisiones y la ordenación de los cruces utilizan bucles explícitos. La colocación de hitos devuelve un booleano y una posición mediante referencia.
- Las identidades y flechas almacenadas utilizan arreglos simples. Las constantes utilizan const y las variables de los fragmentos revisados tienen nombres descriptivos.

## Verificación

La compilación Release desde cero utiliza Qt 6.11.0 MinGW de 64 bits, C++17 y las advertencias -Wall, -Wextra y -Wpedantic.

| Prueba | Cobertura |
| --- | --- |
| Estructuras, modelo y taxis | 139 comprobaciones y cuatro verificaciones estáticas de copia deshabilitada. Límites, listas vacías, altas/bajas, rutas llenas, repetición de llegada/cobro, movimiento y orientación. |
| Grafo | 20 comprobaciones de costos grandes, datos numéricos inválidos, rutas de 100 nodos, dirección, identidad de nodos, cruces, confirmación y atomicidad de edición, división de pesos y tarifas. |
| Reloj y simulación | 16 comprobaciones de agenda, cambio de día, validación, inicio repetido, cotización sin ruta, elección de pasajero con costo alto, generación alcanzable y reinicio. |
| Interfaz específica | 9 comprobaciones de cambio de peso en calle de un sentido, conexión repetida/reemplazada, destrucción de simulación y eliminación de efectos al reiniciar. |
| Interfaz integrada | 16 comprobaciones de Espera, clic y Enter, eventos históricos, minimapa, tarifas, ocho cambios rápidos de pestaña, recogida y entrega manual a 1× con cobro único, estado vacío y reinicio. |
| Recursos | Comprobación de las rutas incorporadas de estilos y flecha de los selectores. |
| Memoria | Prueba corta con 300 registros, cambios de filtro y reinicio. |

Las pruebas se ejecutan fuera de las carpetas de producción. La memoria privada fue de 51,79 MiB al inicio y con 300 registros y de 52,09 MiB después de reiniciar. La comprobación corta de memoria se limita a las operaciones indicadas.

## Alcance y decisiones conservadas

La ciudad y las rutas admiten 100 nodos. Las partidas pertenecen a la sesión y el historial crece hasta reiniciar. La tarifa se fija al recoger al pasajero con la ruta Dijkstra vigente. La tabla Nodos acumula las búsquedas del taxi, incluidas las consultas para elegir pasajero. Estas decisiones están documentadas en arquitectura.md.

En ediciones extremadamente densas, los nombres prueban cuatro ubicaciones y pueden superponerse cuando ninguna queda libre. La cobertura de pruebas comprende los escenarios descritos y la lectura estática de todos los archivos. Una sesión de varias horas y todas las configuraciones de pantalla quedan fuera de esa evidencia.

## Conceptos de apoyo para la sustentación

| Elemento | Significado en este proyecto |
| --- | --- |
| const | Declara un dato cuyo valor queda fijo. |
| append | Añade un elemento al final de una colección de Qt. |
| QPointF | Guarda una posición mediante coordenadas x e y. |
| QVector y QHash | Colecciones de Qt para secuencias y búsquedas por identificador. |
| nullptr | Representa un puntero que no señala un objeto. |
| override | Indica que una clase derivada implementa un método virtual heredado. |
| connect | Conecta una señal de Qt con la operación que debe ejecutarse. |
| auto | Permite al compilador deducir el tipo de una variable a partir de su valor inicial. |
| Expresión lambda [] | Define una función breve en el lugar de uso. Se utiliza especialmente para reaccionar a señales de Qt. |
| Parámetro con & | Permite consultar un objeto sin copiarlo o devolver un resultado modificando un parámetro. |
| std::numeric_limits | Permite consultar los límites de un tipo numérico antes de convertir un valor. |
| QPointer | Puntero de Qt que detecta la destrucción del objeto observado. |
| std::isfinite | Comprueba que un número tiene un valor finito utilizable. |
| = delete | Impide una operación, aquí la copia de contenedores propietarios de memoria. |

## Inventario de funciones revisadas

Los encabezados de cada clase se revisaron junto a su implementación. Esta lista incluye los métodos con cuerpo, los auxiliares libres y el punto de entrada.

### src/estructuras/Cola.cpp

Cola, ~Cola, encolar, desencolar, estaVacia, limpiar.

### src/estructuras/ElementoArista.cpp

ElementoArista, getArista, getSiguiente, setSiguiente.

### src/estructuras/ElementoNodo.cpp

ElementoNodo, getNodo, getSiguiente, setSiguiente.

### src/estructuras/ListaAristas.cpp

ListaAristas, ~ListaAristas, agregar, obtenerArista, existeArista, eliminarArista, eliminarAristasDeNodo, getPrimero, getCantidad, limpiar.

### src/estructuras/ListaNodos.cpp

ListaNodos, ~ListaNodos, agregar, buscarPorId, eliminarPorId, getPrimero, getCantidad, limpiar.

### src/estructuras/NodoCola.cpp

NodoCola, getDato, getSiguiente, setSiguiente.

### src/estructuras/NodoPila.cpp

NodoPila, getDato, getSiguiente, setSiguiente.

### src/estructuras/Pila.cpp

Pila, ~Pila, apilar, desapilar, estaVacia, limpiar.

### src/interfaz/BarrasGananciasWidget.cpp

reiniciarBarras, enCuadro, showEvent, paintEvent, BarrasGananciasWidget, setSimulacion.

### src/interfaz/DialogoNodo.cpp

DialogoNodo, ~DialogoNodo, getNombreNodo, getTipoNodo.

### src/interfaz/GraficoGananciasWidget.cpp

reconstruirSeries, enMuestra, GraficoGananciasWidget, setSimulacion.

### src/interfaz/MainWindow.cpp

asignarCelda, taxiObservado, observandoTaxiAjeno, taxiConAviso, actualizarTextoAyudaEdicion, mostrarMensajeEstado, configurarPanelGuia, enPausaCambiada, actualizarBotonPausa, enMostrarPesosCambiado, enVelocidadCambiada, enModoTiempoCambiado, enModoEdicionCambiado, limpiarSeleccionEdicion, enReiniciarClicado, enCancelarClicado, enEspacioVacioClicado, tramoEnUso, avisoCalleOcupada, motivoNodoEnUso, conservarViajesAlEliminar, confirmarCalle, enNodoClicado, registrarCambioMapa, mostrarEvento, insertarEvento, actualizarEventos, agregarEvento, configurarTablas, actualizarResumenGrafo, actualizarEstadoSimulacion, actualizarTablas, actualizarEspera, ubicarPasajero, limpiarUbicacionPasajero, MainWindow, ~MainWindow.

### src/interfaz/MapaWidget.cpp

fuentePixel, obtenerColorTipoNodo, mezclarColores, dibujarTextoConBorde, transformacionVista, actualizarGeometria, invalidarGeometria, posicionDibujoTaxi, dibujarMundo, dibujarFondoCiudad, actualizarCorredorCalles, dibujarEscenario, dibujarVistaPreviaEdicion, dibujarConfirmacion, dibujarRecorridoConsultado, dibujarCalles, dibujarRutas, dibujarPesos, dibujarNodos, dibujarNombresNodos, dibujarPasajeros, dibujarTaxis, dibujarEfectosNocturnos, areaMinimapa, dibujarMinimapa, enCuadro, enCarreraCompletada, paintEvent, mousePressEvent, mouseMoveEvent, nodoInteractivo, leaveEvent, seleccionarNodoEnFoco, MapaWidget, setSimulacion, setNodoPendiente, setHerramientaEdicion, setSentidoCalle, setDestinoProvisional, resaltarNodo, resaltarRecorridoPasajero, ocultarRecorridoPasajero, resaltarCalle, setModoConducir, setMostrarPesos, mostrarRutaDelTaxi, setTaxiEnFoco.

Auxiliares revisados. anchoConAcera, corredorDeTramo y calcularFlecha.

### src/interfaz/TextoFlotante.cpp

TextoFlotante, getPosicion, getTexto, getColor, getEdad, avanzarEdad.

### src/main.cpp

main.

### src/modelo/Arista.cpp

Arista, getOrigen, getDestino, getPeso, getPesoInicial, getEsAvenida, setOrigen, setDestino, setPeso, setPesoInicial, setEsAvenida.

### src/modelo/ManzanaCiudad.cpp

ManzanaCiudad, getPoligono, getTipo.

### src/modelo/Nodo.cpp

Nodo, getId, getNombre, getX, getY, getPosicion, getTipo, setId, setNombre, setX, setY, setTipo.

### src/modelo/Pasajero.cpp

Pasajero, getId, getOrigen, getDestino.

### src/modelo/ResultadoRuta.cpp

ResultadoRuta, getEncontrada, getCantidadNodos, getNodoEn, getTiempoPonderado, getNodosExplorados, getSaltos, setEncontrada, setTiempoPonderado, setNodosExplorados, limpiarCamino, agregarNodoAlCamino.

### src/simulacion/Grafo.cpp

reconstruirRuta, Grafo, ~Grafo, getNodos, getAristas, buscarNodo, tieneNodo, getIdentidadNodo, getRevisionPesos, obtenerSiguienteIdDisponible, agregarNodo, agregarNodoNuevo, eliminarNodo, sugerirPeso, agregarArista, agregarCalleDoble, previsualizarCalle, crearCalleEditada, previsualizarNodo, crearNodoEditado, eliminarCalle, cambiarPeso, limpiar, obtenerArista, existeAristaDirigida, obtenerPesoEfectivo, obtenerPesoEfectivoArista, obtenerNivelCongestion, registrarUso, nodoMasCercano, setMultiplicadorHoraPico, getMultiplicadorHoraPico, buscarRutaBFS, buscarRutaDFS, buscarRutaDijkstra, calcularTarifa, formatearPesos.

Auxiliares geométricos revisados. proyectar, coordenadaNodo, esInterior, callesExistentes, solapaTramo, registrarCruce, comprobarCapacidad, prepararCalle, prepararNodo, tramosAfectados, reservarIds, planearDivision, comprobarAristas, aplicarAristas.

### src/simulacion/RelojSimulacion.cpp

RelojSimulacion, reiniciar, avanzar, getDia, getModo, setModo, textoHora, minutoActual, periodoActual, textoProximoPeriodo, oscuridad.

Auxiliar revisado. formatearHora.

### src/simulacion/Simulacion.cpp

agregarManzana, construirCiudadInicial, liberarTaxis, crearTaxis, generarPasajero, buscarIndicePasajeroPorId, elegirPasajeroObjetivo, revisarParadaTaxi, planificarRutaAutonoma, actualizarTaxi, avanzarUnPaso, actualizarPeriodo, nombreNodo, registrarEvento, enTickReloj, Simulacion, ~Simulacion, getGrafo, getReloj, getCantidadTaxis, getTaxi, getTaxiManual, getPasajerosEnEspera, getLimitesMapa, getPoligonoAgua, getManzanas, iniciar, estaPausada, setPausada, setVelocidad, setModoTiempo, reiniciar, cotizarCarrera.

### src/taxis/Taxi.cpp

Taxi, ~Taxi, esManual, calcularRuta, getId, getNombre, getColor, getNodoActual, getPosicion, getAngulo, getEnMovimiento, getNodoSiguiente, getCantidadRuta, getPasoRuta, getLlevaPasajero, getDestinoPasajeroABordo, getTarifaPasajeroABordo, getIdPasajeroObjetivo, getViajes, getIngresos, getNodosExplorados, getEsperaPensar, setNodoActual, setPosicion, setIdPasajeroObjetivo, setEsperaPensar, sumarExplorados, subirPasajero, bajarPasajero, limpiarRuta, agregarPasoARuta, cargarRutaDesdeResultado, quitarPrimerPasoRuta, nodoFinalDeRuta, iniciarTramo, avanzarEnTramo, suavizarAngulo, textoEstado.

### src/taxis/TaxiBFS.cpp

TaxiBFS, nombreAlgoritmo, calcularRuta.

### src/taxis/TaxiDFS.cpp

TaxiDFS, nombreAlgoritmo, calcularRuta.

### src/taxis/TaxiDijkstra.cpp

TaxiDijkstra, nombreAlgoritmo, calcularRuta.

### src/taxis/TaxiManual.cpp

TaxiManual, nombreAlgoritmo, esManual, agregarNodoARuta.

### src/interfaz/TransicionesPanel.h

Constructor, paintEvent, eventFilter, animacionesPermitidas y finalizar.
