# SPEC-001: Implementación C++ del Algoritmo PageRank

> **Documento de Especificación Técnica**  
> **Proyecto:** Taller 4 — Algoritmo PageRank de Google  


---

## 1. Propósito

Especificar la implementación en C++17 del algoritmo PageRank usando la librería Eigen para álgebra lineal. El código debe:

1. **Recibir matrices de transición M** (ya normalizadas, proporcionadas por el profesor)
2. **Construir la Matriz de Google G** aplicando el factor de amortiguamiento $d = 0.85$
3. **Ejecutar el Método de las Potencias** hasta convergencia con tolerancia $\epsilon = 10^{-8}$
4. **Exportar resultados a CSV** para análisis en Python
5. **Procesar 5 casos de prueba** con diferentes estructuras de grafo

**Nota importante:** Las matrices de entrada **YA son matrices de transición M** (columnas normalizadas), NO matrices de adyacencia binarias. Por lo tanto, no necesitamos construir M desde una matriz de adyacencia.

---

## 2. Módulos a Implementar

### 2.1 Módulo: `pagerank_core.hpp` / `pagerank_core.cpp`

**Responsabilidad:** Gestionar la construcción de la Matriz de Google G y ejecución del método de potencias.

```cpp
// pagerank_core.hpp
#ifndef PAGERANK_CORE_HPP
#define PAGERANK_CORE_HPP

#include <Eigen/Dense>
#include <vector>
#include <string>

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
    // Constructor con matriz de transición M (ya normalizada) y factor de amortiguamiento
    PageRank(const Eigen::MatrixXd& transicion, double d = 0.85);
    
    // Construir matriz de Google G = d·M + (1-d)/n · J
    // No-const: cachea google_ para get_matriz_google().
    Eigen::MatrixXd construir_matriz_google();
    
    // Ejecutar método de potencias
    ResultadoPageRank ejecutar(double tolerancia = 1e-8, int max_iter = 1000);
    
    // Getters
    int get_n_paginas() const;
    double get_factor_amortiguamiento() const;
    const Eigen::MatrixXd& get_matriz_google() const;
    const Eigen::MatrixXd& get_matriz_transicion() const;
    
    // Validaciones
    bool validar_matriz_transicion() const;
    Eigen::VectorXd calcular_suma_columnas() const;

private:
    Eigen::MatrixXd transicion_;     // Matriz de transición M (entrada)
    Eigen::MatrixXd google_;         // Matriz de Google G (construida)
    double d_;                        // Factor de amortiguamiento
    int n_;                           // Número de páginas
};

#endif
```

**Especificaciones:**

- **Matriz de transición M:** $M_{ij}$ representa la probabilidad de transición de $j$ a $i$
- **Propiedad:** $\sum_i M_{ij} = 1$ para columnas sin sumideros, $0$ para sumideros
- **Matriz de Google:** $G = d \cdot M + \frac{1-d}{n} \cdot J$ donde $J$ es matriz de unos
- **Método de potencias:** $v_{k+1} = G \cdot v_k$ hasta $\|v_{k+1} - v_k\|_2 < \epsilon$

---

### 2.2 Módulo: `casos_prueba.hpp` / `casos_prueba.cpp`

**Responsabilidad:** Definir las 5 matrices de transición M proporcionadas por el profesor.

```cpp
// casos_prueba.hpp
#ifndef CASOS_PRUEBA_HPP
#define CASOS_PRUEBA_HPP

#include <Eigen/Dense>
#include <string>
#include <vector>

struct CasoPrueba {
    int id;
    std::string nombre;
    std::string descripcion;
    Eigen::MatrixXd transicion;  // Matriz M ya normalizada
};

class CasosPrueba {
public:
    // Obtener todos los casos de prueba
    static std::vector<CasoPrueba> obtener_todos();
    
    // Obtener un caso específico
    static CasoPrueba obtener_caso(int id);
    
    // Generar matrices de prueba (proporcionadas por el profesor)
    static Eigen::MatrixXd caso_1();  // 3x3 con sumidero
    static Eigen::MatrixXd caso_2();  // 4x4 regular
    static Eigen::MatrixXd caso_3();  // 8x8 complejo
    static Eigen::MatrixXd caso_4();  // 5x5 con estructura
    static Eigen::MatrixXd caso_5();  // 3x3 con sumidero
};

#endif
```

