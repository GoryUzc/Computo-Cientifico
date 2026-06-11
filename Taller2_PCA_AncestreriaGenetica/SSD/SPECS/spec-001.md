# SPEC-001 — Implementación C++: Módulos del Proyecto PCA Genético

> Especificación formal de la implementación C++:
> firmas, contratos, estructuras de datos, decisiones de diseño y build system.
>
> **Estado:** Especificación para implementación.

---

## 1. Módulo `cargador_csv` — Carga de datos genotípicos

### 1.1 Ficheros

| Fichero | Tipo | Líneas reales |
|---------|------|--------------|
| `src/cargador_csv.hpp` | Header (declaraciones) | 18 |
| `src/cargador_csv.cpp` | Implementación | 97 |

### 1.2 Dependencias

- `csv.h` (fast-cpp-csv-parser, header-only, instalado vía vcpkg)
- `Eigen/Dense` (Eigen 3.4+ desde vcpkg)
- `<string>`, `<vector>`, `<stdexcept>`

### 1.3 Estructura de datos

```cpp
// Estructura implementada (src/cargador_csv.hpp)
struct DatosGenotipo {
    int n_snps;                        // Número de SNPs (100)
    int n_individuos;                  // Número de individuos (30)
    std::vector<std::string> nombres_snps;      // {"SNP_1", ..., "SNP_100"}
    std::vector<std::string> nombres_individuos; // {"Individuo_1", ..., "Individuo_30"}
    std::vector<double> valores;       // Datos en formato column-major para Eigen
};
```

**Nota:** La implementación usa `std::vector<double>` en lugar de `Eigen::MatrixXd` directamente para:
- Evitar dependencias circulares en headers
- Permitir validación de datos antes de crear la matriz Eigen
- Facilitar el paso de datos entre módulos sin copias innecesarias

### 1.4 API

#### `DatosGenotipo cargarCSV(const std::string& ruta)`

```
Contrato:
  Entrada  : ruta — archivo CSV con encabezados.
             Primera columna: nombres de SNPs (SNP_1, SNP_2, ...)
             Primera fila: encabezados (vacío + Individuo_1, Individuo_2, ...)
             Datos: valores 0, 1, 2

  Proceso  : fast-cpp-csv-parser lee fila por fila, saltando encabezado.
             Cada fila se convierte a vector<double>.
             Se acumulan en Eigen::MatrixXd con dimensiones detectadas.

  Salida   : DatosGenotipo con G(n_snps, n_individuos), nombres y dimensiones.

  Excepción: std::runtime_error si el archivo no existe, está vacío,
             o contiene valores fuera de {0, 1, 2}.

Implementación:
  csv::CSVReader reader(ruta, csv::CSVFormat().header_row(0));

  // Extraer nombres de columnas (individuos) desde encabezado
  // Saltar primera columna (nombres de SNPs)

  // Para cada fila:
  //   nombre_snp = row[0].get<>()
  //   para j = 1..ncol-1: G(i, j-1) = row[j].get<int>()
  //   validar que valor ∈ {0, 1, 2}
```

---

## 2. Módulo `procesador_pca` — Frecuencias, filtrado y normalización

### 2.1 Ficheros

| Fichero | Tipo | Líneas reales |
|---------|------|--------------|
| `src/procesador_pca.hpp` | Header (declaraciones + structs) | 24 |
| `src/procesador_pca.cpp` | Implementación | 69 |

### 2.2 Dependencias

- `Eigen/Dense`
- `<vector>`, `<cmath>`, `<algorithm>`

### 2.3 Estructuras de datos

```cpp
// Estructuras implementadas (src/procesador_pca.hpp)
struct ResultadoFiltrado {
    std::vector<double> val_filtrados;   // Matriz filtrada en formato column-major
    std::vector<double> p;             // Frecuencias p_j de columnas sobrevivientes
    std::vector<int> indices_originales; // Índices j originales de columnas conservadas
    int n_snps_filtrados;              // m' = columnas que sobreviven
};

struct ResultadoNormalizado {
    std::vector<double> g_tilde;        // Matriz normalizada (column-major)
    int n_snps_filtrados;              // SNPs después de filtrado
    int n_individuos;                  // Número de individuos
    std::vector<double> p;             // Frecuencias usadas (copia para referencia)
};
```

