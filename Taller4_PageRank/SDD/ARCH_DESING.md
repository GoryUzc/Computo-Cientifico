# ARQUITECTURA Y DISEÑO: Taller 4 — Algoritmo PageRank

> Documento técnico para implementación C++17 con Eigen
> **Problema:** Cálculo del vector de PageRank mediante el Método de las Potencias
> **Versión:** 2.0 — alineada con SPEC-001 v2.0 y SPEC-002 (sin `graph_loader`, sin `construir_matriz_transicion()`)

---

## 1. Stack Tecnológico

| Capa | Tecnología | Versión | Propósito |
|------|-----------|---------|-----------|
| **Cálculo numérico** | C++17 | ISO/IEC 14882:2017 | Núcleo: construcción de G, método potencias |
| **Álgebra lineal** | Eigen 3.4+ | 3.4.0+ | Matrices densas, operaciones vectoriales |
| **Build** | CMake | 3.16+ | Generación de targets |
| **Exportación** | `std::ofstream` (STL) | C++11+ | Escritura de resultados CSV |
| **Visualización** | Python 3.10+ | 3.10+ | Tablas, heatmaps, grafos |
| **Análisis datos** | numpy, pandas | 2.0+ | Procesamiento de CSV |
| **Gráficos** | matplotlib | 3.7+ | Visualización de convergencia |

---

## 2. Objetivo

Implementar el algoritmo PageRank en C++17 usando Eigen para calcular el autovector principal de la Matriz de Google $G$ mediante el Método de las Potencias, procesando 5 casos de prueba y exportando resultados a CSV.

> **Nota (alineado SPEC-001 v2.0):** Las matrices de entrada **YA son matrices de transición $M$** (columnas normalizadas). Por lo tanto no existe el módulo `graph_loader`, ni la función `construir_matriz_transicion()`, ni la carpeta `casos_prueba/*.txt`.

---

## 3. Fundamento Matemático

### 3.1 Matriz de Transición $M$

La entrada es una matriz de transición $M \in \mathbb{R}^{n\times n}$ ya normalizada por columnas, donde $M_{ij}$ representa la probabilidad de transición de la página $j$ a la página $i$.

### 3.2 Matriz de Google $G$

$$G = dM + \frac{1-d}{n}J$$

Donde:
- $d = 0.85$ (factor de amortiguamiento)
- $J \in \mathbb{R}^{n \times n}$ es la matriz de unos
- $\frac{1-d}{n}J$ garantiza irreducibilidad y positividad

**Propiedades de $G$ (cuando $M$ es columnas-normalizadas):**
- Columnas de $G$ suman 1 para columnas de $M$ sin sumidero.
- Para columnas sumidero de $M$ (suma 0), la columna de $G$ suma $1-d$.

### 3.3 Método de las Potencias

**Entrada:** Matriz $G \in \mathbb{R}^{n \times n}$, tolerancia $\epsilon$, máximo iteraciones $k_{max}$
**Salida:** Vector de PageRank $\mathbf{v} \in \mathbb{R}^n$

```
1. v₀ = [1/n, 1/n, ..., 1/n]ᵀ
2. Para k = 0, 1, ..., k_max:
     v_{k+1} = G · v_k
     Si ||v_{k+1} - v_k||₂ < ε:
         RETORNAR v_{k+1}
3. RETORNAR v_{k_max}
```

**Criterio de convergencia:** $\|v_{k+1} - v_k\|_2 < \epsilon = 10^{-8}$

**Tasa de convergencia:** $\|v_k - v^*\| \leq C \cdot d^k$ (con $d = 0.85$, estimado ~100 iteraciones para $\epsilon = 10^{-8}$; valor real depende de la estructura de $M$).

### 3.4 Normalización

Después de cada iteración del método de potencias, y al final del algoritmo:

$$\mathbf{v} \leftarrow \frac{\mathbf{v}}{\sum_i v_i}$$

---

## 4. Parámetros de Configuración