**Casos de prueba (matrices M proporcionadas por el profesor):**

```cpp
// casos_prueba.cpp
#include "casos_prueba.hpp"

Eigen::MatrixXd CasosPrueba::caso_1() {
    // 3x3 con sumidero (columna 3 es sumidero)
    Eigen::MatrixXd M(3, 3);
    M << 0.0, 0.0, 0.0,
         1.0/2.0, 0.0, 0.0,
         1.0/2.0, 1.0, 0.0;
    return M;
}

Eigen::MatrixXd CasosPrueba::caso_2() {
    // 4x4 regular (todas las columnas suman 1)
    Eigen::MatrixXd M(4, 4);
    M << 0.0, 1.0/2.0, 0.0, 1.0/2.0,
         1.0/2.0, 0.0, 1.0/2.0, 0.0,
         0.0, 0.0, 1.0/2.0, 1.0/2.0,
         1.0/2.0, 0.0, 1.0/2.0, 0.0;
    return M;
}

Eigen::MatrixXd CasosPrueba::caso_3() {
    // 8x8 complejo
    Eigen::MatrixXd M(8, 8);
    M << 0.0, 1.0/2.0, 1.0/3.0, 0.0, 0.0, 0.0, 0.0, 0.0,
         1.0/2.0, 0.0, 1.0/3.0, 0.0, 0.0, 0.0, 0.0, 0.0,
         1.0/2.0, 1.0/2.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
         0.0, 0.0, 0.0, 1.0/3.0, 0.0, 0.0, 0.0, 0.0,
         0.0, 0.0, 0.0, 0.0, 0.0, 1.0/2.0, 1.0/3.0, 0.0,
         0.0, 0.0, 0.0, 0.0, 1.0/2.0, 0.0, 1.0/3.0, 0.0,
         0.0, 0.0, 0.0, 0.0, 1.0/2.0, 1.0/2.0, 0.0, 1.0,
         0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0/3.0, 0.0;
    return M;
}

Eigen::MatrixXd CasosPrueba::caso_4() {
    // 5x5 con estructura
    Eigen::MatrixXd M(5, 5);
    M << 0.0, 0.0, 1.0/2.0, 0.0, 0.0,
         1.0, 0.0, 0.0, 0.0, 0.0,
         0.0, 1.0, 0.0, 0.0, 0.0,
         0.0, 0.0, 1.0/2.0, 0.0, 1.0,
         0.0, 0.0, 0.0, 1.0, 0.0;
    return M;
}

Eigen::MatrixXd CasosPrueba::caso_5() {
    // 3x3 con sumidero (columna 3 es sumidero)
    Eigen::MatrixXd M(3, 3);
    M << 0.0, 1.0, 1.0,
         1.0, 0.0, 0.0,
         0.0, 0.0, 0.0;
    return M;
}

std::vector<CasoPrueba> CasosPrueba::obtener_todos() {
    std::vector<CasoPrueba> casos;
    
    casos.push_back({1, "Caso 1", "3x3 con sumidero (columna 3)", caso_1()});
    casos.push_back({2, "Caso 2", "4x4 regular", caso_2()});
    casos.push_back({3, "Caso 3", "8x8 complejo", caso_3()});
    casos.push_back({4, "Caso 4", "5x5 con estructura", caso_4()});
    casos.push_back({5, "Caso 5", "3x3 con sumidero (columna 3)", caso_5()});
    
    return casos;
}

CasoPrueba CasosPrueba::obtener_caso(int id) {
    switch(id) {
        case 1: return {1, "Caso 1", "3x3 con sumidero", caso_1()};
        case 2: return {2, "Caso 2", "4x4 regular", caso_2()};
        case 3: return {3, "Caso 3", "8x8 complejo", caso_3()};
        case 4: return {4, "Caso 4", "5x5 con estructura", caso_4()};
        case 5: return {5, "Caso 5", "3x3 con sumidero", caso_5()};
        default: throw std::invalid_argument("Caso no válido");
    }
}
```

---

