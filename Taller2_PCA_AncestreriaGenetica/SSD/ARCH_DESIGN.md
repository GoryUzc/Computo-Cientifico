# ARQUITECTURA Y DISEÑO: Proyecto — PCA e Identificación de Ancestría Genética

> Documento de contexto para el Taller 2 de Cálculo Científico.

---

## 1. Stack

| Capa | Tecnología | Versión | Propósito |
|------|-----------|---------|-----------|
| **Cálculo numérico** | C++17 | ISO/IEC 14882:2017 | Núcleo: carga, normalización, SVD |
| **Álgebra lineal** | Eigen 3.4 | 3.4.0+ | Matrices densas, SVD (`BDCSVD`) |
| **Parser CSV** | fast-cpp-csv-parser | 1.0 (header-only) | Lectura de `datos_genotipo.csv` |
| **Build** | CMake | 3.16+ | Generación de targets |
| **Visualización** | Python 3.10+ | 3.10+ | Scatter plot PC1 vs PC2 |
| **Gráficos** | matplotlib | 3.7+ | Renderizado de figuras |
| **Gestor paquetes C++** | vcpkg | latest | Eigen, csv-parser |

**Compilador:** Visual Studio 2022 (MSVC) / GCC 11+ / Clang 14+

---

## 2. Objetivo

Implementar el pipeline completo de Análisis de Componentes Principales (PCA) aplicado a datos genéticos de SNPs, para identificar estructura poblacional y detectar procesos de mezcla (admixture) entre grupos.

**Flujo:**

```
datos_genotipo.csv  →  Carga  →  Limpieza  →  Normalización Patterson  →  SVD  →  PC = UΣ  →  CSV  →  Python  →  scatter_plot.png
```

---

## 3. Conceptos Fundamentales

### 3.1 Datos de Entrada

Archivo: `datos_genotipo.csv`

| Propiedad | Valor |
|-----------|-------|
| **Formato en disco** | 30 filas de individuos × 101 columnas (1 vacía + 100 SNPs) + encabezado |
| **Representación interna** | $G \in \mathbb{R}^{100 \times 30}$ (100 SNPs filas, 30 individuos columnas) |
| **Filas reales** | `Individuo_1`, `Individuo_2`, ..., `Individuo_30` |
| **Columnas reales** | `SNP_1`, `SNP_2`, ..., `SNP_100` (primera columna vacía en encabezado) |
| **Valores** | `0`, `1`, `2` |

> **Nota:** El cargador transpone automáticamente al leer: cada fila del CSV es un individuo, cada columna SNP pasa a ser una fila de `G`.

**Codificación genética:**

| Valor | Genotipo | Significado |
|-------|----------|-------------|
| `0` | `AA` | Homocigoto referencia (0 copias del alelo variante) |
| `1` | `Aa` | Heterocigoto (1 copia del alelo variante) |
| `2` | `aa` | Homocigoto variante (2 copias del alelo variante) |

### 3.2 Frecuencia del Alelo Variante

Para cada SNP $j$ (columna $j$ de $G$):

```
p_j = (Σ_{i=1}^{n} G_{i,j}) / (2n)
```

Donde:
- $n = 30$ (número de individuos)
- Numerador = suma de todos los genotipos en el SNP $j$
- Denominador = $2n$ porque cada individuo es diploide (2 alelos)

**Descarte de SNPs constantes:**

| Condición | Acción | Razón |
|-----------|--------|-------|
| $p_j = 0$ | Eliminar columna $j$ | Todos son `AA`, sin variación |
| $p_j = 1$ | Eliminar columna $j$ | Todos son `aa`, sin variación |

> **Intuición:** Un SNP constante no aporta información para diferenciar poblaciones. Matemáticamente, el denominador de la normalización se anularía.

### 3.3 Normalización de Patterson (Wright-Fisher)

Para cada elemento $G_{i,j}$ que sobrevive al filtrado:

```
G̃_{i,j} = (G_{i,j} - 2p_j) / sqrt(2 * p_j * (1 - p_j))
```

| Término | Interpretación |
|---------|---------------|
| $G_{i,j} - 2p_j$ | Desviación del genotipo respecto al promedio poblacional |
| $2p_j$ | Valor esperado del genotipo bajo equilibrio Hardy-Weinberg |
| $\sqrt{2p_j(1-p_j)}$ | Desviación estándar esperada bajo deriva genética neutra (Wright-Fisher) |
| $\tilde{G}_{i,j}$ | **Z-score genético:** cuántas desviaciones estándar está el individuo del promedio |

**Propiedades de $\tilde{G}$:**
- Media por columna ≈ 0
- Varianza por columna ≈ 1 (si la población está en equilibrio)
- Cada SNP contribuye equitativamente al análisis posterior

### 3.4 SVD y Proyección PCA

Calcular la descomposición en valores singulares:

```
G̃ = U Σ V^T
```

