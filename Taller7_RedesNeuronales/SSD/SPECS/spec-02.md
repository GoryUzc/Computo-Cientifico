# SPEC-002: Visualizador Python de la red neuronal

> **Proyecto:** Taller 7 — Entrenamiento de red neuronal con Backpropagation  
> **Módulo:** `python/visualizar_red.py`  
> **Responsabilidad:** leer los CSV exportados por C++, generar gráficas de convergencia y ajuste, y reconstruir el resumen comparativo.

---

## 1. Propósito y dependencias

El script visualiza los 15 experimentos (3 tasas de aprendizaje × 5 semillas):

1. Una gráfica de pérdida contra época para cada tasa `0.001`, `0.01` y `0.5`.
2. Una gráfica de ajuste para `alpha = 0.01`, con las predicciones finales de las semillas disponibles y la función de referencia.
3. Un resumen por experimento ordenado por pérdida final, guardado en CSV y mostrado en consola.

Dependencias Python declaradas en `python/requirements.txt`: pandas 2.0 o posterior, NumPy 1.24 o posterior y Matplotlib 3.7 o posterior.

El módulo contiene funciones (`load_results`, `plot_convergence_histograms`, `plot_fit_curves`, `plot_summary_table` y `main`); no implementa la clase `VisualizadorRedNeuronal`.

## 2. Datos de entrada

Los CSV se esperan en `resultados/` relativo al directorio de trabajo actual. El programa C++ debe ejecutarse antes que el visualizador.

| Archivos | Encabezados | Cantidad |
|---|---|---:|
| `entrenamiento_alfa_<código>_seed_<semilla>.csv` | `epoch,loss` | Hasta 15 |
| `predicciones_alfa_<código>_seed_<semilla>.csv` | `x,y_real,y_pred` | Hasta 15 |

El código de la tasa en el nombre se calcula como `int(round(alpha * 1000))`: `1`, `10` y `500`, respectivamente para `0.001`, `0.01` y `0.5`. Los historiales contienen la época 0 y las 10 000 épocas de entrenamiento (10 001 registros, más encabezado); las predicciones corresponden a las 100 muestras de entrenamiento.

El visualizador recorre las combinaciones configuradas en `ALPHAS` y `SEEDS`. Si falta un historial, omite esa combinación. Si existe un historial pero falta su CSV de predicciones, lanza `FileNotFoundError`. Si no encuentra ningún historial, también informa el error con la instrucción de ejecutar primero el programa C++.

## 3. Comportamiento de las funciones

### `load_results(results_dir="resultados")`

Carga las columnas `epoch` y `loss` del historial, y las columnas de predicciones `x`, `y_real` y `y_pred`. Devuelve una entrada por experimento encontrado con la tasa, semilla, épocas, pérdidas y DataFrame de predicciones.

### `plot_convergence_histograms(results, results_dir="resultados")`

Crea `resultados/graficos/` si no existe y guarda una figura por cada valor de `ALPHAS`. Cada figura mide 12 × 6 pulgadas, presenta las curvas disponibles coloreadas por semilla, incluye leyenda y cuadrícula, y se guarda a 300 DPI. La escala de los ejes es lineal.

### `plot_fit_curves(results, results_dir="resultados")`

Superpone las predicciones `y_pred` contra sus valores `x` para los experimentos con `alpha = 0.01`. La referencia se calcula directamente como \(f(x)=x^2+\sin(5x)\) sobre 500 puntos entre -2 y 2; no se lee desde `funcion_real.csv`. Si no hay predicciones para esa tasa, muestra una advertencia y no guarda la gráfica.

### `plot_summary_table(results, results_dir="resultados")`

Calcula para cada experimento:

- `loss_final`: último valor del historial.
- `converges`: si algún valor del historial es menor que `CONVERGENCE_THRESHOLD` (0.1).
- `epoch_converged`: primera época que alcanza el umbral, o `-1` si no se alcanza.

Python incluye la época 0 en esta comprobación; el entrenador C++ no la usa para determinar `converged` y revisa únicamente las pérdidas devueltas durante `train_step`.

Ordena las filas ascendentemente por pérdida final, escribe `resultados/resumen_comparativo.csv` y muestra la tabla en consola. Este proceso sobrescribe el resumen previamente exportado por C++.

## 4. Archivos de salida

```text
resultados/
├── graficos/
│   ├── convergencia_alfa_0.001.png
│   ├── convergencia_alfa_0.01.png
│   ├── convergencia_alfa_0.5.png
│   └── ajuste_alfa_0.01_todas_semillas.png
└── resumen_comparativo.csv
```

Las tres gráficas de convergencia se guardan individualmente, a 300 DPI. La gráfica de ajuste se crea cuando hay resultados con `alpha = 0.01`. El resumen generado por Python tiene las columnas `alpha,seed,loss_final,converges,epoch_converged`; el generado por el exportador C++ utiliza `alpha,seed,loss_final,convergio,epoca_convergencia`.

## 5. Ejecución

Desde la raíz del proyecto:

```powershell
python python\visualizar_red.py
```

La ruta de resultados usada por `main` es relativa al directorio desde el que se ejecuta el comando. Las funciones de carga y generación también permiten pasar explícitamente otro directorio de resultados.

---

*Fin de SPEC-002*