### 2.3 Módulo: `exportador.hpp` / `exportador.cpp`

**Responsabilidad:** Exportar resultados a archivos CSV.

```cpp
// exportador.hpp
#ifndef EXPORTADOR_HPP
#define EXPORTADOR_HPP

#include "pagerank_core.hpp"
#include <string>

class Exportador {
public:
    // Exportar vector de PageRank final con grados (grado_entrada/grado_salida
    // derivados del patrón no-cero de la matriz de transición M).
    static bool exportar_pagerank(const std::string& archivo,
                                   const ResultadoPageRank& resultado,
                                   const Eigen::MatrixXd& transicion);
    
    // Exportar historial de convergencia
    static bool exportar_convergencia(const std::string& archivo,
                                       const ResultadoPageRank& resultado);
    
    // Exportar matriz de Google G (para visualización)
    static bool exportar_matriz_google(const std::string& archivo,
                                        const Eigen::MatrixXd& G);
    
    // Exportar matriz de transición M (para referencia)
    static bool exportar_matriz_transicion(const std::string& archivo,
                                            const Eigen::MatrixXd& M);
    
    // Exportar resumen comparativo de todos los casos
    static bool exportar_resumen(const std::string& archivo,
                                  const std::vector<ResultadoPageRank>& resultados,
                                  const std::vector<std::string>& nombres_casos);
};

#endif
```

**Formato de salida CSV:**

**`caso1_pagerank.csv` (formato definitivo, alineado con SPEC-002):**
```csv
pagina,pagerank,grado_entrada,grado_salida
0,0.052632,1,2
1,0.473684,1,1
2,0.473684,1,0
```

**`caso1_convergencia.csv`:**
```csv
iteracion,error_norma,pagerank_0,pagerank_1,pagerank_2
0,0.000000,0.333333,0.333333,0.333333
1,0.157123,0.116667,0.441667,0.441667
2,0.025432,0.060833,0.469583,0.469583
...
```

**`resumen_comparativo.csv`:**
```csv
caso,descripcion,n_paginas,iteraciones,error_final,convergencia,tiempo_segundos,pagerank_max,pagerank_min,suma_pagerank
1,3x3 con sumidero,3,75,8.5e-9,true,0.0018,0.473684,0.052632,1.000000
2,4x4 regular,4,60,3.2e-9,true,0.0015,0.250000,0.250000,1.000000
3,8x8 complejo,8,120,9.8e-9,true,0.0025,0.166667,0.083333,1.000000
4,5x5 con estructura,5,80,5.1e-9,true,0.0008,0.333333,0.083333,1.000000
5,3x3 con sumidero,3,50,1.2e-9,true,0.0012,0.500000,0.000000,1.000000
```

---

### 2.4 Módulo: `main.cpp`

**Responsabilidad:** Orquestar el flujo completo de la simulación.

