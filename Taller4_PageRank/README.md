# PageRank — Taller 4 Cálculo Científico

Implementación en **C++17 + Eigen** del algoritmo PageRank (Método de las
Potencias) y visualización en **Python** (pandas / matplotlib / tabulate).

## Diseño

- Entrada: matrices de transición **$M$ ya normalizadas** (especificadas en
  `SDD/SPECS/SPEC-001.md` y hardcodeadas en `src/casos_prueba.cpp`).
- No hay carga externa de grafos: **no existe** `graph_loader` ni `casos_prueba/*.txt`.
- Salida C++: **21 CSVs** en `resultados/` (5 casos × 4 archivos + resumen).
- Salida Python: **16 tablas** CSV/LaTeX y **4 PNG 300 DPI** en
  `resultados/tablas/` y `resultados/graficos/`.

## Requisitos

- Windows / Linux / macOS
- CMake >= 3.16
- Visual Studio 2022 / GCC >= 9 / Clang >= 10
- Eigen 3.4+ (vía vcpkg o descarga directa)
- Python 3.10+
- pip packages: `numpy`, `pandas`, `matplotlib`, `tabulate`

## Compilación (C++)

```powershell
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build . --config Release
.\Release\pagerank.exe
```

> **Nota:** ejecuta `pagerank.exe` desde `build/` para que `../resultados`
> resuelva correctamente a la carpeta del proyecto.

## Ejecución (C++)

```powershell
cd build
.\Release\pagerank.exe
```

Archivos generados:

```
resultados/
├── caso1_matriz_M.csv
├── caso1_matriz_G.csv
├── caso1_pagerank.csv
├── caso1_convergencia.csv
├── ... (caso2..5)
└── resumen_comparativo.csv
```

## Ejecución (Python)

```bash
pip install -r python/requirements.txt
cd python
PYTHONIOENCODING=utf-8 python visualizar_pagerank.py
```

> `PYTHONIOENCODING=utf-8` solo es necesario en Windows para caracteres
> `Σ` en consola. En Linux/macOS no es necesario.

Archivos generados:

```
resultados/
├── tablas/
│   ├── tabla_01_resumen_general.csv + .tex
│   ├── tabla_02_ranking_caso{1..5}.csv + .tex
│   ├── tabla_03_convergencia.csv + .tex
│   └── tabla_04_matrices.csv + .tex
└── graficos/
    ├── 01_convergencia_todos_casos.png
    ├── 02_pagerank_por_caso.png
    ├── 03_heatmap_matrices_G.png
    └── 04_comparacion_top_pages.png
```

## Verificación de Especificación

### SPEC-001 (C++)

| Criterio | Estado |
|----------|--------|
| Matrices $M$ hardcodeadas (sin archivos externos) | ✅ |
| Clase `PageRank` con `transicion_` (sin `adyacencia_`) | ✅ |
| `construir_matriz_google()` cachea `google_` (no-const) | ✅ |
| Historial incluye estado inicial (it 0) | ✅ |
| Renormalización final `v = v / v.sum()` | ✅ |
| Validación tolera sumidero exacto 0.0 | ✅ |
| `casoN_pagerank.csv` 4 columnas | ✅ |
| `resumen_comparativo.csv` 10 columnas (con `suma_pagerank`) | ✅ |
| `casoN_matriz_{M,G}.csv` formato largo ($n\times n$) | ✅ |

### SPEC-002 (Python)

| Criterio | Estado |
|----------|--------|
| 16 tablas generadas (CSV + LaTeX) | ✅ |
| 4 gráficas PNG a 300 DPI | ✅ |
| Reporte estadístico en consola | ✅ |
| Tablas de ranking usan `grado_entrada`/`grado_salida` | ✅ |

### Criterios Numéricos (SPEC-001 §10.1)

| Criterio | Resultado |
|----------|-----------|
| Suma de PageRank | $1.0 \pm 10^{-6}$ en los 5 casos |
| No negatividad | $\min(\text{pagerank}) \geq 0$ |
| Convergencia $\epsilon = 10^{-8}$ | Todos los casos |
| Iteraciones < 1000 | 23 / **1** / 451 / 99 / 109 |

### Observaciones

- **Matriz $G$ estocástica:** requiere columnas de $M$ que sumen 1.
  Casos 1, 2, 3 tienen columnas no-unitarias o sumideros; para esas
  columnas $G$ suma $1-d \neq 1$. Esto no afecta la validez del PageRank
  final (se renormaliza a 1 al final de `ejecutar()`).
- **Caso 5:** el resultado físico esperado por la especificación ("nodo 2
  tiene mayor PageRank") no coincide con la matriz $M$ provista por el
  profesor. El algoritmo produce resultados correctos y válidos (nodo 0
  mayor, nodo 2 sumidero).

## Estructura C++

```
src/
├── main.cpp                  # Orquestador
├── pagerank_core.{hpp,cpp}  # Clase PageRank + G + potencias
├── casos_prueba.{hpp,cpp}   # 5 matrices M hardcodeadas
└── exportador.{hpp,cpp}     # CSV: 4-col, long-form, resumen 10-col
```

## Rutas Relativas

El C++ se ejecuta desde `build/`, y escribe `../resultados/casoN_*.csv` para
ubicar los CSV en la carpeta `resultados/` del proyecto. El script Python
busca `resultados/` relativo a su ubicación (`python/../resultados`).

## Licencia

Trabajo académico — Taller 4, Cálculo Científico.