### 2.4 API

#### `ResultadoFiltrado filtrarSNPsConstantes(int n_snps, int n_individuos, const std::vector<double>& valores)`

**Contrato implementado:**
- **Entrada:** `n_snps` — número de SNPs (100), `n_individuos` — número de individuos (30), `valores` — datos en formato column-major
- **Proceso:** Calcula frecuencias p_j y filtra SNPs constantes (p_j = 0 o p_j = 1)
- **Salida:** Matriz filtrada, frecuencias y mapeo de índices
- **Error:** Lanza `std::runtime_error` si el resultado filtrado está vacío

#### `ResultadoNormalizado normalizarPatterson(const ResultadoFiltrado& filtrado)`

**Contrato implementado:**
- **Entrada:** ResultadoFiltrado con G_filtrada y frecuencias p
- **Proceso:** Aplica fórmula de Patterson fila por fila
- **Salida:** G_tilde normalizada
- **Error:** Lanza `std::runtime_error` si denominador ≈ 0 o media ≠ 0

---

## 3. Módulo `svd_calculador` — SVD y proyección PCA

### 3.1 Ficheros

| Fichero | Tipo | Líneas reales |
|---------|------|--------------|
| `src/svd_calculador.hpp` | Header (declaraciones + structs) | 15 |
| `src/svd_calculador.cpp` | Implementación | 36 |

### 3.2 Dependencias

- `Eigen/Dense`, `Eigen/SVD`

### 3.3 Estructuras de datos

Este módulo no define estructuras propias; utiliza directamente tipos de Eigen:

```cpp
// Tipos usados (ver svd_calculador.hpp)
Eigen::MatrixXd SnpMatFromVector(...)     // Convierte vector column-major a Eigen::MatrixXd
Eigen::MatrixXd calcularPCA(...)        // Retorna matriz PC directamente
```

### 3.4 API

#### `Eigen::MatrixXd SnpMatFromVector(int n_snps, int n_individuos, const std::vector<double>& data)`

Función auxiliar que convierte datos column-major a matriz Eigen.

#### `Eigen::MatrixXd calcularPCA(const std::vector<double>& g_tilde, int n_snps_filtrados, int n_individuos, Eigen::VectorXd& valores_singulares, double& var_pc1, double& var_pc2)`

**Contrato implementado:**
- **Entrada:** `g_tilde` — datos normalizados, dimensiones, salida por referencia
- **Proceso:** 
  1. Convierte vector a `Eigen::MatrixXd`
  2. Transpone y calcula SVD con `BDCSVD` + `ComputeThinU | ComputeThinV`
  3. Calcula varianza explicada para PC1 y PC2
  4. Retorna PC = U * Σ
- **Salida:** Matriz PC (n_individuos × 30), valores singulares, varianzas por referencia
- **Nota:** Usa `ComputeThinU/V` (no Full) por eficiencia con n > m'

---

## 4. Módulo `exportador` — Guardado de resultados

### 4.1 Ficheros

| Fichero | Tipo | Líneas reales |
|---------|------|--------------|
| `src/exportador.hpp` | Header (declaraciones) | 14 |
| `src/exportador.cpp` | Implementación | 29 |

### 4.2 Dependencias

- `Eigen/Dense`
- `<fstream>`, `<iomanip>`, `<vector>`, `<string>`

### 4.3 API

#### `void exportarPCA(const std::string& ruta, const Eigen::MatrixXd& PC, const std::vector<std::string>& nombres_individuos)`

**Contrato implementado:**
- **Entrada:** ruta del archivo, matriz PC, nombres de individuos
- **Proceso:** Escribe CSV con formato `Individuo,PC1,PC2` (10 decimales)
- **Salida:** archivo CSV legible por Python pandas

#### `void exportarVarianza(const std::string& ruta, double var_pc1, double var_pc2)`

**Contrato implementado:**
- **Entrada:** ruta del archivo, varianzas calculadas
- **Proceso:** Escribe CSV con columnas `Componente,VarianzaExplicada`
- **Salida:** archivo CSV para análisis posterior

---

## 5. Módulo `main` — Orquestador del flujo completo

### 5.1 Fichero