```cpp
// main.cpp
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include "pagerank_core.hpp"
#include "casos_prueba.hpp"
#include "exportador.hpp"

int main() {
    std::cout << "=== ALGORITMO PAGERANK ===" << std::endl;
    std::cout << "Taller 4 - Cálculo Científico" << std::endl;
    
    // Parámetros
    const double d = 0.85;
    const double tolerancia = 1e-8;
    const int max_iter = 1000;
    
    std::cout << "\nParámetros:" << std::endl;
    std::cout << "  Factor de amortiguamiento (d): " << d << std::endl;
    std::cout << "  Tolerancia (ε): " << tolerancia << std::endl;
    std::cout << "  Máximo iteraciones: " << max_iter << std::endl;
    
    // Obtener todos los casos de prueba
    auto casos = CasosPrueba::obtener_todos();
    std::vector<ResultadoPageRank> resultados;
    std::vector<std::string> nombres_casos;
    
    // Procesar cada caso
    for (const auto& caso : casos) {
        std::cout << "\n========================================" << std::endl;
        std::cout << "Procesando " << caso.nombre << ": " << caso.descripcion << std::endl;
        std::cout << "========================================" << std::endl;
        
        int n = caso.transicion.rows();
        std::cout << "  N páginas: " << n << std::endl;
        
        // Validar matriz de transición
        PageRank pr(caso.transicion, d);
        
        if (!pr.validar_matriz_transicion()) {
            std::cout << "  [WARNING] Matriz de transición tiene columnas que no suman 1" << std::endl;
            std::cout << "  Suma de columnas: " << pr.calcular_suma_columnas().transpose() << std::endl;
        }
        
        // Construir matriz de Google
        Eigen::MatrixXd G = pr.construir_matriz_google();
        std::cout << "  Matriz de Google G construida" << std::endl;
        
        // Ejecutar método de potencias
        ResultadoPageRank resultado = pr.ejecutar(tolerancia, max_iter);
        
        // Mostrar resultados
        std::cout << "\n  Resultados:" << std::endl;
        std::cout << "    Iteraciones: " << resultado.iteraciones << std::endl;
        std::cout << "    Error final: " << std::scientific << resultado.error_final << std::endl;
        std::cout << "    Convergencia: " << (resultado.convergencia ? "Sí" : "No") << std::endl;
        std::cout << "    Tiempo: " << std::fixed << std::setprecision(6) 
                  << resultado.tiempo_ejecucion << " segundos" << std::endl;
        
        std::cout << "    PageRank: [";
        for (int i = 0; i < n; ++i) {
            std::cout << std::fixed << std::setprecision(6) << resultado.pagerank(i);
            if (i < n-1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
        
        std::cout << "    Suma: " << std::fixed << std::setprecision(10) 
                  << resultado.pagerank.sum() << std::endl;
        
        // Exportar resultados
        std::string prefijo = "../resultados/caso" + std::to_string(caso.id);
        Exportador::exportar_pagerank(prefijo + "_pagerank.csv", resultado, caso.transicion);
        Exportador::exportar_convergencia(prefijo + "_convergencia.csv", resultado);
        Exportador::exportar_matriz_google(prefijo + "_matriz_G.csv", G);
        Exportador::exportar_matriz_transicion(prefijo + "_matriz_M.csv", caso.transicion);
        
        resultados.push_back(resultado);
        nombres_casos.push_back(caso.descripcion);
        
        std::cout << "  Archivos exportados a resultados/" << std::endl;
    }
    
    // Exportar resumen comparativo
    std::cout << "\n========================================" << std::endl;
    std::cout << "Exportando resumen comparativo..." << std::endl;
    Exportador::exportar_resumen("../resultados/resumen_comparativo.csv", 
                                  resultados, nombres_casos);
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "¡Proceso completado!" << std::endl;
    std::cout << "Resultados disponibles en: resultados/" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
```

---

## 3. Especificaciones Técnicas

### 3.1 Tipos de Datos

| Tipo | Eigen Type | Uso |
|------|-----------|-----|
| Matriz densa | `Eigen::MatrixXd` | Matrices de transición M, Google G |
| Vector denso | `Eigen::VectorXd` | Vector de PageRank |

### 3.2 Parámetros de Configuración

| Parámetro | Valor | Descripción |
|-----------|-------|-------------|
| `d` | 0.85 | Factor de amortiguamiento |
| `tolerancia` | 1e-8 | Criterio de convergencia |
| `max_iter` | 1000 | Máximo de iteraciones |
| Vector inicial | Uniforme $[1/n, ..., 1/n]^T$ | Distribución neutral |

### 3.3 Estructura de Archivos de Salida

```
resultados/
├── caso1_matriz_M.csv              # Matriz de transición M (caso 1)
├── caso1_matriz_G.csv              # Matriz de Google G (caso 1)
├── caso1_pagerank.csv              # PageRank final (caso 1)
├── caso1_convergencia.csv          # Historial de convergencia (caso 1)
├── caso2_matriz_M.csv
├── caso2_matriz_G.csv
├── caso2_pagerank.csv
├── caso2_convergencia.csv
├── caso3_matriz_M.csv
├── caso3_matriz_G.csv
├── caso3_pagerank.csv
├── caso3_convergencia.csv
├── caso4_matriz_M.csv
├── caso4_matriz_G.csv
├── caso4_pagerank.csv
├── caso4_convergencia.csv
├── caso5_matriz_M.csv
├── caso5_matriz_G.csv
├── caso5_pagerank.csv
├── caso5_convergencia.csv
└── resumen_comparativo.csv         # Tabla comparativa de los 5 casos
```

