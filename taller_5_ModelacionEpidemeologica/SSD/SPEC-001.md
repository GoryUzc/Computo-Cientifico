# SPEC-001: Implementación C++ del Modelo SEIR con RK4

> **Documento de Especificación Técnica**  
> **Proyecto:** Taller 5 — Modelación Epidemiológica SEIR  


---

## 1. Propósito

Especificar la implementación en C++17 con Eigen de un solver numérico para el modelo epidemiológico SEIR utilizando el **Método de Runge-Kutta de 4to orden (RK4)** implementado manualmente. El solver debe manejar **dos escenarios epidemiológicos**, incluyendo uno con **β variable en el tiempo** (intervención de confinamiento en t=30).


## 2. Módulos a Implementar

### 2.1 Módulo: `modelo_seir.hpp` / `modelo_seir.cpp`

**Responsabilidad:** Definir la función $\mathbf{f}(t, \mathbf{y})$ del sistema SEIR y manejar β variable.

```cpp
// modelo_seir.hpp
#ifndef MODELO_SEIR_HPP
#define MODELO_SEIR_HPP

#include <Eigen/Dense>

class ModeloSEIR {
public:
    ModeloSEIR(double N, double sigma, double gamma);
    
    // Función f(t, y) del sistema SEIR
    // y = [S, E, I, R]
    Eigen::Vector4d evaluar(double t, const Eigen::Vector4d& y, 
                            int escenario) const;
    
    // Obtener β según escenario y tiempo
    double obtener_beta(double t, int escenario) const;
    
    // Calcular R₀ teórico
    double calcular_R0(int escenario) const;
    
    // Getters
    double get_N() const { return N_; }
    double get_sigma() const { return sigma_; }
    double get_gamma() const { return gamma_; }

private:
    double N_;       // Población total
    double sigma_;   // Tasa de progresión E → I (1/5 días)
    double gamma_;   // Tasa de recuperación I → R (1/10 días)
};

#endif
```

**Implementación:**

```cpp
// modelo_seir.cpp
#include "modelo_seir.hpp"

ModeloSEIR::ModeloSEIR(double N, double sigma, double gamma)
    : N_(N), sigma_(sigma), gamma_(gamma) {}

Eigen::Vector4d ModeloSEIR::evaluar(double t, const Eigen::Vector4d& y, 
                                     int escenario) const {
    double beta = obtener_beta(t, escenario);
    double S = y(0), E = y(1), I = y(2), R = y(3);
    
    double contagio = beta * S * I / N_;
    
    Eigen::Vector4d dydt;
    dydt(0) = -contagio;              // dS/dt
    dydt(1) = contagio - sigma_ * E;  // dE/dt
    dydt(2) = sigma_ * E - gamma_ * I;// dI/dt
    dydt(3) = gamma_ * I;             // dR/dt
    
    return dydt;
}

double ModeloSEIR::obtener_beta(double t, int escenario) const {
    if (escenario == 1) {
        return 0.6;  // Escenario A: constante
    } else {
        return (t < 30.0) ? 0.6 : 0.25;  // Escenario B: intervención
    }
}

double ModeloSEIR::calcular_R0(int escenario) const {
    // Para escenario B, retornamos el R₀ inicial (β=0.6)
    return 0.6 / gamma_;
}
```

---

### 2.2 Módulo: `solver_rk4.hpp` / `solver_rk4.cpp`

**Responsabilidad:** Implementar el método de Runge-Kutta 4to orden.