| Fichero | Tipo | Líneas reales |
|---------|------|--------------|
| `src/main.cpp` | Punto de entrada | 45 |

### 5.2 Flujo de 6 pasos implementado

```
PASO 1 — Cargar datos
         cargarCSV(ruta_csv) → DatosGenotipo
         Log: "SNPs originales: 100, Individuos: 30"

PASO 2 — Filtrar SNPs constantes
         filtrarSNPsConstantes(datos.n_snps, datos.n_individuos, datos.valores) → ResultadoFiltrado
         Log: "SNPs filtrados: 97"

PASO 3 — Normalizar Patterson
         normalizarPatterson(filtrado) → ResultadoNormalizado

PASO 4 — Calcular PCA
         calcularPCA(normalizado.g_tilde, ...) → Eigen::MatrixXd (PC)
         Log: "Varianza PC1: XX.XX%, PC2: XX.XX%"

PASO 5 — Exportar resultados
         exportarPCA(ruta_pca, PC, datos.nombres_individuos)
         exportarVarianza(ruta_var, var_pc1, var_pc2)

PASO 6 — Ejecutar visualización Python
         (Nota: en la implementación actual, se omite por simplicidad)
```

### 5.3 Gestión de errores implementada

```cpp
Bloque try/catch en todo el flujo.
Cualquier std::exception se captura e imprime a stderr con código de salida 1.
```

---

## 6. Build System — CMakeLists.txt

### 6.1 Configuración implementada

```cmake
# CMakeLists.txt real
cmake_minimum_required(VERSION 3.16)
project(pca_genetica LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Eigen3 CONFIG REQUIRED)
```

### 6.2 Targets

| Target | Fuentes | Enlace |
|--------|---------|--------|
| `pca_genetica` | main.cpp, cargador_csv.cpp, procesador_pca.cpp, svd_calculador.cpp, exportador.cpp | `Eigen3::Eigen` |

### 6.3 Inclusión de headers

```cmake
target_include_directories(pca_genetica PRIVATE 
    src
    ${CMAKE_SOURCE_DIR}/include
)
```

### 6.4 Opciones de compilación

```cmake
if(MSVC)
    target_compile_options(pca_genetica PRIVATE /W4)
else()
    target_compile_options(pca_genetica PRIVATE -Wall -Wextra)
endif()
```

---

## 7. Dependencias y versiones

| Librería | Versión | Origen | Tipo | Propósito |
|----------|---------|--------|------|-----------|
| **Eigen** | 3.4+ | vcpkg | Header-only + imported target | Matrices densas, BDCSVD |
| **fast-cpp-csv-parser** | 1.0 | include/csv.h | Header-only | Lectura de CSV con encabezados |

---

## 8. Notas de diseño (actualizadas)

- **Separación de responsabilidades:**
  - `cargador_csv`: formato de archivo de entrada (CSV con encabezados)
  - `procesador_pca`: transformaciones matemáticas (frecuencias, normalización)
  - `svd_calculador`: álgebra lineal (SVD, proyección)
  - `exportador`: formato de archivo de salida (CSV para Python)
  - `main`: orquestación y logging

- **Formato de datos:** Se usa `std::vector<double>` en column-major para transferir datos entre módulos sin dependencias de Eigen en headers.

- **Eigen::BDCSVD:** Usado con `ComputeThinU | ComputeThinV` por eficiencia (matriz 30 × ~97, donde n > m').

- **Precisión numérica:** Todo en `double` (64-bit). La normalización de Patterson produce valores típicamente en rango [-5, 5].

---

## 9. Criterios de Aceptación (verificación técnica)

| Criterio | Test | Umbral | Estado |
|----------|------|--------|--------|
| Carga CSV | Dimensiones de datos | 100 SNPs, 30 individuos | ✅ |
| Filtrado | p_j ∈ (0,1) para columnas filtradas | Estricto | ✅ |
| Normalización | Media por columna de G_tilde | \|media\| < 1e-10 | ✅ |
| SVD | Convergencia sin errores | Sin excepciones | ✅ |
| PCA | Varianza PC1 + PC2 | > 40% típicamente | ✅ |
| Exportación | resultado_pca.csv | 30 filas, 3 columnas | ✅ |
| Varianza | varianza.csv | 2 filas, 2 columnas | ✅ |

---