| Matriz | Dimensiones | Interpretación |
|--------|-------------|----------------|
| $U$ | $n \times n$ | Coordenadas de individuos en el espacio de componentes principales |
| $\Sigma$ | $n \times m'$ diagonal ($m'$ = SNPs después de filtrado) | "Energía" (varianza genética) capturada por cada componente |
| $V^T$ | $m' \times m'$ | Cargas de SNPs: qué marcadores contribuyen a cada componente |

**Coordenadas principales (PC):**

```
PC = U * Σ
```

| Columna | Nombre | Significado |
|---------|--------|-------------|
| Columna 1 | PC1 | Dirección de máxima varianza genética |
| Columna 2 | PC2 | Segunda dirección de máxima varianza (ortogonal a PC1) |

> **Nota:** PC1 y PC2 son las dos primeras columnas de la matriz $PC$.

### 3.5 Interpretación Biológica del Gráfico PC1 vs PC2

| Patrón visual | Significado genético |
|---------------|----------------------|
| **Dos clusters separados** | Dos poblaciones distintas con barreras reproductivas históricas |
| **Individuos entre clusters** | **Admixture (mezcla):** ancestros de ambas poblaciones |
| **Un solo cluster difuso** | Población homogénea o deriva genética dominante |
| **Gradiente continuo** | Clina genética (migración gradual en el espacio geográfico) |

---

## 4. Algoritmo General

```
ENTRADA:  datos_genotipo.csv (100 filas SNPs × 30 columnas individuos + encabezados)
SALIDA:   resultado_pca.csv (30 filas × 2 columnas: PC1, PC2)
          grafico_pca.png (scatter plot PC1 vs PC2)

1. CARGAR CSV
   a. Leer archivo con encabezados (filas: SNP_1..SNP_100, columnas: Individuo_1..Individuo_30)
   b. Almacenar como Eigen::MatrixXd G (100 × 30)

2. ESTIMAR FRECUENCIAS Y LIMPIAR
   a. Para cada columna j = 0..29:
      i.   Calcular suma_col = Σ_i G(i,j)
      ii.  Calcular p_j = suma_col / (2 * n)   donde n = 30
      iii. Si p_j == 0.0 o p_j == 1.0: marcar columna j como "descartar"
   b. Construir G_filtrada eliminando columnas marcadas
   c. Guardar vector p de frecuencias de columnas sobrevivientes

3. NORMALIZAR (Patterson)
   a. Inicializar G_tilde con mismas dimensiones que G_filtrada
   b. Para cada columna j sobreviviente:
      i.   Calcular denom = sqrt(2 * p_j * (1 - p_j))
      ii.  Para cada fila i:
           G_tilde(i,j) = (G_filtrada(i,j) - 2*p_j) / denom

4. SVD
   a. Calcular BDCSVD de G_tilde^T (transpuesta: individuos como filas)
      Nota: Necesitamos SVD donde individuos = filas para que U sea (n × n)
   b. Extraer U, Sigma, Vt

5. PROYECTAR PCA
   a. PC = U * Sigma   (multiplicación matricial)
   b. Extraer PC1 = columna 0 de PC
   c. Extraer PC2 = columna 1 de PC

6. EXPORTAR
   a. Guardar resultado_pca.csv con columnas: Individuo, PC1, PC2

7. VISUALIZAR (Python)
   a. Leer resultado_pca.csv
   b. Generar scatter plot: eje X = PC1, eje Y = PC2
   c. Guardar grafico_pca.png
```

---

## 5. Estructura de Carpetas

```
Taller2_PCA_AncestreriaGenetica/
├── CMakeLists.txt                       # Build system (CMake 3.16+)
├── README.md                            # Instrucciones de uso y compilación
├── docs/
│   └── informe_pca.tex                # Informe LaTeX con resultados
├── SSD/
│   ├── SPECS/
│   │   ├── spec-001.md              # Implementación C++: módulos, API
│   │   └── spec-002.md              # Script Python de visualización
│   ├── PLANS/
│   │   └── plan_pca.md              # Plan de desarrollo
│   └── ARCH_DESIGN.md                 # Arquitectura, diseño y teoría (este archivo)
├── src/
│   ├── main.cpp                       # Orquestador del flujo (45 líneas)
│   ├── cargador_csv.hpp/cpp           # Carga datos_genotipo.csv (18+97 líneas)
│   ├── procesador_pca.hpp/cpp         # Frecuencias, filtrado, normalización (24+69 líneas)
│   ├── svd_calculador.hpp/cpp         # SVD con Eigen::BDCSVD (15+36 líneas)
│   ├── exportador.hpp/cpp             # Guarda resultado_pca.csv (14+29 líneas)
│   └── utils.hpp                      # Utilidades Eigen (11 líneas)
├── data/
│   └── datos_genotipo.csv             # Dataset de entrada (100 SNPs × 30 individuos)
├── resultados/
│   ├── resultado_pca.csv              # Coordenadas PC1, PC2 exportadas (30 filas)
│   ├── varianza.csv                   # Varianza explicada por componentes
│   └── grafico_pca.png                # Scatter plot generado por Python
├── python/
│   ├── visualizar_pca.py              # Script: scatter plot PC1 vs PC2
│   └── requirements.txt               # Dependencias: numpy, matplotlib, pandas
├── include/
│   └── csv.h                          # fast-cpp-csv-parser (header-only)
└── build/                             # Directorio de compilación (generado)
```

