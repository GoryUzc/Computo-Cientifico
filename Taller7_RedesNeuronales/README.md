# Taller 7 — Red neuronal con Backpropagation

Proyecto de cómputo científico que implementa en C++ una red neuronal para aproximar una función no lineal. El flujo genera datos con ruido, entrena la red con distintas inicializaciones y tasas de aprendizaje, exporta los resultados a CSV y permite visualizarlos con Python.

## Especificaciones técnicas

- **Lenguaje y estándar:** C++17.
- **Construcción:** CMake 3.10 o posterior.
- **Álgebra lineal:** Eigen3.
- **Visualización y análisis:** Python 3, NumPy, pandas y Matplotlib.
- **Función objetivo:** \(f(x) = x^2 + \sin(5x)\).
- **Dominio de entrada:** \(x \in [-2, 2]\).
- **Ruido de entrenamiento:** distribución normal \(\mathcal{N}(0, 0.5^2)\).
- **Datos de entrenamiento:** 100 muestras, generadas una sola vez con semilla 7 para comparar los experimentos sobre el mismo conjunto.
- **Red:** una entrada, una capa oculta de 20 neuronas con ReLU y una salida lineal.
- **Entrenamiento:** propagación hacia delante y Backpropagation implementados con Eigen; actualización mediante descenso de gradiente por lotes.
- **Inicialización:** distribución normal con desviación estándar \(\sqrt{2/H}\) para pesos y sesgos ocultos; sesgo de salida inicializado a cero.
- **Épocas:** 10 000 por experimento.
- **Criterio de convergencia:** primera época con pérdida menor que 0.1; si no se alcanza, se reporta `-1`.
- **Experimentos:** producto de 3 tasas de aprendizaje (`0.001`, `0.01`, `0.5`) por 5 semillas (`42`, `123`, `456`, `789`, `1024`), para un total de 15 ejecuciones.
- **Pérdida:** \(J = \frac{1}{2n}\sum_{i=1}^{n}(\hat{y}_i-y_i)^2\).

## Estructura del proyecto

```text
.
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── data_generator.hpp / .cpp
│   ├── neural_network.hpp / .cpp
│   ├── trainer.hpp / .cpp
│   └── exporter.hpp / .cpp
├── python/
│   ├── visualizar_red.py
│   └── requirements.txt
├── resultados/
│   ├── CSV de datos, historiales, predicciones y resumen
│   └── graficos/
└── SSD/
    └── SPECS/
```

## Requisitos previos

1. Un compilador compatible con C++17 y CMake 3.10 o posterior.
2. Eigen3 instalado y disponible para CMake mediante su paquete de configuración (`Eigen3Config.cmake`).
3. Python 3 con pip.

Instala las dependencias de visualización desde la raíz del proyecto:

```powershell
python -m pip install -r python\requirements.txt
```

## Compilar y ejecutar

Desde la raíz del proyecto, configura y compila:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

Si Eigen3 no está en una ruta que CMake detecte automáticamente, proporciona su prefijo de instalación al configurar, por ejemplo mediante `-DCMAKE_PREFIX_PATH=<ruta-a-Eigen3>`.

Ejecuta el binario desde la raíz del proyecto para que los resultados se escriban en su carpeta `resultados`:

```powershell
.\build\Release\backprop_nn.exe
```

Después genera las gráficas y el resumen con:

```powershell
python python\visualizar_red.py
```

El visualizador necesita los CSV generados por el programa C++; si no los encuentra, informa que primero debe ejecutarse el entrenamiento. Tanto el programa C++ como el script Python usan rutas relativas al directorio de trabajo actual.

## Archivos de salida

El entrenamiento genera los siguientes CSV en `resultados/`:

| Archivo | Contenido |
|---|---|
| `datos_entrenamiento.csv` | Muestras del conjunto de entrenamiento (`x`, `y_real`, `y_pred`); en este archivo `y_pred` contiene los valores generados con ruido. |
| `funcion_real.csv` | 500 puntos ordenados del dominio para representar la función (`x`, `y_real`, `y_pred`). |
| `entrenamiento_alfa_<código>_seed_<semilla>.csv` | Historial de pérdida con columnas `epoch`, `loss`; contiene época 0 más las épocas entrenadas. |
| `predicciones_alfa_<código>_seed_<semilla>.csv` | Datos y predicciones finales con columnas `x`, `y_real`, `y_pred`. |
| `resumen_comparativo.csv` | Una fila por experimento con `alpha`, `seed`, `loss_final`, `convergio` y `epoca_convergencia`. |

El `<código>` de la tasa en los nombres de archivo es el valor de `alpha × 1000`: `1` para `0.001`, `10` para `0.01` y `500` para `0.5`.

El visualizador crea `resultados/graficos/` si aún no existe y guarda cuatro imágenes PNG (tres curvas de convergencia y una gráfica de ajuste para `alpha = 0.01`). También actualiza `resultados/resumen_comparativo.csv` con el resumen leído de los historiales.

## Componentes

- `data_generator`: genera entradas uniformes en `[-2, 2]` y evalúa la función con ruido gaussiano.
- `neural_network`: calcula la salida de la red, la pérdida, los gradientes y actualiza pesos y sesgos.
- `trainer`: ejecuta los 15 experimentos y registra la época de convergencia.
- `exporter`: exporta los historiales, predicciones y el resumen en CSV.
- `python/visualizar_red.py`: carga los resultados, genera las gráficas y produce el resumen comparativo.

Las especificaciones detalladas de los módulos están en [`SSD/SPECS/spec-01.md`](SSD/SPECS/spec-01.md) y [`SSD/SPECS/spec-02.md`](SSD/SPECS/spec-02.md).
