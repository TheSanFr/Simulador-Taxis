# TaxiGraph

Simulador de taxis de escritorio desarrollado en C++17 con Qt 6 Widgets y Charts. La ciudad es un grafo dirigido y ponderado. Circulan seis taxis autónomos, dos por algoritmo BFS, DFS y Dijkstra, y un taxi controlado por el usuario.

## Estructura del proyecto

```text
TaxiGraph/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp
│   ├── modelo/
│   ├── estructuras/
│   ├── simulacion/
│   ├── taxis/
│   └── interfaz/
├── ui/
├── recursos/
├── docs/
│   ├── arquitectura.md
│   └── diseno-interfaz.md
├── .agents/
└── build/
```

Cada clase conserva su declaración en `.h` y su implementación en `.cpp`. `TransicionesPanel.h` contiene una implementación breve en la propia cabecera. Las carpetas agrupan responsabilidades y los nombres de clase permiten encontrar sus archivos directamente.

| Carpeta | Contenido y responsabilidad |
| --- | --- |
| `src/modelo/` | Datos que representan la ciudad y los viajes. |
| `src/estructuras/` | Listas enlazadas, cola y pila implementadas para las búsquedas del grafo. |
| `src/simulacion/` | Red vial, algoritmos de búsqueda, reloj y coordinación de la partida. |
| `src/taxis/` | Estado y movimiento compartido de los taxis y comportamiento de cada variante. |
| `src/interfaz/` | Ventana, dibujo del mapa, paneles y efectos de presentación. |
| `ui/` | Formularios editables con Qt Designer. |
| `recursos/` | Hoja de estilos y flecha de los selectores, incorporadas al ejecutable por CMake. |
| `docs/` | Arquitectura y criterios de diseño de la interfaz. |
| `.agents/` | Instrucciones de apoyo para herramientas de desarrollo. |
| `build/` | Directorios de compilación generados localmente. |

## Mapa de clases

Los nombres siguientes identifican los archivos `.h` y `.cpp` dentro de cada carpeta, salvo la cabecera de transiciones indicada arriba.

| Carpeta | Clase | Responsabilidad |
| --- | --- | --- |
| `src/modelo/` | `Nodo` | Identificador, nombre, posición y tipo de un lugar de la ciudad. |
| | `Arista` | Conexión dirigida entre nodos, peso actual, peso inicial y condición de avenida. |
| | `Pasajero` | Identificador, nodo de recogida y nodo de destino. |
| | `ResultadoRuta` | Camino encontrado, saltos, tiempo ponderado y nodos explorados. |
| | `ManzanaCiudad` | Polígono y tipo de una manzana para representar el entorno. |
| `src/estructuras/` | `ListaNodos` | Almacena, busca y elimina los nodos del grafo. |
| | `ElementoNodo` | Enlace de la lista que contiene un `Nodo`. |
| | `ListaAristas` | Almacena, busca y elimina conexiones dirigidas. |
| | `ElementoArista` | Enlace de la lista que contiene una `Arista`. |
| | `Cola` | Estructura FIFO utilizada en BFS. |
| | `NodoCola` | Dato entero y enlace de la cola. |
| | `Pila` | Estructura LIFO utilizada en DFS. |
| | `NodoPila` | Dato entero y enlace de la pila. |
| `src/simulacion/` | `Grafo` | Edición de la red, pesos, congestión, BFS, DFS, Dijkstra y fórmula tarifaria. |
| | `Simulacion` | Construye la ciudad, crea los taxis, genera pasajeros y avanza las recogidas y entregas. |
| | `RelojSimulacion` | Hora, día, períodos de tráfico y nivel de oscuridad. |
| `src/taxis/` | `Taxi` | Estado compartido, ruta, movimiento, pasajero a bordo, viajes e ingresos. |
| | `TaxiBFS` | Calcula las rutas mediante BFS. |
| | `TaxiDFS` | Calcula las rutas mediante DFS. |
| | `TaxiDijkstra` | Calcula las rutas mediante Dijkstra. |
| | `TaxiManual` | Valida los siguientes nodos elegidos por el usuario según el sentido de las calles. |
| `src/interfaz/` | `MainWindow` | Conecta controles y simulación, presenta tablas y eventos y coordina el editor. |
| | `MapaWidget` | Dibuja la ciudad y sus entidades e interpreta las interacciones sobre el mapa. |
| | `BarrasGananciasWidget` | Representa los ingresos actuales con barras animadas. |
| | `GraficoGananciasWidget` | Presenta la evolución reciente de los ingresos con Qt Charts. |
| | `DialogoNodo` | Solicita y valida los datos del formulario de creación de nodos. |
| | `TextoFlotante` | Posición, texto, color y edad de un aviso temporal sobre el mapa. |
| | `TransicionesPanel` | Gestiona el desplazamiento y desvanecimiento breve al cambiar de pestaña. |