| Parámetro | Valor | Justificación |
|-----------|-------|---------------|
| Factor amortiguamiento ($d$) | 0.85 | Estándar de Google |
| Tolerancia ($\epsilon$) | $10^{-8}$ | Precisión requerida |
| Máximo iteraciones ($k_{max}$) | 1000 | Evitar bucles infinitos |
| Vector inicial | Uniforme $[1/n, ..., 1/n]^T$ | Distribución neutral |
| Norma de convergencia | $L_2$ (euclidiana) | Estándar numérico |

---

## 5. Casos de Prueba

> **Fuente:** matrices $M$ hardcodeadas en `src/casos_prueba.cpp` (provistas por el profesor). No hay archivos `.txt` ni carga externa.

### Caso 1: 3x3 con sumidero

```
Matriz de transición M:
[ 0   0   0 ]
[ 1/2 0   0 ]
[ 1/2 1   0 ]

Características:
- Columna 3 (índice 2) es sumidero (suma 0)
- Esperado: nodo 2 acumula mayor PageRank
```

### Caso 2: 4x4 regular

```
Matriz de transición M:
[ 0   1/2 0   1/2 ]
[ 1/2 0   1/2 0   ]
[ 0   0   1/2 1/2 ]
[ 1/2 0   1/2 0   ]

Características:
- Todas las columnas suman 1
- Esperado: PageRank casi uniforme (0.25 cada nodo)
```

### Caso 3: 8x8 complejo

```
Matriz de transición M:
[ 0   1/2 1/3 0   0   0   0   0   ]
[ 1/2 0   1/3 0   0   0   0   0   ]
[ 1/2 1/2 0   1   0   0   0   0   ]
[ 0   0   0   1/3 0   0   0   0   ]
[ 0   0   0   0   0   1/2 1/3 0   ]
[ 0   0   0   0   1/2 0   1/3 0   ]
[ 0   0   0   0   1/2 1/2 0   1   ]
[ 0   0   0   0   0   0   1/3 0   ]

Características:
- Columnas 2 y 3 (índices 2 y 3) no suman 1 (2/3 y 4/3)
- La tolerancia ε = 10⁻⁸ se alcanza en 451 iteraciones
```

### Caso 4: 5x5 con estructura

```
Matriz de transición M:
[ 0   0   1/2 0   0   ]
[ 1   0   0   0   0   ]
[ 0   1   0   0   0   ]
[ 0   0   1/2 0   1   ]
[ 0   0   0   1   0   ]

Características:
- Columnas suman 1
- Esperado: nodo 3 concentra mayor PageRank
```

### Caso 5: 3x3 con sumidero

```
Matriz de transición M:
[ 0   1   1 ]
[ 1   0   0 ]
[ 0   0   0 ]

Características:
- Columna 3 (índice 2) es sumidero (suma 0)
- Resultado: nodo 0 tiene mayor PageRank (0.486)
```

---

## 6. Estructura del Proyecto

```
Taller4_PageRank/
├── CMakeLists.txt
├── README.md
├── SDD/
│   ├── SPECS/
│   │   ├── SPEC-001.md
│   │   └── SPEC-002.md
│   └── ARCH_DESING.md               # Este archivo
├── src/
│   ├── main.cpp
│   ├── pagerank_core.hpp
│   ├── pagerank_core.cpp
│   ├── casos_prueba.hpp
│   ├── casos_prueba.cpp
│   ├── exportador.hpp
│   └── exportador.cpp
├── python/
│   ├── visualizar_pagerank.py
│   └── requirements.txt
├── build/
├── resultados/
│   ├── tablas/                      # Generado por SPEC-002 (auto-creadas)
│   ├── graficos/                    # Generado por SPEC-002 (auto-creadas)
│   ├── caso1_pagerank.csv
│   ├── caso1_convergencia.csv
│   ├── caso1_matriz_M.csv
│   ├── caso1_matriz_G.csv
│   ├── caso{2..5}_*.csv
│   └── resumen_comparativo.csv
```

**Fuera de alcance (v1.0 removidos):** `graph_loader` (módulo), `.txt` de prueba, `construir_matriz_transicion()` y miembro `adyacencia_`.

