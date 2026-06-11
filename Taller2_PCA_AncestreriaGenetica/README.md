# Taller 2 - PCA e Identificación de Ancestría Genética

Implementación en C++ del Análisis de Componentes Principales (PCA) aplicado a datos genéticos de SNPs para identificar estructura poblacional y detectar procesos de mezcla (admixture).

## Tabla de Contenidos

- [Requisitos](#requisitos)
- [Compilación](#compilación)
- [Ejecución](#ejecución)
- [Estructura del Proyecto](#estructura-del-proyecto)
- [Pipeline del PCA](#pipeline-del-pca)
- [Interpretación de Resultados](#interpretación-de-resultados)

---

## Requisitos

### Software requerido

| Herramienta | Versión mínima | Descripción |
|-------------|----------------|-------------|
| **CMake** | 3.16+ | Sistema de build |
| **Compilador C++** | GCC 11+, Clang 14+, o MSVC 2022 | Soporte C++17 |
| **Eigen3** | 3.4+ | Álgebra lineal (header-only) |
| **fast-cpp-csv-parser** | 1.0 | Header-only en `include/csv.h` |
| **Python** | 3.10+ | Visualización de resultados |

### Dependencias Python

```bash
pip install numpy matplotlib pandas
```

O usar el archivo de requerimientos:

```bash
pip install -r python/requirements.txt
```

---

## Compilación

### Windows (MSVC)

```powershell
# 1. Crear directorio de compilación
mkdir build
cd build

# 2. Configurar con CMake (ajustar ruta de vcpkg si es necesario)
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE="C:/Users/User/vcpkg/scripts/buildsystems/vcpkg.cmake"

# 3. Compilar en Release
cmake --build . --config Release

# 4. El ejecutable se genera en: build\Release\pca_genetica.exe
```

### Linux/macOS (GCC/Clang)

```bash
# 1. Crear directorio de compilación
mkdir build && cd build

# 2. Configurar con CMake
cmake ..

# 3. Compilar
cmake --build . -j$(nproc)

# 4. El ejecutable se genera en: build/pca_genetica
```

---

## Ejecución

### Ejecutar el pipeline C++

```powershell
# Windows
.\build\Release\pca_genetica.exe

# Linux/macOS
./build/pca_genetica
```

### Ejecutar visualización Python

```bash
cd python
python visualizar_pca.py
```

### Salida esperada

```
=== PASO 1: Cargar datos ===
SNPs originales: 100, Individuos: 30

=== PASO 2: Filtrar SNPs constantes ===
SNPs filtrados: 97

=== PASO 3: Normalizar Patterson ===

=== PASO 4: Calcular PCA ===
Varianza PC1: XX.XX%, PC2: XX.XX%

=== PASO 5: Exportar resultados ===
Exportados: resultados/resultado_pca.csv, resultados/varianza.csv

Pipeline completado con exito.
```

---

## Estructura del Proyecto

```
Taller2_PCA_AncestreriaGenetica/
├── CMakeLists.txt                          # Build system
├── README.md                               # Este archivo
├── docs/
│   └── informe_pca.tex                     # Informe LaTeX
├── SSD/
│   ├── SPECS/
│   │   ├── spec-001.md                     # Especificación C++
│   │   └── spec-002.md                     # Especificación Python
│   ├── PLANS/
│   │   └── plan_pca.md                     # Plan de desarrollo
│   └── ARCH_DESIGN.md                       # Arquitectura y teoría
├── src/
│   ├── main.cpp                            # Orquestador (45 líneas)
│   ├── cargador_csv.hpp/cpp                # Carga CSV (18+97 líneas)
│   ├── procesador_pca.hpp/cpp              # Normalización Patterson (24+69 líneas)
│   ├── svd_calculador.hpp/cpp              # SVD con Eigen (15+36 líneas)
│   ├── exportador.hpp/cpp                  # Exportación CSV (14+29 líneas)
│   └── utils.hpp                           # Utilidades (11 líneas)
├── data/
│   └── datos_genotipo.csv                  # Dataset entrada (100 SNPs × 30 individuos)
├── resultados/
│   ├── resultado_pca.csv                   # Coordenadas PC1, PC2 (30 filas)
│   ├── varianza.csv                        # Varianza explicada (PC1, PC2)
│   └── grafico_pca.png                     # Scatter plot PC1 vs PC2
├── python/
│   ├── visualizar_pca.py                   # Script visualización
│   └── requirements.txt                      # Dependencias Python
└── include/
    └── csv.h                               # fast-cpp-csv-parser
```

---

## Pipeline del PCA

### Flujo de procesamiento

```
datos_genotipo.csv
       ↓
   [CARGAR] → DatosGenotipo
       ↓
   [FILTRAR] → Elimina SNPs constantes (p_j = 0 o p_j = 1)
       ↓
   [NORMALIZAR] → Fórmula de Patterson: G_tilde = (G - 2p) / sqrt(2p(1-p))
       ↓
   [SVD] → G_tilde^T = U × Σ × V^T
       ↓
   [PROYECTAR] → PC = U × Σ
       ↓
   [EXPORTAR] → resultado_pca.csv + varianza.csv
       ↓
   [VISUALIZAR] → grafico_pca.png (scatter PC1 vs PC2)
```

### Implementación por módulos

| Módulo | Archivo | Responsabilidad |
|--------|---------|-----------------|
| `cargador_csv` | cargador_csv.hpp/cpp | Lectura CSV, validación valores {0,1,2} |
| `procesador_pca` | procesador_pca.hpp/cpp | Frecuencias, filtrado, normalización Patterson |
| `svd_calculador` | svd_calculador.hpp/cpp | Descomposición SVD, cálculo PCA |
| `exportador` | exportador.hpp/cpp | Escritura CSV de resultados |
| `main` | main.cpp | Orquestación del pipeline |

---

## Interpretación de Resultados

### Archivo resultado_pca.csv

```csv
Individuo,PC1,PC2
Individuo_1,0.2345,-1.5678
Individuo_2,-0.8765,2.3456
...
```

- **Individuo**: Nombre del individuo
- **PC1**: Primera componente principal (máxima varianza)
- **PC2**: Segunda componente principal (ortogonal a PC1)

### Archivo varianza.csv

```csv
Componente,VarianzaExplicada
PC1,25.30
PC2,18.70
```

- **PC1**: Porcentaje de varianza explicada por el primer componente
- **PC2**: Porcentaje de varianza explicada por el segundo componente

### Patrones en el gráfico PC1 vs PC2

| Patrón visual | Interpretación genética |
|---------------|----------------------|
| Clusters separados | Poblaciones distintas con barreras reproductivas |
| Individuos entre clusters | Admixture (mezcla de ancestros) |
| Cluster único difuso | Población homogénea sin estructura |
| Gradiente continuo | Clinalidad genética (migración gradual) |

---

## Referencias

1. Patterson, N., Price, A. L., & Reich, D. (2006). *Population structure and eigenanalysis.* PLoS Genetics, 2(12), e190.
2. Price, A. L., et al. (2006). *Principal components analysis corrects for stratification in genome-wide association studies.* Nature Genetics, 38(8), 904-909.
3. Eigen Documentation: https://eigen.tuxfamily.org/dox/group__SVD__Module.html
4. fast-cpp-csv-parser: https://github.com/ben-strasser/fast-cpp-csv-parser