---

## 4. Algoritmos Detallados

### 4.1 Construcción de la Matriz de Google

```cpp
Eigen::MatrixXd PageRank::construir_matriz_google() {
    Eigen::MatrixXd J = Eigen::MatrixXd::Ones(n_, n_);
    
    // G = d·M + (1-d)/n · J
    google_ = d_ * transicion_ + (1.0 - d_) / n_ * J;
    
    return google_;
}
```

### 4.2 Método de las Potencias

```cpp
ResultadoPageRank PageRank::ejecutar(double tolerancia, int max_iter) {
    ResultadoPageRank resultado;
    resultado.historial_error.reserve(max_iter);
    resultado.historial_pagerank.reserve(max_iter);
    
    // 1. Vector inicial uniforme
    Eigen::VectorXd v = Eigen::VectorXd::Constant(n_, 1.0 / n_);
    Eigen::VectorXd v_anterior;
    
    auto inicio = std::chrono::high_resolution_clock::now();
    
    // Guardar estado inicial
    resultado.historial_error.push_back(0.0);
    resultado.historial_pagerank.push_back(v);
    
    // 2. Iterar método de potencias
    for (int k = 0; k < max_iter; ++k) {
        v_anterior = v;
        v = google_ * v;
        
        // 3. Verificar convergencia
        double error = (v - v_anterior).norm();
        resultado.historial_error.push_back(error);
        resultado.historial_pagerank.push_back(v);
        
        if (error < tolerancia) {
            resultado.convergencia = true;
            resultado.iteraciones = k + 1;
            resultado.error_final = error;
            break;
        }
        
        resultado.iteraciones = k + 1;
        resultado.error_final = error;
    }
    
    // 4. Normalizar (corrección por punto flotante)
    v = v / v.sum();
    
    auto fin = std::chrono::high_resolution_clock::now();
    resultado.tiempo_ejecucion = std::chrono::duration<double>(fin - inicio).count();
    resultado.pagerank = v;
    
    return resultado;
}
```

### 4.3 Validación de Matriz de Transición

```cpp
bool PageRank::validar_matriz_transicion() const {
    Eigen::VectorXd sumas = calcular_suma_columnas();
    
    for (int j = 0; j < n_; ++j) {
        double suma = sumas(j);
        // Permitir columnas de sumidero (suma = 0) o columnas válidas (suma = 1)
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

---

## 5. Criterios de Aceptación

### 5.1 Verificaciones Numéricas

| Criterio | Método | Valor Esperado |
|----------|--------|----------------|
| **Suma de PageRank** | `pagerank.sum()` | $1.0 \pm 10^{-6}$ |
| **No negatividad** | `pagerank.minCoeff()` | $\geq 0$ |
| **Convergencia** | `error_final < 1e-8` | Sí para todos los casos |
| **Iteraciones razonables** | `iteraciones < 1000` | Sí |
| **Matriz G estocástica** | `G.colwise().sum()` | Vector de unos |

### 5.2 Verificaciones por Caso

| Caso | Descripción | Verificación | Valor Esperado |
|------|-------------|--------------|----------------|
| 1 | 3x3 con sumidero | Sumidero acumula prestigio | Nodo 2 tiene mayor PageRank |
| 2 | 4x4 regular | Grafo simétrico | PageRank uniforme o casi uniforme |
| 3 | 8x8 complejo | Grafo grande | Convergencia en ~100 iteraciones |
| 4 | 5x5 con estructura | Estructura específica | PageRank refleja estructura |
| 5 | 3x3 con sumidero | Sumidero en columna 3 | Nodo 2 tiene mayor PageRank |

### 5.3 Verificaciones de Visualización

| Gráfico | Herramienta Python | Descripción |
|---------|-------------------|-------------|
| Tabla comparativa | `pandas.DataFrame` | Resumen de los 5 casos |
| Heatmap de matriz G | `plt.imshow()` | Visualización de G |
| Gráfico de convergencia | `plt.plot()` | Error vs iteraciones |
| Histograma de PageRank | `plt.bar()` | Distribución de PageRank por nodo |

---

## 6. Dependencias y Compilación

### 6.1 CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(PageRank LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg toolchain
set(VCPKG_ROOT "$ENV{VCPKG_ROOT}" CACHE PATH "vcpkg root directory")
if(EXISTS "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# Eigen3
find_package(Eigen3 REQUIRED)

# Ejecutable
add_executable(pagerank
    src/main.cpp
    src/pagerank_core.cpp
    src/casos_prueba.cpp
    src/exportador.cpp
)

target_link_libraries(pagerank PRIVATE Eigen3::Eigen)
target_include_directories(pagerank PRIVATE src)

# Optimizaciones
target_compile_options(pagerank PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/Wall>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -O2>
)
```