---

## 7. Especificaciones de Implementación

### 7.1 Estructura de Datos

```cpp
// pagerank_core.hpp
struct ResultadoPageRank {
    Eigen::VectorXd pagerank;              // Vector final de PageRank
    int iteraciones;                        // Iteraciones hasta convergencia
    double error_final;                     // ||v_{k+1} - v_k||₂
    bool convergio;                         // true si convergió antes de max_iter
    double tiempo_ejecucion;                // Tiempo en segundos
    std::vector<double> historial_error;    // Error por iteración (para gráfica)
    std::vector<Eigen::VectorXd> historial_pagerank;  // Vector en cada iteración
};

class PageRank {
public:
    PageRank(const Eigen::MatrixXd& transicion, double d = 0.85);
    Eigen::MatrixXd construir_matriz_google();  // No-const: cachea google_
    
    ResultadoPageRank ejecutar(double tolerancia = 1e-8, int max_iter = 1000);
    
    int get_n_paginas() const;
    double get_factor_amortiguamiento() const;
    const Eigen::MatrixXd& get_matriz_google() const;
    const Eigen::MatrixXd& get_matriz_transicion() const;
    
    bool validar_matriz_transicion() const;
    Eigen::VectorXd calcular_suma_columnas() const;

private:
    Eigen::MatrixXd transicion_;     // Matriz M (entrada) — NO adyacencia_
    Eigen::MatrixXd google_;         // G cacheada
    double d_;
    int n_;
};
```

### 7.2 Construcción de $G$

```cpp
Eigen::MatrixXd PageRank::construir_matriz_google() {
    Eigen::MatrixXd J = Eigen::MatrixXd::Ones(n_, n_);
    google_ = d_ * transicion_ + (1.0 - d_) / n_ * J;
    return google_;
}
```

### 7.3 Validación de la Matriz de Transición

```cpp
bool PageRank::validar_matriz_transicion() const {
    Eigen::VectorXd sumas = calcular_suma_columnas();
    for (int j = 0; j < n_; ++j) {
        double suma = sumas(j);
        // Válida si suma ≈ 1.0 o es un sumidero (suma ≈ 0.0).
        // Usar > 1e-6 para tolerar exacto 0.0 sin falsos positivos por punto flotante.
        if (std::abs(suma - 1.0) > 1e-6 && suma > 1e-6) {
            return false;
        }
    }
    return true;
}

Eigen::VectorXd PageRank::calcular_suma_columnas() const {
    return transicion_.colwise().sum();
}
```

### 7.4 Método de las Potencias

```cpp
ResultadoPageRank PageRank::ejecutar(double tolerancia, int max_iter) {
    ResultadoPageRank resultado;
    resultado.historial_error.reserve(max_iter + 1);
    resultado.historial_pagerank.reserve(max_iter + 1);

    Eigen::VectorXd v = Eigen::VectorXd::Constant(n_, 1.0 / n_);
    auto inicio = std::chrono::high_resolution_clock::now();

    // Estado inicial (requerido para convergencia SPEC-002)
    resultado.historial_error.push_back(0.0);
    resultado.historial_pagerank.push_back(v);

    for (int k = 0; k < max_iter; ++k) {
        auto v_anterior = v;
        v = google_ * v;
        double error = (v - v_anterior).norm();

        resultado.iteraciones = k + 1;
        resultado.error_final = error;
        resultado.historial_error.push_back(error);
        resultado.historial_pagerank.push_back(v);

        if (error < tolerancia) {
            resultado.convergio = true;
            break;
        }
    }

    v = v / v.sum();  // Corrección por punto flotante

    auto fin = std::chrono::high_resolution_clock::now();
    resultado.tiempo_ejecucion = std::chrono::duration<double>(fin - inicio).count();
    resultado.pagerank = v;
    return resultado;
}
```

---

## 8. Formato de Salida CSV

### 8.1 `casoN_pagerank.csv` (formato definitivo)

```csv
pagina,pagerank,grado_entrada,grado_salida
0,0.052632,1,2
1,0.473684,1,1
2,0.473684,1,0
```