```cpp
// solver_rk4.hpp
#ifndef SOLVER_RK4_HPP
#define SOLVER_RK4_HPP

#include "modelo_seir.hpp"
#include <vector>

struct EstadoSEIR {
    double tiempo;
    Eigen::Vector4d y;       // [S, E, I, R]
    double conservacion;     // S + E + I + R - N
};

class SolverRK4 {
public:
    SolverRK4(const ModeloSEIR& modelo, double h, double t_final);
    
    // Resolver sistema para un escenario específico
    std::vector<EstadoSEIR> resolver(int escenario, const Eigen::Vector4d& y0);
    
    // Métricas
    double calcular_pico_infectados(const std::vector<EstadoSEIR>& historia) const;
    double calcular_dia_pico(const std::vector<EstadoSEIR>& historia) const;
    double calcular_recuperados_finales(const std::vector<EstadoSEIR>& historia) const;
    double calcular_error_max_conservacion(const std::vector<EstadoSEIR>& historia) const;

private:
    const ModeloSEIR& modelo_;
    double h_;
    double t_final_;
    
    // Paso individual de RK4
    Eigen::Vector4d paso_rk4(double t, const Eigen::Vector4d& y, int escenario);
};

#endif
```

**Implementación del paso RK4:**

```cpp
// solver_rk4.cpp
Eigen::Vector4d SolverRK4::paso_rk4(double t, const Eigen::Vector4d& y, 
                                     int escenario) {
    // Las 4 etapas evalúan f en tiempos diferentes
    // Cada evaluación obtendrá el β correcto según t
    Eigen::Vector4d k1 = modelo_.evaluar(t, y, escenario);
    Eigen::Vector4d k2 = modelo_.evaluar(t + h_/2.0, y + (h_/2.0) * k1, escenario);
    Eigen::Vector4d k3 = modelo_.evaluar(t + h_/2.0, y + (h_/2.0) * k2, escenario);
    Eigen::Vector4d k4 = modelo_.evaluar(t + h_, y + h_ * k3, escenario);
    
    return y + (h_ / 6.0) * (k1 + 2.0*k2 + 2.0*k3 + k4);
}

std::vector<EstadoSEIR> SolverRK4::resolver(int escenario, 
                                             const Eigen::Vector4d& y0) {
    std::vector<EstadoSEIR> historia;
    historia.reserve(static_cast<size_t>(t_final_ / h_) + 1);
    
    Eigen::Vector4d y = y0;
    double t = 0.0;
    double N0 = y.sum();
    
    // Estado inicial
    EstadoSEIR estado_inicial;
    estado_inicial.tiempo = t;
    estado_inicial.y = y;
    estado_inicial.conservacion = y.sum() - N0;
    historia.push_back(estado_inicial);
    
    // Ciclo principal
    int num_pasos = static_cast<int>(t_final_ / h_);
    for (int i = 0; i < num_pasos; ++i) {
        y = paso_rk4(t, y, escenario);
        t += h_;
        
        EstadoSEIR estado;
        estado.tiempo = t;
        estado.y = y;
        estado.conservacion = y.sum() - N0;
        historia.push_back(estado);
    }
    
    return historia;
}
```

---

### 2.3 Módulo: `exportador.hpp` / `exportador.cpp`

**Responsabilidad:** Exportar resultados a CSV.

```cpp
// exportador.hpp
#ifndef EXPORTADOR_HPP
#define EXPORTADOR_HPP

#include "solver_rk4.hpp"
#include <string>

class Exportador {
public:
    // Exportar evolución temporal
    static bool exportar_evolucion(const std::string& archivo,
                                    const std::vector<EstadoSEIR>& historia);
    
    // Exportar verificación de conservación
    static bool exportar_conservacion(const std::string& archivo,
                                       const std::vector<EstadoSEIR>& historia);
    
    // Exportar resumen comparativo
    static bool exportar_resumen(const std::string& archivo,
                                  double pico_A, double dia_pico_A, 
                                  double recuperados_A, double error_A,
                                  double pico_B, double dia_pico_B, 
                                  double recuperados_B, double error_B);
};

#endif
```

**Formato de salida CSV:**

**`escenario_A.csv` y `escenario_B.csv`:**
```csv
tiempo,S,E,I,R
0.0,29999900.0,0.0,100.0,0.0
0.1,29999895.2,0.8,102.5,1.5
...
200.0,1500000.0,0.0,50.0,28499950.0
```

**`conservacion_A.csv` y `conservacion_B.csv`:**
```csv
tiempo,error_absoluto
0.0,0.0
0.1,0.0
...
200.0,0.00012
```

