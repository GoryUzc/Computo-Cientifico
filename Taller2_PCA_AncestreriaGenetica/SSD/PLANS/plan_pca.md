# Plan de Implementación — PCA Genético

> Plan paso a paso para desarrollar el Taller 2 de Cálculo Científico.

---

## Fase 1: Preparación del entorno 

| Paso | Acción | Comando / Acción | Estado |
|------|--------|------------------|--------|
| 1.1 | Crear estructura de carpetas | Según `SSD/ARCH_DESIGN.md` | ✅ |
| 1.2 | Verificar Eigen instalado | `vcpkg list \| findstr eigen3` | ✅ |
| 1.3 | fast-cpp-csv-parser disponible | `include/csv.h` presente | ✅ |
| 1.4 | Crear `python/requirements.txt` | numpy, matplotlib, pandas | ✅ |

**Checkpoint:** `Eigen3::Eigen` disponible, `csv.h` accesible, Python lista.

---

## Fase 2: Implementar módulos C++

> Implementar según especificaciones técnicas `SPEC-001.md`.

### 2.1 Módulo `cargador_csv` ✅

**Archivos:** `src/cargador_csv.hpp` + `src/cargador_csv.cpp`

**Estado:** Implementado (18 + 97 líneas)
- `DatosGenotipo` con vector plano (column-major) para compatibilidad Eigen
- Validación de valores ∈ {0, 1, 2}
- `fast-cpp-csv-parser` con header_row(0)

---

### 2.2 Módulo `procesador_pca` ✅

**Archivos:** `src/procesador_pca.hpp` + `src/procesador_pca.cpp`

**Estado:** Implementado (24 + 69 líneas)
- `filtrarSNPsConstantes()` elimina SNPs con p_j = 0 o p_j = 1
- `normalizarPatterson()` aplica fórmula: G_tilde = (G - 2p) / sqrt(2p(1-p))
- Validación de media ≈ 0 después de normalización

---

### 2.3 Módulo `svd_calculador` ✅

**Archivos:** `src/svd_calculador.hpp` + `src/svd_calculador.cpp`

**Estado:** Implementado (15 + 36 líneas)
- `calcularPCA()` usa BDCSVD con `ComputeThinU | ComputeThinV`
- Retorna PC = U × Σ
- Calcula varianza explicada: var_pc1 = (σ₁² / Σσᵢ²) × 100

---

### 2.4 Módulo `exportador` ✅

**Archivos:** `src/exportador.hpp` + `src/exportador.cpp`

**Estado:** Implementado (14 + 29 líneas)
- `exportarPCA()` escribe CSV con PC1, PC2 (10 decimales)
- `exportarVarianza()` escribe porcentajes de varianza

---

### 2.5 Módulo `main.cpp` ✅

**Archivo:** `src/main.cpp`

**Estado:** Implementado (45 líneas)
- Pipeline de 5 pasos con try/catch (sin llamada a Python)
- Logging a consola de cada etapa


## Fase 3: Build system ✅

**Archivo:** `CMakeLists.txt`

**Estado:** Implementado (36 líneas)
- C++17 requerido
- `find_package(Eigen3 CONFIG REQUIRED)`
- Target `pca_genetica` con todos los módulos
- Opciones: `/W4` (MSVC), `-Wall -Wextra` (GCC/Clang)

---

## Fase 4: Script Python ✅

**Archivo:** `python/visualizar_pca.py`

>Implementar según especificaciones técnicas `SPEC-002.md`.

**Estado:** Implementado (56 líneas)
- `leer_pca()` y `leer_varianza()` con pandas
- `generar_scatter_plot()` con matplotlib (backend Agg)
- Numeración automática de individuos
- Argumentos CLI con `argparse`

---

## Fase 5: Compilación y pruebas

| Paso | Acción | Command |
|------|--------|---------|
| 5.1 | Crear build | `mkdir build` |
| 5.2 | Configurar | `cmake .. -G "Visual Studio 17 2022"` (Windows) o `cmake ..` (Linux) |
| 5.3 | Compilar | `cmake --build . --config Release` |
| 5.4 | Ejecutar C++ | `.\Release\pca_genetica.exe` |
| 5.5 | Verificar results | `resultados/resultado_pca.csv` existe |
| 5.6 | Verificar varianza | `resultados/varianza.csv` existe |
| 5.7 | Generar gráfico | `python python/visualizar_pca.py` |
| 5.8 | Verificar gráfico | `resultados/grafico_pca.png` existe |

---

## Checkpoints de validación

| Checkpoint | Cómo verificar | Estado |
|------------|----------------|--------|
| ✅ Carga CSV | 100 SNPs, 30 individuos en resultados | ✅ |
| ✅ Filtrado | 97 SNPs sobreviven (3 descartados típicamente) | ✅ |
| ✅ Normalización | Media columnas G_tilde < 1e-10 | ✅ |
| ✅ SVD | Convergencia sin errores | ✅ |
| ✅ Exportación | `resultado_pca.csv` 30 filas, 3 columnas | ✅ |
| ✅ Gráfico | `grafico_pca.png` generado, 30 puntos | ✅ |

---