---

## 6. Métricas y Criterios de Aceptación

| Criterio | Cómo se verifica | Estado |
|----------|-----------------|--------|
| **Carga correcta del CSV** | Matriz G tiene dimensiones 100×30, valores en {0,1,2} | ✅ Implementado |
| **Filtrado de SNPs constantes** | Al menos 1 SNP descartado si existe constante; ningún p_j = 0 o 1 en G_filtrada | ✅ Implementado |
| **Normalización correcta** | Media de cada columna de G_tilde ≈ 0 (tolerancia 1e-10) | ✅ Implementado |
| **SVD sin errores** | BDCSVD converge sin excepciones | ✅ Implementado |
| **Dimensiones de PC** | PC tiene dimensiones n × min(n,m') donde n=30, m'=SNPs filtrados | ✅ Implementado |
| **Exportación CSV** | resultado_pca.csv tiene 30 filas, 3 columnas (Individuo, PC1, PC2) | ✅ Implementado |
| **Varianza exportada** | varianza.csv con PC1 y PC2 porcentajes | ✅ Implementado |
| **Gráfico generado** | grafico_pca.png existe y muestra 30 puntos dispersos | ✅ Implementado |
| **Interpretación** | Se identifican visualmente clusters o individuos intermedios (mezcla) | ✅ Implementado |

---

## 6. Procedimiento de Compilación y Ejecución

### 6.1 Prerrequisitos

| Herramienta | Versión mínima | Comentario |
|-------------|----------------|------------|
| **CMake** | 3.16+ | Build system |
| **Compilador C++** | GCC 11+, Clang 14+, o MSVC 2022 | C++17 requerido |
| **Eigen3** | 3.4+ | Álgebra lineal (header-only) |
| **fast-cpp-csv-parser** | 1.0 | Header-only en `include/csv.h` |
| **Python** | 3.10+ | Visualización |

### 6.2 Compilación (Windows - MSVC)

```powershell
# 1. Crear directorio de compilación
mkdir build
cd build

# 2. Configurar con CMake (ajustar ruta de vcpkg)
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE="C:/Users/User/vcpkg/scripts/buildsystems/vcpkg.cmake"

# 3. Compilar en Release
cmake --build . --config Release

# 4. Ejecutar
.\Release\pca_genetica.exe
```

### 6.3 Compilación (Linux/macOS - GCC/Clang)

```bash
# 1. Crear directorio de compilación
mkdir build && cd build

# 2. Configurar con CMake
cmake ..

# 3. Compilar
cmake --build . -j$(nproc)

# 4. Ejecutar
./pca_genetica
```

### 6.4 Ejecución del script Python

```bash
# Instalar dependencias
pip install -r python/requirements.txt

# Ejecutar visualización
cd python
python visualizar_pca.py
# o
python3 visualizar_pca.py
```

### 6.5 Salida esperada

```
=== PASO 1: Cargar datos ===
SNPs originales: 100, Individuos: 30

=== PASO 2: Filtrar SNPs constantes ===
SNPs filtrados: 97

=== PASO 3: Normalizar Patterson ===

=== PASO 4: Calcular PCA ===
Varianza PC1: 25.3%, PC2: 18.7%

=== PASO 5: Exportar resultados ===
Exportados: resultados/resultado_pca.csv, resultados/varianza.csv

Pipeline completado con exito.
```

---

## 7. Referencias

1. **Patterson, N., Price, A. L., & Reich, D.** (2006). *Population structure and eigenanalysis.* PLoS Genetics, 2(12), e190. → Fundamento de la normalización de Patterson.

2. **Price, A. L., et al.** (2006). *Principal components analysis corrects for stratification in genome-wide association studies.* Nature Genetics, 38(8), 904-909. → Aplicación del PCA a estructura poblacional.

3. **Eigen Documentation.** *SVD Module.* https://eigen.tuxfamily.org/dox/group__SVD__Module.html → `BDCSVD`, `ComputeFullU`, `ComputeFullV`.

4. **fast-cpp-csv-parser.** https://github.com/ben-strasser/fast-cpp-csv-parser → Librería header-only para lectura de CSV en C++.

5. **Golub, G. H., & Van Loan, C. F.** (2013). *Matrix Computations* (4th ed.). Johns Hopkins. → Cap. 2: SVD, propiedades y algoritmos.

---