**`resumen.csv`:**
```csv
metrica,escenario_A,escenario_B
pico_infectados,12000000.0,6500000.0
dia_pico,45.3,68.7
recuperados_finales,28000000.0,20000000.0
error_max_conservacion,1.2e-09,1.5e-09
```

---

### 2.4 Módulo: `main.cpp`

**Responsabilidad:** Orquestar el flujo completo.

```cpp
// main.cpp
#include <iostream>
#include <iomanip>
#include "modelo_seir.hpp"
#include "solver_rk4.hpp"
#include "exportador.hpp"

int main() {
    std::cout << "=== MODELO EPIDEMIOLÓGICO SEIR ===" << std::endl;
    std::cout << "Solver: Runge-Kutta 4to Orden (RK4)" << std::endl;
    
    // Parámetros del modelo
    const double N = 3e7;
    const double sigma = 0.2;   // 1/5 días
    const double gamma = 0.1;   // 1/10 días
    
    ModeloSEIR modelo(N, sigma, gamma);
    
    // Parámetros de simulación
    const double h = 0.1;       // Paso temporal (días)
    const double t_final = 200; // Tiempo final (días)
    
    SolverRK4 solver(modelo, h, t_final);
    
    // Condiciones iniciales: S₀ = N-100, E₀ = 0, I₀ = 100, R₀ = 0
    Eigen::Vector4d y0;
    y0 << N - 100.0, 0.0, 100.0, 0.0;
    
    std::cout << "\nCondiciones iniciales:" << std::endl;
    std::cout << "  S(0) = " << y0(0) << std::endl;
    std::cout << "  E(0) = " << y0(1) << std::endl;
    std::cout << "  I(0) = " << y0(2) << std::endl;
    std::cout << "  R(0) = " << y0(3) << std::endl;
    std::cout << "  N(0) = " << y0.sum() << std::endl;
    
    // ============================================
    // ESCENARIO A: Epidemia Libre
    // ============================================
    std::cout << "\n=== ESCENARIO A: Epidemia Libre ===" << std::endl;
    std::cout << "  β = 0.6 (constante)" << std::endl;
    std::cout << "  R₀ = " << modelo.calcular_R0(1) << std::endl;
    
    auto historia_A = solver.resolver(1, y0);
    
    double pico_A = solver.calcular_pico_infectados(historia_A);
    double dia_pico_A = solver.calcular_dia_pico(historia_A);
    double recuperados_A = solver.calcular_recuperados_finales(historia_A);
    double error_A = solver.calcular_error_max_conservacion(historia_A);
    
    std::cout << "  Pico de infectados: " << pico_A << std::endl;
    std::cout << "  Día del pico: " << dia_pico_A << std::endl;
    std::cout << "  Recuperados finales: " << recuperados_A << std::endl;
    std::cout << "  Error máx conservación: " << error_A << std::endl;
    
    // ============================================
    // ESCENARIO B: Con Intervención
    // ============================================
    std::cout << "\n=== ESCENARIO B: Con Intervención ===" << std::endl;
    std::cout << "  β = 0.6 (t < 30), β = 0.25 (t >= 30)" << std::endl;
    std::cout << "  R₀ inicial = 6.0, R₀ intervención = 2.5" << std::endl;
    
    auto historia_B = solver.resolver(2, y0);
    
    double pico_B = solver.calcular_pico_infectados(historia_B);
    double dia_pico_B = solver.calcular_dia_pico(historia_B);
    double recuperados_B = solver.calcular_recuperados_finales(historia_B);
    double error_B = solver.calcular_error_max_conservacion(historia_B);
    
    std::cout << "  Pico de infectados: " << pico_B << std::endl;
    std::cout << "  Día del pico: " << dia_pico_B << std::endl;
    std::cout << "  Recuperados finales: " << recuperados_B << std::endl;
    std::cout << "  Error máx conservación: " << error_B << std::endl;
    
    // ============================================
    // EXPORTAR RESULTADOS
    // ============================================
    std::cout << "\n=== EXPORTANDO RESULTADOS ===" << std::endl;
    
    Exportador::exportar_evolucion("../../resultados/escenario_A.csv", historia_A);
    Exportador::exportar_evolucion("../../resultados/escenario_B.csv", historia_B);
    Exportador::exportar_conservacion("../../resultados/conservacion_A.csv", historia_A);
    Exportador::exportar_conservacion("../../resultados/conservacion_B.csv", historia_B);
    Exportador::exportar_resumen("../../resultados/resumen.csv",
                                  pico_A, dia_pico_A, recuperados_A, error_A,
                                  pico_B, dia_pico_B, recuperados_B, error_B);
    
    std::cout << "Resultados exportados a resultados/" << std::endl;
    
    return 0;
}
```

