# SPEC-001: Implementación C++ del Solver de Poisson 2D

> **Documento de Especificación Técnica**  
> **Proyecto:** Taller 3 — Simulación de Distribución de Calor 2D  

---

## 1. Propósito

Especificar la implementación en C++17 de un solver numérico para la **Ecuación de Poisson 2D** usando el **Método de Diferencias Finitas (MDF)** con condiciones de frontera de Dirichlet. El código debe construir el sistema lineal $Au = b$, resolverlo con un solver iterativo de Eigen, y exportar los resultados a CSV para visualización en Python.

---

## 2. Módulos a Implementar

### 2.1 Módulo: `malla.hpp` / `malla.cpp`

**Responsabilidad:** Gestionar la discretización del dominio y el mapeo entre índices 2D y 1D.

```cpp
// malla.hpp
#ifndef MALLA_HPP
#define MALLA_HPP

class Malla {
public:
    Malla(int n_interno, double longitud);

    // Getters
    int get_n() const;                    // Nodos internos por lado
    int get_N() const;                    // Total de incógnitas (n*n)
    double get_h() const;                 // Espaciado de malla
    double get_longitud() const;          // Longitud del dominio

    // Mapeo de índices
    int indice_2d_a_1d(int i, int j) const;  // (i,j) → k
    void indice_1d_a_2d(int k, int& i, int& j) const;  // k → (i,j)

    // Coordenadas físicas
    double coordenada_x(int i) const;     // x_i = i * h
    double coordenada_y(int j) const;     // y_j = j * h

private:
    int n_;           // Nodos internos por lado
    int N_;           // Total de incógnitas
    double h_;        // Espaciado
    double L_;        // Longitud del dominio
};

#endif
```

**Especificaciones:**

- `n_` = 20 (nodos internos por lado)
- `L_` = 1.0 (longitud del dominio $[0,1] \times [0,1]$)
- `h_` = L / (n + 1) = 1/21 ≈ 0.047619
- `N_` = n * n = 400
- Índices internos: $i, j \in [1, n]$
- Mapeo: $k = (i-1) \cdot n + (j-1)$ donde $k \in [0, N-1]$

---

### 2.2 Módulo: `ensamblador.hpp` / `ensamblador.cpp`

**Responsabilidad:** Construir la matriz dispersa $A$ y el vector $b$ del sistema lineal.

```cpp
// ensamblador.hpp
#ifndef ENSAMBLADOR_HPP
#define ENSAMBLADOR_HPP

#include <Eigen/Sparse>
#include <Eigen/Dense>
#include "malla.hpp"

struct CondicionesFrontera {
    double T_superior;   // y = L
    double T_inferior;   // y = 0
    double T_izquierda;  // x = 0
    double T_derecha;    // x = L
};

struct FuenteCalor {
    bool activa;
    double intensidad;   // Q
    int nodo_i;          // Posición x del nodo fuente
    int nodo_j;          // Posición y del nodo fuente
};

class Ensamblador {
public:
    Ensamblador(const Malla& malla, 
                const CondicionesFrontera& fronteras,
                const FuenteCalor& fuente);

    // Construcción del sistema
    Eigen::SparseMatrix<double> construir_matriz_A();
    Eigen::VectorXd construir_vector_b();

    // Exportación de patrón de matriz
    void exportar_patron_matriz(const std::string& archivo,
                                const Eigen::SparseMatrix<double>& A);

private:
    const Malla& malla_;
    CondicionesFrontera fronteras_;
    FuenteCalor fuente_;

    // Métodos auxiliares
    void agregar_contribucion_frontera(int k, int i, int j, Eigen::VectorXd& b);
    void agregar_contribucion_fuente(int k, int i, int j, Eigen::VectorXd& b);
};

#endif
```

**Especificaciones de construcción:**

**Matriz A (400 × 400):**

```cpp
// Para cada nodo interno (i, j) con índice k:
A.coeffRef(k, k) = 4.0;           // Diagonal principal

// Vecino izquierdo (i-1, j)
if (i > 1) A.coeffRef(k, k-1) = -1.0;

// Vecino derecho (i+1, j)
if (i < n) A.coeffRef(k, k+1) = -1.0;

// Vecino inferior (i, j-1)
if (j > 1) A.coeffRef(k, k-n) = -1.0;

// Vecino superior (i, j+1)
if (j < n) A.coeffRef(k, k+n) = -1.0;
```