## Por dónde empezar a leer

1. `src/main.cpp` configura Qt, la paleta y los estilos y abre `MainWindow`.
2. `ui/mainwindow.ui` define los controles y la distribución de la ventana.
3. `src/interfaz/MainWindow.cpp` conecta esos controles con las operaciones de la simulación.
4. `src/simulacion/Simulacion.cpp` permite seguir una partida desde la generación de pasajeros hasta el cobro de una carrera.
5. `src/simulacion/Grafo.cpp` contiene las búsquedas y la edición de calles y cruces.
6. `src/interfaz/MapaWidget.cpp` explica cómo se representa ese estado en pantalla.

Para cambiar colores y espaciados, empieza por `recursos/interfaz.qss`. Para cambiar un formulario, abre el archivo correspondiente de `ui/` con Qt Designer. El detalle de las reglas y de la propiedad de memoria está en [Arquitectura](docs/arquitectura.md).

## Compilar y ejecutar

Se necesita CMake 3.21 o posterior, un compilador con C++17 y Qt 6 con Widgets y Charts. En Windows, utiliza un kit de Qt y un compilador de la misma arquitectura y familia. La configuración de desarrollo utilizada es Qt 6.11.0 para MinGW de 64 bits.

### Qt Creator

1. Abre el `CMakeLists.txt` de la raíz.
2. Selecciona el kit Qt 6 MinGW de 64 bits.
3. Configura un directorio de compilación nuevo, por ejemplo `build/QtCreator-Release`, y el tipo `Release`.
4. Compila y ejecuta el objetivo `TaxiGraph`.

Después de cambiar la ubicación del repositorio o su distribución, usa una configuración nueva. La caché de CMake guarda rutas absolutas al directorio fuente y a sus herramientas.

### PowerShell con Ninja

Ejecuta los comandos desde la raíz del repositorio. Ajusta las rutas de Qt y sus herramientas a tu instalación.

```powershell
$qt = 'C:\Qt\6.11.0\mingw_64'
$cmake = 'C:\Qt\Tools\CMake_64\bin\cmake.exe'
$ninja = 'C:/Qt/Tools/Ninja/ninja.exe'
$compilador = 'C:/Qt/Tools/mingw1310_64/bin/g++.exe'
$env:PATH = "C:\Qt\Tools\mingw1310_64\bin;$qt\bin;" + $env:PATH

& $cmake -S . -B 'build/Release' -G Ninja `
    '-DCMAKE_BUILD_TYPE=Release' `
    "-DCMAKE_PREFIX_PATH=$qt" `
    "-DCMAKE_CXX_COMPILER=$compilador" `
    "-DCMAKE_MAKE_PROGRAM=$ninja"

& $cmake --build 'build/Release' --parallel 3
& '.\build\Release\TaxiGraph.exe'
```

El ejecutable requiere las bibliotecas de Qt. Los comandos anteriores las hacen accesibles mediante `PATH` durante el desarrollo. Para distribuir la aplicación en otro equipo, prepara una carpeta de entrega con las dependencias mediante `windeployqt` del mismo kit utilizado para compilar.

## Fuentes y archivos generados

Los archivos de `src/`, `ui/`, `recursos/`, `docs/` y la configuración de CMake forman la entrega del proyecto. Qt genera `ui_mainwindow.h`, `ui_nodedialog.h`, archivos `moc_*.cpp`, recursos compilados y objetos dentro del directorio de compilación. Esos archivos se regeneran al compilar. Los cambios de los formularios se realizan en `ui/` y los de recursos en `recursos/`.

La carpeta `.qtcreator/`, los archivos `.user` y `build/` corresponden a la configuración local o al resultado de la compilación y están excluidos del control de versiones mediante `.gitignore`.

La cobertura de la revisión y sus pruebas está en [Revisión final](docs/revision-final.md).