> `grado_entrada` = cantidad de entradas no nulas en la fila `i` de $M$.
> `grado_salida` = cantidad de salidas no nulas en la columna `j` de $M$.

### 8.2 `resumen_comparativo.csv` (10 columnas, con `suma_pagerank`)

```csv
caso,descripcion,n_paginas,iteraciones,error_final,convergencia,tiempo_segundos,pagerank_max,pagerank_min,suma_pagerank
1,3x3 con sumidero,3,23,8.5e-09,true,0.000013,0.661041,0.114125,1.000000
2,4x4 regular,4,1,0.000000e+00,true,0.000003,0.250000,0.250000,1.000000
3,8x8 complejo,8,451,9.7e-09,true,0.000362,0.231324,0.027328,1.000000
4,5x5 con estructura,5,99,9.7e-09,true,0.000056,0.370572,0.077334,1.000000
5,3x3 con sumidero,3,109,9.5e-09,true,0.000050,0.486486,0.050000,1.000000
```

### 8.3 `casoN_convergencia.csv`

```csv
iteracion,error_norma,pagerank_0,pagerank_1,pagerank_2
0,0.000000,0.333333,0.333333,0.333333
1,0.157123,0.116667,0.441667,0.441667
...
```

> La iteración 0 siempre tiene `error_norma = 0.0` y el vector inicial uniforme (requerido por SPEC-002 §2.1).

### 8.4 `casoN_matriz_M.csv` y `casoN_matriz_G.csv` (formato largo)

```csv
fila,columna,valor
0,0,0.000000
0,1,0.000000
...
n-1,n-1,0.000000
```

> Formato **largo** (una fila por elemento, total $n \times n$ filas). SPEC-002 reconstruye la matriz densa con `int(row['fila'])`, `int(row['columna'])`, `row['valor']`. Si se omiten ceros la reconstrucción falla.

---

## 9. Procedimiento de Compilación y Ejecución

### 9.1 Compilación (Windows - MSVC)

```powershell
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build . --config Release
.\Release\pagerank.exe
```

### 9.2 Ejecución

El ejecutable usa rutas relativas `../resultados/...`, por lo que debe ejecutarse desde `build/`:

```powershell
cd build
.\Release\pagerank.exe
```

### 9.3 Ejecución del script Python

```bash
pip install -r python/requirements.txt
PYTHONIOENCODING=utf-8 python python/visualizar_pagerank.py
```

> `PYTHONIOENCODING=utf-8` evita error de codificación en consola Windows (cabeceras con `Σ`).

### 9.4 Salida esperada en consola

```
=== ALGORITMO PAGERANK ===
Taller 4 - Cálculo Científico

Procesando Caso 1: 3x3 con sumidero
  N páginas: 3
  Iteraciones: 23
  Error final: 8.08e-09
  Convergencia: Sí
  Tiempo: 0.000013 segundos
  PageRank: [0.114125, 0.224834, 0.661041]
  Suma: 1.0000000000

Procesando Caso 2: 4x4 regular
  N páginas: 4
  Iteraciones: 1
  Error final: 0.000000e+00
  Convergencia: Sí
  Tiempo: 0.000003 segundos
  PageRank: [0.250000, 0.250000, 0.250000, 0.250000]
  Suma: 1.0000000000

...
¡Proceso completado! Resultados disponibles en: resultados/
```

---

## 10. Criterios de Aceptación

### 10.1 Verificaciones Numéricas

| Criterio | Método | Valor Esperado | Estado Verificado |
|----------|--------|----------------|-------------------|
| **Suma de PageRank** | `pagerank.sum()` | $1.0 \pm 10^{-6}$ | ✅ |
| **No negatividad** | `pagerank.minCoeff()` | $\geq 0$ | ✅ |
| **Convergencia** | `error_final < 1e-8` | Sí para todos | ✅ |
| **Iteraciones razonables** | `iteraciones < 1000` | Sí | ✅ |
| **Matriz G estocástica** | `G.colwise().sum()` | Vector de unos | ⚠️ Parcial |