**Vector b (400 × 1):**

```cpp
// Inicializar en cero
b = Eigen::VectorXd::Zero(N);

// Para cada nodo (i, j):
// 1. Contribución de fronteras
if (i == 1)  b[k] += T_izquierda;    // 50°C
if (i == n)  b[k] += T_derecha;      // 50°C
if (j == 1)  b[k] += T_inferior;     // 0°C
if (j == n)  b[k] += T_superior;     // 100°C

// 2. Contribución de fuente de calor
if (fuente.activa && i == fuente.nodo_i && j == fuente.nodo_j) {
    b[k] += h * h * fuente.intensidad;
}
```

**Parámetros por defecto:**

- `T_superior` = 100.0°C
- `T_inferior` = 0.0°C
- `T_izquierda` = 50.0°C
- `T_derecha` = 50.0°C
- Fuente: nodo central $(i_c, j_c) = (10, 10)$ o $(11, 11)$ para $n=20$
- Intensidad fuente: $Q = 1000.0$ (ajustable)

---

### 2.3 Módulo: `solver.hpp` / `solver.cpp`

**Responsabilidad:** Resolver el sistema lineal $Au = b$ usando solvers iterativos de Eigen.

```cpp
// solver.hpp
#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <Eigen/IterativeLinearSolvers>

class Solver {
public:
    Solver(double tolerancia = 1e-10, int max_iter = 1000);

    // Resolver sistema
    Eigen::VectorXd resolver(const Eigen::SparseMatrix<double>& A,
                             const Eigen::VectorXd& b);

    // Verificación
    double calcular_residuo(const Eigen::SparseMatrix<double>& A,
                           const Eigen::VectorXd& u,
                           const Eigen::VectorXd& b);

    // Getters
    int get_iteraciones() const;
    double get_error_final() const;
    bool convergio() const;

private:
    double tolerancia_;
    int max_iter_;
    int iteraciones_;
    double error_final_;
    bool convergencia_;
};

#endif
```

**Especificaciones del solver:**

- **Tipo:** `Eigen::BiCGSTAB<Eigen::SparseMatrix<double>>`
- **Precondicionador:** `Eigen::IncompleteLUT<double>`
- **Tolerancia:** $10^{-10}$
- **Máximo iteraciones:** 1000
- **Verificación:** $\|Au - b\|_2 < \text{tolerancia}$

**Flujo de resolución:**

```cpp
Eigen::BiCGSTAB<Eigen::SparseMatrix<double>> solver;
solver.preconditioner(Eigen::IncompleteLUT<double>());
solver.setTolerance(tolerancia_);
solver.setMaxIterations(max_iter_);

solver.compute(A);
Eigen::VectorXd u = solver.solve(b);

iteraciones_ = solver.iterations();
error_final_ = solver.error();
convergencia_ = (solver.info() == Eigen::Success);
```

---

### 2.4 Módulo: `exportador.hpp` / `exportador.cpp`

**Responsabilidad:** Exportar resultados a archivos CSV para visualización en Python.

```cpp
// exportador.hpp
#ifndef EXPORTADOR_HPP
#define EXPORTADOR_HPP

#include <Eigen/Dense>
#include <string>
#include "malla.hpp"

class Exportador {
public:
    // Exportar solución de temperatura
    static bool exportar_temperatura(const std::string& archivo,
                                     const Malla& malla,
                                     const Eigen::VectorXd& u);

    // Exportar matriz completa (con fronteras) para visualización
    static bool exportar_temperatura_completa(const std::string& archivo,
                                              const Malla& malla,
                                              const Eigen::VectorXd& u,
                                              double T_sup, double T_inf,
                                              double T_izq, double T_der);
};

#endif
```

**Formato de salida CSV:**

**`temperatura.csv`** (solo nodos internos):

```csv
x,y,temperatura
0.047619,0.047619,25.123
0.095238,0.047619,26.456
...
```

**`temperatura_completa.csv`** (incluye fronteras):

```csv
x,y,temperatura,tipo
0.0,0.0,50.0,frontera
0.047619,0.0,0.0,frontera
...
0.047619,0.047619,25.123,interno
...
```

---

### 2.5 Módulo: `main.cpp`

**Responsabilidad:** Orquestar el flujo completo de la simulación.