### 6.2 Compilación

**Windows (MSVC):**
```powershell
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
.\Release\pagerank.exe
```

**Linux/macOS (GCC/Clang):**
```bash
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
./pagerank
```

---

## 7. Prompt Maestro para Kilo IA

```markdown
# Tarea: Implementar Algoritmo PageRank en C++17 con Eigen

## Contexto
Estoy desarrollando el Taller 4 de Cálculo Científico. Necesito implementar 
el algoritmo PageRank de Google para calcular la importancia de páginas web 
en un grafo dirigido.

## Especificación
Lee el archivo SDD/SPECS/spec-001.md para los detalles completos.

## Arquitectura
- pagerank_core.hpp/cpp: Clase PageRank (construcción de G, método potencias)
- casos_prueba.hpp/cpp: 5 matrices de transición M proporcionadas por el profesor
- exportador.hpp/cpp: Escritura de CSV
- main.cpp: Orquestador (5 casos de prueba)

## IMPORTANTE
Las matrices de entrada YA son matrices de transición M (columnas normalizadas),
NO matrices de adyacencia binarias. No necesitamos construir M desde adyacencia.

## Parámetros
- Factor de amortiguamiento d = 0.85
- Tolerancia ε = 10⁻⁸
- Máximo iteraciones = 1000
- Vector inicial = uniforme [1/n, ..., 1/n]ᵀ

## Casos de Prueba (del profesor)
1. 3x3 con sumidero (columna 3)
2. 4x4 regular
3. 8x8 complejo
4. 5x5 con estructura
5. 3x3 con sumidero (columna 3)

## Requisitos
1. Usar Eigen 3.4+ para álgebra lineal
2. Usar C++17
3. Separar en módulos con responsabilidades claras
4. Exportar resultados a CSV para cada caso
5. Generar resumen comparativo
6. Verificar que la suma de PageRank sea 1.0
7. Validar que las columnas de M sumen 1 (excepto sumideros)

## Primero
Muéstrame el código de pagerank_core.hpp y pagerank_core.cpp
```

---

## 8. Checklist de Implementación

- [x] Crear estructura de carpetas (`src/`, `resultados/`, `python/`)
- [x] Implementar `pagerank_core.hpp/cpp` con construcción de G y método de potencias
- [x] Implementar `casos_prueba.hpp/cpp` con las 5 matrices M del profesor
- [x] Implementar `exportador.hpp/cpp` con escritura CSV
- [x] Implementar `main.cpp` con flujo completo
- [x] Configurar `CMakeLists.txt` con Eigen
- [x] Compilar y ejecutar
- [x] Verificar que los CSV se generen correctamente
- [x] Implementar script Python de visualización
- [x] Generar tabla comparativa de los 5 casos
- [x] Validar resultados (suma = 1, convergencia, sentido físico)

---

## 9. Cambios respecto a versión anterior

| Aspecto | Versión 1.0 ❌ | Versión 2.0 ✅ |
|---------|----------------|----------------|
| **Entrada** | Matriz de adyacencia binaria | Matriz de transición M (ya normalizada) |
| **Método** | `construir_matriz_transicion()` | Eliminado (no necesario) |
| **Variable** | `adyacencia_` | `transicion_` |
| **Caso 5** | 6x6 completo | 3x3 con sumidero |
| **Validación** | Verificar binaria | Verificar suma de columnas = 1 |
| **Casos** | Hardcoded genéricos | Exactos del profesor |

---

**Fin del documento SPEC-001.md**

---