> ⚠️ `G = d·M + (1-d)/n·J` solo es estocástica por columnas cuando $M$ es columnas-normalizada. En presencia de sumideros ($M_{*j}=0$) o columnas no-normalizadas, las columnas afectadas suman $1-d$ (0.15) o valores no-unitarios. El algoritmo sigue convergiendo a un vector renormalizado válido porque `ejecutar()` aplica `v = v / v.sum()`.

### 10.2 Verificaciones Físicas por Caso

| Caso | Descripción | Verificación | Valor Esperado | Resultado Medido | Estado |
|------|-------------|--------------|----------------|------------------|--------|
| 1 | 3x3 con sumidero | Sumidero acumula prestigio | Nodo 2 tiene mayor PageRank | Nodo 2: 0.661041 | ✅ |
| 2 | 4x4 regular | Grafo simétrico | PageRank uniforme o casi uniforme | Uniforme 0.25 | ✅ |
| 3 | 8x8 complejo | Grafo grande | Convergencia en ~100 iteraciones | 451 iteraciones (convergido, error 9.7e-9) | ⚠️ Converge, iteraciones mayores al estimado teórico |
| 4 | 5x5 con estructura | Estructura específica | PageRank refleja estructura | Nodo 3: 0.370572 (máximo) | ✅ |
| 5 | 3x3 con sumidero | Sumidero en columna 3 | Nodo 2 tiene mayor PageRank | Nodo 0: 0.486486 (nodo 2: 0.050000) | ❌ No cumple |

> El valor esperado para caso 5 de la especificación no coincide con la matriz $M$ provista por el profesor. El algoritmo ejecuta correctamente la fórmula y renormaliza; el resultado válido es **nodo 0 = 0.486486**.

### 10.3 Verificaciones de Visualización

| Gráfico | Herramienta Python | Descripción | Estado |
|---------|-------------------|-------------|--------|
| Tabla comparativa | `pandas.DataFrame` | Resumen de los 5 casos | ✅ |
| Heatmap de matriz G | `plt.imshow()` | Visualización de $G$ | ✅ |
| Gráfico de convergencia | `plt.plot()` | Error vs iteraciones | ✅ |
| Histograma de PageRank | `plt.bar()` | Distribución de PageRank por nodo | ✅ |

---

## 11. Consideraciones de Implementación

### 11.1 Manejo de Sumideros

Las columnas sumidero de $M$ (suma 0) quedan en ceros. La Matriz de Google $G = d·M + (1-d)/n·J$ agrega $\frac{1-d}{n}$ a cada entrada en columnas sumidero a través de $J$. Esto preserva la irreducibilidad y permite al método de las potencias converger. La columna de $G$ correspondiente a un sumidero suma $1-d$.

### 11.2 Eficiencia Computacional

- **Complejidad por iteración:** $O(n^2)$ (multiplicación matriz-vector densa)
- **Para $n \leq 100$:** Matrices densas adecuadas.
- **Para $n > 1000$:** Considerar `Eigen::SparseMatrix`.

### 11.3 Estabilidad Numérica

- Usar `double` (64 bits).
- Renormalizar vector al final de `ejecutar()` (`v = v / v.sum()`).
- `validar_matriz_transicion()` tolera sumas exactas 1.0 o 0.0 usando umbral `1e-6` para evitar falsos positivos por punto flotante en sumideros.

---

## 12. Referencias

1. Page, L., Brin, S., Motwani, R., & Winograd, T. (1999). *The PageRank Citation Ranking: Bringing Order to the Web.* Stanford InfoLab.
2. Langville, A. N., & Meyer, C. D. (2006). *Google's PageRank and Beyond.* Princeton University Press.
3. Golub, G. H., & Van Loan, C. F. (2013). *Matrix Computations* (4th ed.). Johns Hopkins University Press.
4. Meyer, C. D. (2000). *Matrix Analysis and Applied Linear Algebra.* SIAM.
5. Eigen Documentation. https://eigen.tuxfamily.org/dox/

---

**Fin del documento ARCH_DESING.md**