```cpp
// main.cpp
#include <iostream>
#include "malla.hpp"
#include "ensamblador.hpp"
#include "solver.hpp"
#include "exportador.hpp"

int main() {
    std::cout << "=== SIMULACIÓN DE DISTRIBUCIÓN DE CALOR 2D ===" << std::endl;

    // 1. Configurar malla
    int n = 20;
    double L = 1.0;
    Malla malla(n, L);

    std::cout << "Dominio: [0, " << L << "] × [0, " << L << "]" << std::endl;
    std::cout << "Nodos internos: " << n << " × " << n << " = " << n*n << std::endl;
    std::cout << "Espaciado h: " << malla.get_h() << std::endl;

    // 2. Configurar condiciones de frontera
    CondicionesFrontera fronteras;
    fronteras.T_superior = 100.0;
    fronteras.T_inferior = 0.0;
    fronteras.T_izquierda = 50.0;
    fronteras.T_derecha = 50.0;

    // 3. Configurar fuente de calor
    FuenteCalor fuente;
    fuente.activa = true;
    fuente.intensidad = 1000.0;
    fuente.nodo_i = (n + 1) / 2;  // = 10 para n=20
    fuente.nodo_j = (n + 1) / 2;  // = 10 para n=20

    // 4. Ensamblar sistema
    std::cout << "\nConstruyendo sistema..." << std::endl;
    Ensamblador ensamblador(malla, fronteras, fuente);
    auto A = ensamblador.construir_matriz_A();
    auto b = ensamblador.construir_vector_b();

    std::cout << "  Matriz A: " << A.rows() << " × " << A.cols() 
              << " (no nulos: " << A.nonZeros() << ")" << std::endl;
    std::cout << "  Vector b: " << b.size() << " × 1" << std::endl;

    // 5. Resolver sistema
    std::cout << "\nResolviendo con BiCGSTAB..." << std::endl;
    Solver solver(1e-10, 1000);
    auto u = solver.resolver(A, b);

    std::cout << "  Iteraciones: " << solver.get_iteraciones() << std::endl;
    std::cout << "  Error final: " << solver.get_error_final() << std::endl;
    std::cout << "  Convergencia: " << (solver.convergencia() ? "Sí" : "No") << std::endl;

    // 6. Verificar residuo
    double residuo = solver.calcular_residuo(A, u, b);
    std::cout << "  Residuo ||Au - b||: " << residuo << std::endl;

    // 7. Exportar resultados
    std::cout << "\nExportando resultados..." << std::endl;
    Exportador::exportar_temperatura("resultados/temperatura.csv", malla, u);
    Exportador::exportar_temperatura_completa("resultados/temperatura_completa.csv", 
                                               malla, u, 
                                               fronteras.T_superior, 
                                               fronteras.T_inferior,
                                               fronteras.T_izquierda, 
                                               fronteras.T_derecha);

    // 8. Exportar patrón de matriz
    ensamblador.exportar_patron_matriz("resultados/patron_matriz_A.csv", A);

    std::cout << "\nSimulación completada con éxito." << std::endl;

    // 9. Ejecutar segundo caso: sin fuente de calor
    std::cout << "\n=== CASO 2: SIN FUENTE DE CALOR ===" << std::endl;
    fuente.activa = false;
    Ensamblador ensamblador2(malla, fronteras, fuente);
    auto A2 = ensamblador2.construir_matriz_A();
    auto b2 = ensamblador2.construir_vector_b();

    Solver solver2(1e-10, 1000);
    auto u2 = solver2.resolver(A2, b2);

    Exportador::exportar_temperatura("resultados/temperatura_sin_fuente.csv", malla, u2);
    Exportador::exportar_temperatura_completa("resultados/temperatura_sin_fuente_completa.csv",
                                               malla, u2,
                                               fronteras.T_superior,
                                               fronteras.T_inferior,
                                               fronteras.T_izquierda,
                                               fronteras.T_derecha);

    std::cout << "Resultados sin fuente exportados." << std::endl;

    return 0;
}
```

---

## 3. Especificaciones Técnicas

### 3.1 Tipos de Datos

| Tipo | Eigen Type | Uso |
|------|-----------|-----|
| Matriz dispersa | `Eigen::SparseMatrix<double>` | Matriz A del sistema |
| Vector denso | `Eigen::VectorXd` | Vector b, solución u |
| Solver | `Eigen::BiCGSTAB<SparseMatrix<double>>` | Solver iterativo |
| Precondicionador | `Eigen::IncompleteLUT<double>` | Acelerar convergencia |

