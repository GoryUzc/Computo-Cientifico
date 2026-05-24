# SPEC-001 — Implementación C++: Módulos del Proyecto

> Documento derivado de SPEC-000. Recoge el código C++ ya implementado
> en `src/` con su especificación formal: firmas, contratos, estructuras y
> decisiones de diseño.
>
> **Estado:** implementado y verificado (véase [Tests](#tests-unitarios)).

---

## 1. Módulo `cargador_imagen` — Carga de imágenes

### 1.1 Ficheros

| Fichero | Tipo | Líneas |
|---------|------|--------|
| `src/cargador_imagen.hpp` | Header (declaraciones) | 27 |
| `src/cargador_imagen.cpp` | Implementación | 49 |

### 1.2 Dependencias

- `stb_image.h` (header-only, incluido manualmente en `include/stb/`)
- `Eigen/Dense` (Eigen 3.4+ desde vcpkg)

### 1.3 API

#### `Eigen::MatrixXd cargarImagenGris(const std::string& ruta)`

```
Contrato:
  Entrada  : ruta — ruta a un archivo PNG/JPG en disco.
  Proceso  : stb_image carga el archivo y fuerza la conversión a 1 canal (L).
             Cada píxel unsigned char [0–255] se convierte a double.
  Salida   : Eigen::MatrixXd de dimensiones (alto × ancho).
  Excepción: std::runtime_error si el archivo no existe o no es legible.

Implementación:
  stbi_load(ruta.c_str(), &ancho, &alto, &canales, 1)
  → recorre la matriz píxel a píxel: matriz(i,j) = static_cast<double>(datos[i*ancho+j])
```

#### `std::vector<Eigen::MatrixXd> cargarImagenColor(const std::string& ruta)`

```
Contrato:
  Entrada  : ruta — archivo PNG/JPG en disco.
  Proceso  : stb_image fuerza 3 canales (RGB). Separa R, G, B en matrices
             independientes de tamaño (alto × ancho), valores [0.0, 255.0].
  Salida   : vector[3] de Eigen::MatrixXd  →  [0]=R, [1]=G, [2]=B

Orden de canales:
  idx = (i * ancho + j) * 3
  R ← datos[idx+0], G ← datos[idx+1], B ← datos[idx+2]
```

#### `std::pair<int,int> obtenerDimensiones(const std::string& ruta)`

```
Contrato:
  Entrada  : ruta — archivo PNG/JPG en disco.
  Proceso  : stbi_info() sin cargar píxeles a memoria.
  Salida   : {ancho, alto}
  Excepción: std::runtime_error si no se puede leer el encabezado.
```

---

## 2. Módulo `svd_procesador` — Descomposición SVD y reconstrucción

### 2.1 Ficheros

| Fichero | Tipo | Líneas |
|---------|------|--------|
| `src/svd_procesador.hpp` | Header (declaraciones + structs) | 46 |
| `src/svd_procesador.cpp` | Implementación | 34 |

### 2.2 Dependencias

- `Eigen/Dense`, `Eigen/SVD`

### 2.3 Estructuras de datos

#### `struct ResultadoSVD`

```cpp
struct ResultadoSVD {
    Eigen::MatrixXd U;       // Matriz izquierda    (m × r)
    Eigen::VectorXd sigma;   // Valores singulares  (r)  — ordenados σ₁≥σ₂≥…≥σᵣ
    Eigen::MatrixXd Vt;      // Matriz derecha T    (r × n)
    int rango;               // r = σ.size()        — rango efectivo
};
```

Relación matemática: `A = U · diag(sigma) · Vt`.

### 2.4 API

#### `ResultadoSVD calcularSVD(const Eigen::MatrixXd& A)`

```
Algoritmo : Eigen::BDCSVD con ComputeFullU | ComputeFullV
            (divide-and-conquer + bidiagonalización; recomendado para N > 100)

Precisión  : Single-precision call (JacobiSVD anidado en BDCSVD interno)
             Error de reconstrucción: ~1e-15 (doble precisión)

Contrato:
  - Calcula U completo (m×m), V completo (n×n), todos los valores singulares.
  - No requiere matriz simétrica ni cuadrada.
  - sigma está garantizado ordenado descendente por Eigen.
```

#### `Eigen::MatrixXd reconstruirRangoK(const ResultadoSVD& resultado, int k)`

```
Fórmula : A_k = U[:,:k] · diag(σ[:k]) · Vt[:k,:]

Garantías:
  - k se clampéa a min(k, resultado.rango) automáticamente.
  - No modifica el ResultadoSVD de entrada (paso por valor de sub-bloques).

Complejidad: O(k · m · n)  (k productos matriz-vector-matrix)
```

#### `std::vector<Eigen::MatrixXd> reconstruirRangoKColor(...)`

```
Aplica reconstruirRangoK() independientemente a cada uno de los 3 canales
SVD devueltos por calcularSVD(canal).

Entrada : vector<ResultadoSVD> de tamaño 3 (R, G, B)
Salida  : vector<Eigen::MatrixXd> de tamaño 3 (Rk, Gk, Bk)
```

---

## 3. Módulo `metricas` — Cálculo y exportación de métricas

### 3.1 Ficheros

| Fichero | Tipo | Líneas |
|---------|------|--------|
| `src/metricas.hpp` | Header (declaraciones + structs) | 49 |
| `src/metricas.cpp` | Implementación | 92 |

### 3.2 Dependencias

- `Eigen/Dense`, `<cmath>`, `<fstream>`, `<iomanip>`

### 3.3 Estructura de datos

#### `struct MetricasCompresion`

```cpp
struct MetricasCompresion {
    int    k;                 // Rango de la aproximación
    double error_relativo;    // ||A - A_k||_F / ||A||_F
    double energia_pct;       // % de energía capturada: Σᵢ₌₁ᵏ σᵢ² / Σᵢ₌₁ʳ σᵢ² × 100
    double tasa_compresion;   // (m·n) / (k·(m+n+1))
};
```

### 3.4 API — Funciones puras (sin efectos secundarios)

| Función | Fórmula | Complejidad |
|---------|---------|-------------|
| `normaFrobenius(A)` | `A.norm()` | O(m·n) |
| `calcularErrorRelativo(A, Ak)` | `||A−Ak||_F / ||A||_F` | O(m·n) |
| `calcularEnergia(sigma, k)` | `sum(σᵢ², i≤k) / sum(σᵢ², i≤r) × 100` | O(r) |
| `calcularTasaCompresion(m, n, k)` | `(m·n) / (k·(m+n+1))` | O(1) |

### 3.5 API — Funciones batch (con efectos secundarios)

| Función | Efecto |
|---------|--------|
| `calcularTodasLasMetricas(A, sigma, valores_k)` | Calcula energía + tasa para cada k; error se rellena posteriormente |
| `completarErrorRelativo(metricas, A, reconstrucciones)` | Asigna `error_relativo` a cada entrada del vector |
| `guardarMetricasCSV(ruta, metricas)` | Escribe `k,error_relativo,energia_pct,tasa_compresion` con 10 decimales |
| `guardarValoresSingularesCSV(ruta, sigma)` | Escribe `indice,valor_singular` (1-indexado) |

---

## 4. Módulo `exportador` — Guardado de PNG

### 4.1 Ficheros

| Fichero | Tipo | Líneas |
|---------|------|--------|
| `src/exportador.hpp` | Header (declaraciones) | 23 |
| `src/exportador.cpp` | Implementación | 48 |

### 4.2 Dependencias

- `stb_image_write.h` (macro `STB_IMAGE_WRITE_IMPLEMENTATION` definida **sólo** en `exportador.cpp` para evitar LNK2005)
- `Eigen/Dense`

### 4.3 Funciones auxiliares

#### `static unsigned char clamp255(double v)` (privado)

```
Redondeo al entero más cercano, clamp al rango [0, 255].
Long→char sin comprobación de rango: clampleo previamente.
```

### 4.4 API

#### `void guardarPNGGris(const std::string& ruta, const Eigen::MatrixXd& matriz)`

```
Entrada  : matriz double en [0.0, 255.0]
Proceso  : convierte a unsigned char, fila por fila
           stbi_write_png(ruta, ancho=cols, alto=rows, canales=1, ...)
Excepción: std::runtime_error si stbi_write_png falla
```

#### `void guardarPNGColor(const std::string& ruta, R, G, B)`

```
Entrada  : 3 matrices double [0.0, 255.0] de igual tamaño
Proceso  : intercala R,G,B en buffer RGBA→RGB row-major
           stbi_write_png(ruta, ancho, alto, canales=3, ...)
Excepción: std::runtime_error si stbi_write_png falla
```

---

## 5. Módulo `main` — Orquestador del flujo completo

### 5.1 Fichero

| Fichero | Tipo | Líneas |
|---------|------|--------|
| `src/main.cpp` | Punto de entrada | 176 |

### 5.2 Detección de raíz del proyecto

```cpp
static std::string detectarRaizProyecto();
```

```
Busca hacia arriba desde cwd (máx. 12 niveles) el archivo CMakeLists.txt.
Si no lo encuentra, usa GetModuleFileNameA() (Windows) para obtener la
carpeta del ejecutable y repite la búsqueda desde ahí.
Garantiza que rutas relativas como "assets/test_gris.png" funcionen
independientemente del directorio de trabajo.
```

### 5.3 Parseo de argumentos CLI

| Flag | Descripción |
|------|-------------|
| `--imagen <ruta>` | Ruta de la imagen a procesar |
| `--k <k1,k2,...>` | Lista de rangos separados por coma (ej: `5,20,50,100,200`) |
| `--modo gris\|color` | Modo: escala de grises o color RGB |

### 5.4 Flujo de 6 pasos

```
PASO 1 — Cargar imagen
         obtenerDimensiones() → cargarImagenGris() o cargarImagenColor()
         → vector<Eigen::MatrixXd> canales

PASO 2 — Calcular SVD
         Para cada canal → calcularSVD() → ResultadoSVD
         → vector<ResultadoSVD> svd_por_canal

PASO 3 — Aproximaciones de rango bajo
         Para cada k en valores_k:
            reconstruirRangoK() o reconstruirRangoKColor()
            guardarPNGGris() o guardarPNGColor() → ak_k.png
            calcularErrorRelativo() → actualizar métricas

PASO 5 — Exportar CSV
         guardarMetricasCSV()        → resultados/datos/metricas.csv
         guardarValoresSingularesCSV()→ resultados/datos/valores_singulares.csv

PASO 6 — Gráficos Python
         system("python analisis_cuantitativo.py") → resultados/graficos/
```

### 5.5 Gestión de errores

```
Bloque try/catch en todo el flujo.
Cualquier std::exception se captura y se imprime a stderr con código de salida 1.
```

---

## 6. Módulo `test_reconstruccion_exacta` — Test unitario

### 6.1 Fichero

| Fichero | Tipo | Líneas |
|---------|------|--------|
| `src/test_reconstruccion_exacta.cpp` | Test unitario | 77 |

### 6.2 Propósito

```
Valida el criterio de aceptación SPEC-000 §12 #3:
"Reconstrucción exacta (k = r) coincide con original (error < 1e-10)"
```

### 6.3 Metodología

```
1. Genera matriz sintética 512×512 con Eigen::MatrixXd::Random()
   → rango completo garantizado (r = 512)

2. Calcula SVD con Eigen::BDCSVD(ComputeFullU | ComputeFullV)

3. Reconstruye A_k con k = r (todos los valores singulares):
   A_k = U[:,:k] · diag(σ[:k]) · Vt[:k,:]

4. Error relativo: ||A - A_k||_F / ||A||_F

5. PASS si error < 1e-10, FAIL en caso contrario
```

### 6.4 Resultado verificado

```
[TEST] Reconstruccion exacta k=r: error = 4.751268e-15
[TEST] Resultado: PASS
```
Error medido está **~20 000× por debajo** del umbral `1e-10`.

---

## 7. Build system — CMakeLists.txt

### 7.1 Configuración general

```cmake
cmake_minimum_required(VERSION 3.16)
project(SvdImageCompression LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Toolchain vcpkg
find_package(Eigen3 REQUIRED)   # Eigen3::Eigen es el imported target
```

### 7.2 Targets

| Target | Fuentes | Enlace |
|--------|---------|--------|
| `svd_compresion` | `src/main.cpp`, `cargador_imagen.cpp`, `svd_procesador.cpp`, `metricas.cpp`, `exportador.cpp` | `Eigen3::Eigen` |
| `test_reconstruccion_exacta` | `src/test_reconstruccion_exacta.cpp` | `Eigen3::Eigen` |

### 7.3 Opciones de compilación

```
MSVC  : /Wall
GCC   : -Wall -Wextra -O2
Clang : -Wall -Wextra -O2
```

### 7.4 Inclusión de Eigen

```
target_link_libraries(... PRIVATE Eigen3::Eigen)
target_include_directories(... PRIVATE ${CMAKE_SOURCE_DIR}/include)
  → include/   contiene stb_image.h y stb_image_write.h
```

---

## 8. Dependencias y versiones

| Librería | Versionada | Origen | Tipo | Propósito |
|----------|-----------|--------|------|-----------|
| Eigen 3.4+ | eigen3:x64-windows 5.0.1 | vcpkg | Header-only + imported target | BDCSVD, álgebra lineal |
| stb_image | 2024-07-29 | vcpkg / manual | Header-only | Carga PNG/JPG |
| stb_image_write | 2024-07-29 | vcpkg / manual | Header-only | Guardado PNG |

---

## 9. Notas de diseño

- **Separación de responsabilidades**: cada módulo tiene una única razón de cambio.
  - `cargador_imagen`: formato de archivo de entrada.
  - `svd_procesador`: algoritmo matemático.
  - `metricas`: fórmulas de evaluación.
  - `exportador`: formato de archivo de salida.
  - `main`: orquestación y CLI.
- **STB_IMAGE_WRITE_IMPLEMENTATION** se define **exclusivamente** en `exportador.cpp`
  para evitar múltiples definiciones de símbolos (LNK2005).
- **Eigen3::Eigen** es el imported target; no se usa `Eigen3_INCLUDE_DIRS`
  directo — el target propaga las rutas de inclusión automáticamente.
- **`detectarRaizProyecto()`** hace que el binario sea agnóstico al directorio
  de trabajo: funciona desde `build/Release/`, desde la raíz o desde cualquier
  subdirectorio.