---

## 3. Especificaciones Técnicas

### 3.1 Tipos de Datos

| Tipo | Eigen Type | Uso |
|------|-----------|-----|
| Vector de estado | `Eigen::Vector4d` | [S, E, I, R] |
| Historial | `std::vector<EstadoSEIR>` | Evolución temporal |

### 3.2 Parámetros de Configuración

| Parámetro | Valor | Descripción |
|-----------|-------|-------------|
| `N` | $3 \times 10^7$ | Población total |
| `sigma` | $0.2$ | Tasa E → I |
| `gamma` | $0.1$ | Tasa I → R |
| `h` | $0.1$ días | Paso temporal |
| `t_final` | $200$ días | Tiempo final |
| `y0` | $[N-100, 0, 100, 0]^T$ | Condiciones iniciales |

### 3.3 Estructura de Archivos de Salida

```
resultados/
├── escenario_A.csv              # Evolución temporal escenario A
├── escenario_B.csv              # Evolución temporal escenario B
├── conservacion_A.csv           # Verificación conservación A
├── conservacion_B.csv           # Verificación conservación B
└── resumen.csv                  # Métricas comparativas
```

---

## 4. Dependencias y Compilación

### 4.1 CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(SEIR_RK4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Eigen3
set(VCPKG_APPLOCAL_DEPS OFF)
find_package(Eigen3 CONFIG REQUIRED)

# Ejecutable
add_executable(seir_rk4
    src/main.cpp
    src/modelo_seir.cpp
    src/solver_rk4.cpp
    src/exportador.cpp
)

target_link_libraries(seir_rk4 PRIVATE Eigen3::Eigen)
target_include_directories(seir_rk4 PRIVATE src)
```

---

## 5. Resumen: Cómo Programar el Escenario B

### 5.1 Flujo de ejecución

```
main.cpp
   │
   ├─► solver.resolver(escenario=2, y0)
   │      │
   │      └─► paso_rk4(t, y, escenario=2)
   │             │
   │             ├─► k1 = modelo.evaluar(t, y, escenario=2)
   │             │      └─► beta = obtener_beta(t, escenario=2)
   │             │             └─► retorna 0.6 si t<30, 0.25 si t≥30
   │             │
   │             ├─► k2 = modelo.evaluar(t+h/2, y+h/2·k1, escenario=2)
   │             │      └─► beta = obtener_beta(t+h/2, escenario=2)
   │             │
   │             ├─► k3 = modelo.evaluar(t+h/2, y+h/2·k2, escenario=2)
   │             │      └─► beta = obtener_beta(t+h/2, escenario=2)
   │             │
   │             └─► k4 = modelo.evaluar(t+h, y+h·k3, escenario=2)
   │                    └─► beta = obtener_beta(t+h, escenario=2)
   │
   └─► Retorna historial completo
```


## 6. Checklist de Implementación

- [x] Implementar `modelo_seir.hpp/cpp` con función $\mathbf{f}(t, \mathbf{y})$ y `obtener_beta(t, escenario)`
- [x] Implementar `solver_rk4.hpp/cpp` con método RK4 manual
- [x] Implementar `exportador.hpp/cpp` con escritura CSV
- [x] Implementar `main.cpp` con flujo completo para ambos escenarios
- [x] Configurar `CMakeLists.txt` con Eigen
- [ ] Verificación de resultados CSV

---

**Fin del documento SPEC-001.md**

---