### 3.2 Parámetros de Configuración

| Parámetro | Valor | Descripción |
|-----------|-------|-------------|
| `n` | 20 | Nodos internos por lado |
| `L` | 1.0 | Longitud del dominio |
| `h` | 1/21 ≈ 0.0476 | Espaciado de malla |
| `N` | 400 | Total de incógnitas |
| `T_superior` | 100.0°C | Temperatura borde superior |
| `T_inferior` | 0.0°C | Temperatura borde inferior |
| `T_izquierda` | 50.0°C | Temperatura borde izquierdo |
| `T_derecha` | 50.0°C | Temperatura borde derecho |
| `Q` | 1000.0 | Intensidad fuente de calor |
| `tolerancia` | 1e-10 | Tolerancia del solver |
| `max_iter` | 1000 | Máximo iteraciones |

### 3.3 Estructura de Archivos de Salida

```
resultados/
├── temperatura.csv                    # Solución con fuente (nodos internos)
├── temperatura_completa.csv           # Solución con fuente (incluye fronteras)
├── temperatura_sin_fuente.csv         # Solución sin fuente (nodos internos)
├── temperatura_sin_fuente_completa.csv # Solución sin fuente (incluye fronteras)
└── patron_matriz_A.csv                # Elementos no nulos de A
```

---

## 4. Criterios de Aceptación

### 4.1 Verificaciones Numéricas

| Criterio | Método | Valor Esperado |
|----------|--------|----------------|
| Dimensiones de A | `A.rows()`, `A.cols()` | 400 × 400 |
| Dimensión de b | `b.size()` | 400 |
| Simetría de A | `(A - A.transpose()).norm()` | < 1e-14 |
| Elementos no nulos | `A.nonZeros()` | ≈ 1960 |
| Diagonal dominante | `A.diagonal().minCoeff()` | 4.0 |
| Residuo final | `||Au - b||` | < 1e-10 |
| Rango temperaturas | `u.minCoeff()`, `u.maxCoeff()` | [0, 150] (con fuente) |

### 4.2 Verificaciones Físicas

| Criterio | Descripción |
|----------|-------------|
| **Simetría izquierda-derecha** | $u(x, y) = u(1-x, y)$ dado que $T_{izq} = T_{der} = 50$ |
| **Gradiente vertical** | Temperatura aumenta de $y=0$ (0°C) a $y=1$ (100°C) |
| **Efecto de fuente** | Pico de temperatura visible en $(0.5, 0.5)$ |
| **Conservación** | Flujo neto de calor en fronteras = calor generado por fuente |

### 4.3 Verificaciones de Visualización

| Gráfico | Herramienta Python | Descripción |
|---------|-------------------|-------------|
| Patrón de matriz A | `plt.spy()` | Mostrar 5 diagonales |
| Heatmap con isotermas | `plt.imshow()` + `plt.contour()` | Distribución de temperatura |
| Comparación con/sin fuente | Subplots lado a lado | Efecto de la fuente puntual |
| Perfil transversal | `plt.plot()` | $u(x, 0.5)$ mostrando pico |

---

## 5. Dependencias y Compilación

### 5.1 Dependencias

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(Calor2D LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Eigen3 (header-only)
find_package(Eigen3 3.4 REQUIRED)

# Ejecutable
add_executable(calor_2d
    src/main.cpp
    src/malla.cpp
    src/ensamblador.cpp
    src/solver.cpp
    src/exportador.cpp
)

```

### 5.2 Compilación

**Windows (MSVC):**

```powershell
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
.\Release\calor_2d.exe
```

---


## 7. Checklist de Implementación

- [x] Crear estructura de carpetas (`src/`, `resultados/`, `python/`)
- [x] Implementar `malla.hpp/cpp` con mapeo de índices
- [x] Implementar `ensamblador.hpp/cpp` con construcción de A y b
- [x] Implementar `solver.hpp/cpp` con BiCGSTAB de Eigen
- [x] Implementar `exportador.hpp/cpp` con escritura CSV
- [x] Implementar `main.cpp` con flujo completo
- [x] Configurar `CMakeLists.txt` con Eigen
- [x] Compilar y ejecutar
- [x] Verificar que los CSV se generen correctamente
- [x] Implementar script Python de visualización
- [x] Generar las 4 gráficas requeridas

---

**Fin del documento SPEC-001.md**
