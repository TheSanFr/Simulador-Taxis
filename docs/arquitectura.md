  # Arquitectura de OnDrive
  
  Esta guía describe la organización del código que acompaña al proyecto. El [README](../README.md) contiene el mapa de las 28 clases y las instrucciones de compilación.
  
  ## Flujo de ejecución
  
  ```text
  src/main.cpp
      └── MainWindow
          ├── Simulacion
          │   ├── Grafo y estructuras enlazadas
          │   ├── RelojSimulacion
          │   ├── 6 taxis autónomos y 1 taxi manual
          │   └── Pasajeros en espera
          ├── MapaWidget
          ├── BarrasGananciasWidget
          ├── GraficoGananciasWidget
          └── TransicionesPanel
  ```
  
  `MainWindow` crea la simulación, construye los componentes visuales y conecta las señales de Qt. `Simulacion` mantiene el estado de la partida. Las clases de taxi solicitan rutas a `Grafo`, y `MapaWidget` representa ese estado. La ventana traduce los clics y cambios de controles en operaciones de conducción, pausa o edición.
  
  ## Responsabilidades y dependencias
  
  | Área | Archivos principales | Relación con el resto |
  | --- | --- | --- |
  | Datos | `src/modelo/` | Describe lugares, calles, pasajeros, resultados de búsqueda y manzanas. |
  | Estructuras | `src/estructuras/` | Proporciona las listas del grafo y los contenedores de BFS y DFS. |
  | Red y partida | `src/simulacion/` | Usa datos, estructuras y taxis para mantener las reglas y la evolución de la partida. |
  | Vehículos | `src/taxis/` | Comparte movimiento en `Taxi` y especializa la planificación y el control manual. |
  | Presentación | `src/interfaz/` | Consulta la simulación, recibe sus señales y presenta el estado al usuario. |
  
  Las clases `TaxiBFS`, `TaxiDFS` y `TaxiDijkstra` comparten el movimiento y el estado de `Taxi`. Cada variante implementa `calcularRuta` y delega la búsqueda en `Grafo`. El contador de nodos explorados acumula el trabajo de las búsquedas realizadas por cada taxi, incluidas las consultas para elegir qué pasajero buscar.
  
  ## Grafo y edición
  
  Los nodos tienen un identificador entre 0 y 99. Una calle de doble sentido se representa mediante dos aristas dirigidas. Las búsquedas y los caminos de los taxis admiten hasta 100 nodos. El editor permite crear nodos, conectar calles, dividir tramos al crear cruces, eliminar elementos y cambiar pesos.
  
  `MainWindow` comprueba que una modificación del editor conserve los viajes en curso y evita alterar tramos ocupados. `Grafo` construye y valida el plan de edición antes de aplicarlo. La señal `grafoModificado` permite actualizar los conteos y reconstruir la geometría visual que depende de la red.
  
  Cada alta de un nodo recibe una identidad adicional que cambia aunque se reutilice su identificador numérico. Los vínculos de Eventos comprueban esa identidad antes de resaltar un cruce. Así, un evento conserva su texto histórico y su referencia deja de activarse si el lugar original fue eliminado.
  
  ## Pasajeros y carreras
  
  La simulación comienza con seis pasajeros e intenta generar otros mientras haya menos de catorce en espera. La recogida sucede cuando un taxi disponible llega al nodo del pasajero. Los autónomos pueden elegir un pasajero como objetivo de navegación. Ese objetivo permite que varios taxis compitan por la misma recogida.
  
  La tabla Espera consulta las estimaciones y permite resaltar temporalmente el origen y el destino. Si el grafo carece de una ruta entre ambos puntos, presenta «Sin ruta» y la recogida se rechaza hasta que el trayecto sea posible. El taxi manual se conduce eligiendo cruces válidos en el mapa. El pasajero sale de la lista al subir a un taxi y el ingreso se acredita una sola vez al llegar al destino.
  
  La lista de Espera presenta una estimación obtenida con Dijkstra. Al recoger al pasajero se inicia el taxímetro con la base de $4.600 y se fijan los recargos del destino y del período de recogida. El importe final se calcula según los saltos realmente completados y el tiempo ponderado realmente recorrido. Cada tramo conserva el peso efectivo que tenía al entrar, y durante el movimiento se acumula la fracción recorrida de ese peso. Al entregar se aplica el mínimo y se acredita el cobro una sola vez.
  
  ```text
  Taxímetro = máximo(8.100, 4.600 + saltos × 1.625 + tiempo ponderado × 1.625)
  Tarifa = taxímetro + recargo del destino + recargo nocturno
  ```
  
  Los recargos por destino son $8.600 para aeropuerto, $2.400 para terminal y $2.000 para sitio turístico. El período nocturno añade $2.000. La hora pico aplica un multiplicador de 1,8 a los pesos efectivos de las calles. El uso de cada tramo aumenta su peso en 0,05 hasta un techo fijo de 1,5 veces su peso inicial. El incremento acumulado por uso se mantiene durante la sesión. El multiplicador de hora pico depende del período activo.
  
  ## Tiempo y actualización visual
  
  `Simulacion` utiliza un temporizador de 16 ms y mide el tiempo transcurrido con `QElapsedTimer`. Divide los avances en pasos pequeños cuando la velocidad lo requiere. Las velocidades admitidas son 1×, 2× y 4×. `RelojSimulacion` convierte 720 segundos simulados en un día y permite fijar un período o seguir la agenda automática.
  
  `MapaWidget` almacena la base de la ciudad, la geometría de calles y los rótulos reutilizables. Lee los pesos y la congestión vigentes para dibujar el estado actual. Los cambios del grafo invalidan la geometría almacenada. La elección de un taxi filtra las trayectorias y destinos destacados mientras los vehículos continúan visibles.
  
  La tabla Espera reutiliza sus cotizaciones mientras sigan vigentes los pasajeros, la revisión de pesos y el período. Las barras muestran los importes reales mientras interpolan su ancho. La gráfica conserva una ventana de 90 segundos simulados. La transición entre pestañas dura 150 ms, libera su captura al terminar y consulta la preferencia de animaciones de Windows.
  
  ## Propiedad de memoria
  
  `MainWindow` utiliza la relación de padres e hijos de Qt para la simulación y los widgets. `Simulacion` libera los taxis que crea. `Grafo` libera sus listas, y estas destruyen los elementos y los datos que contienen. La cola y la pila eliminan sus enlaces al extraer elementos o vaciarse.
  
  `ListaNodos`, `ListaAristas`, `Cola` y `Pila` deshabilitan la construcción por copia y la asignación por copia. Cada instancia mantiene la propiedad de sus enlaces y la liberación de sus elementos.
  
  Los pasajeros, manzanas y avisos temporales se almacenan como valores. Los avisos caducan y la gráfica elimina muestras antiguas. El historial de eventos permanece durante la sesión y se vacía al reiniciar, por lo que su tamaño depende de la duración de la partida.
  
  ## Taxímetro y calibración de tarifas
  
  El taxímetro de Tu taxi aparece en una tarjeta flotante de la esquina superior izquierda del mapa durante Conducir. Muestra el acumulado antes del mínimo y los recargos por separado. La indicación del mínimo de $8.100 permanece mientras el acumulado sea inferior a ese valor. En pausa el contador se detiene y al editar se oculta para dejar libre el mapa. La vista de conducción reserva espacio superior para que la tarjeta no tape los cruces. Si se crean nodos cerca del borde superior, ese espacio se amplía al reconstruir la geometría.
  
  Los pesos iniciales automáticos se calculan a partir de la longitud visual dividida por 400, con ventaja de avenida de 0,75, redondeo a una décima y mínimo de 0,25. Se conserva la fórmula exigida, sus valores de $4.600 y $1.625, el mínimo y los recargos. La duración visual convierte cada unidad ponderada mediante (400 / 130) × 0,75 segundos. Esa conversión aproxima la velocidad anterior y evita que la reducción de pesos acelere los vehículos en la misma proporción.
  
  La calibración sobre los 28 nodos iniciales y destinos normales de 2 a 4 saltos estima aproximadamente $36.000–$37.000 por tres rutas con tráfico inicial y $40.000–$41.000 al techo de uso. Son recorridos de referencia. Los desvíos, los destinos especiales y la hora pico producen importes diferentes.
  
  ## Formularios y recursos
  
  `ui/mainwindow.ui` define la composición de la ventana y `ui/nodedialog.ui` el formulario del nodo. CMake utiliza AUTOUIC y AUTOMOC para generar las cabeceras de esos formularios y el soporte del sistema de señales de Qt.
  
  `recursos/interfaz.qss` contiene los estilos y `recursos/flecha-selector.png` la flecha de los selectores. El registro de recursos en `CMakeLists.txt` incorpora ambos al ejecutable con rutas estables para que la interfaz pueda cargarlos sin depender del directorio de ejecución.
  
  ## Alcance de la entrega
  
  La aplicación utiliza una partida en memoria con una ciudad inicial y edición interactiva. Los ingresos, pasajeros, cambios del mapa y eventos pertenecen a la sesión actual. Reiniciar reconstruye el estado inicial. El límite de 100 nodos forma parte de las representaciones actuales de búsquedas y rutas y debe revisarse conjuntamente si se amplía el tamaño de la ciudad.
