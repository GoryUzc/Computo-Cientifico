///# 📘 SPEC-003.md — Módulo 3: Evaluador de Superficie en Malla

> **Documento de Especificación Técnica**  
> **Proyecto:** Taller 6 — Interpolación RBF  
> **Módulo:** `evaluador.hpp` / `evaluador.cpp`  
> **Responsabilidad:** Crear malla regular 50×50, evaluar la superficie RBF en cada punto, calcular errores y exportar resultados

---

## 1. Propósito del Módulo

Este módulo es el **puente entre el entrenamiento y la visualización**. Su responsabilidad es:

1. **Crear una malla regular** de 50×50 = 2500 puntos en $[-2, 2]^2$
2. **Evaluar la superficie RBF** $s(\mathbf{x}) = \sum_{j=1}^{N} w_j \phi(\|\mathbf{x} - \mathbf{x}_j\|)$ en cada punto de la malla
3. **Calcular el error de entrenamiento** (qué tan bien se ajustó el modelo a los datos ruidosos)
4. **Calcular el error de generalización** (qué tan bien aproxima la función real $\sin(x)\cos(y)$)
5. **Exportar los resultados** a CSV para visualización en Python

---

## 2. Contrato de Entrada/Salida

### 2.1 Entrada (Parámetros del Constructor)

| Parámetro | Tipo | Valor | Descripción |
|-----------|------|-------|-------------|
| `epsilon` | `double` | 0.5, 1.5, 5.0 | Parámetro de forma del kernel Gaussiano |
| `resolucion` | `int` | 50 | Número de puntos por dimensión |
| `min_val` | `double` | -2.0 | Límite inferior del dominio |
| `max_val` | `double` | 2.0 | Límite superior del dominio |

### 2.2 Entrada (Datos del Entrenamiento)

| Parámetro | Tipo | Dimensiones | Descripción |
|-----------|------|-------------|-------------|
| `centros` | `Eigen::MatrixXd` | N × 2 | Puntos de entrenamiento (x_j, y_j) |
| `pesos` | `Eigen::VectorXd` | N × 1 | Pesos w_j calculados por el solver |
| `valores_train` | `Eigen::VectorXd` | N × 1 | Valores objetivo y_j (con ruido) |

### 2.3 Salida (Estructuras Eigen)

| Método | Retorno | Dimensiones | Descripción |
|--------|---------|-------------|-------------|
| `crear_malla()` | `Eigen::MatrixXd` | M × 2 | Puntos de la malla (M = resolución²) |
| `evaluar()` | `Eigen::VectorXd` | M × 1 | Valores z_predicho en la malla |
| `evaluar_en_centros()` | `Eigen::VectorXd` | N × 1 | Valores z_predicho en los centros |

### 2.4 Salida (Métricas Escalares)

| Método | Retorno | Descripción |
|--------|---------|-------------|
| `calcular_error_entrenamiento()` | `double` | RMSE en puntos de entrenamiento |
| `calcular_error_funcion_real()` | `double` | RMSE vs función real (sin ruido) |

### 2.5 Salida (Archivos CSV)

**`resultados/superficie_eps_X.csv`:**
```csv
x,y,z_predicho
-2.000000,-2.000000,0.123456
-1.918367,-2.000000,0.234567
...
```
**Filas:** 2500 (50×50)

**`resultados/funcion_real.csv`:**
```csv
x,y,z_real
-2.000000,-2.000000,0.123456
...
```
**Filas:** 2500 (para comparar con la predicción)

---

## 3. Especificación de la Clase

### 3.1 Archivo `src/evaluador.hpp`

```cpp
#ifndef EVALUADOR_HPP
#define EVALUADOR_HPP

#include <Eigen/Dense>
#include <string>

class Evaluador {
public:
    /**
     * @brief Constructor del evaluador de superficie
     * @param epsilon Parámetro de forma del kernel Gaussiano
     * @param resolucion Número de puntos por dimensión (default: 50)
     * @param min_val Límite inferior del dominio (default: -2.0)
     * @param max_val Límite superior del dominio (default: 2.0)
     */
    Evaluador(double epsilon, int resolucion = 50, 
              double min_val = -2.0, double max_val = 2.0);

    /**
     * @brief Crea una malla regular de resolución × resolución puntos
     * @return Matriz M×2 con las coordenadas (x, y) de cada punto
     *         donde M = resolución²
     */
    Eigen::MatrixXd crear_malla() const;

    /**
     * @brief Evalúa la superficie RBF en los puntos de la malla
     * @param malla Matriz M×2 con puntos a evaluar
     * @param centros Matriz N×2 con los centros de las RBF
     * @param pesos Vector N×1 con los pesos w_j
     * @return Vector M×1 con los valores z_predicho
     * 
     * Fórmula: s(x_k) = Σ_j w_j · exp(-ε² · ||x_k - x_j||²)
     * Costo: O(M × N)
     */
    Eigen::VectorXd evaluar(const Eigen::MatrixXd& malla,
                            const Eigen::MatrixXd& centros,
                            const Eigen::VectorXd& pesos) const;

    /**
     * @brief Evalúa la superficie RBF en los puntos de entrenamiento
     * @param centros Matriz N×2 con los centros
     * @param pesos Vector N×1 con los pesos
     * @return Vector N×1 con los valores predichos en los centros
     */
    Eigen::VectorXd evaluar_en_centros(const Eigen::MatrixXd& centros,
                                        const Eigen::VectorXd& pesos) const;

    /**
     * @brief Calcula el error RMSE en los puntos de entrenamiento
     * @param y_real Valores reales (con ruido)
     * @param y_predicho Valores predichos por la RBF
     * @return RMSE = sqrt( (1/N) Σ (y_real - y_predicho)² )
     */
    static double calcular_rmse(const Eigen::VectorXd& y_real,
                                const Eigen::VectorXd& y_predicho);

    /**
     * @brief Calcula los valores de la función real f(x,y) = sin(x)cos(y)
     * @param malla Matriz M×2 con puntos
     * @return Vector M×1 con los valores reales
     */
    static Eigen::VectorXd funcion_real(const Eigen::MatrixXd& malla);

    /**
     * @brief Exporta la superficie predicha a CSV
     * @param archivo Ruta del archivo
     * @param malla Matriz M×2 con coordenadas
     * @param z_predicho Vector M×1 con valores predichos
     * @return true si la exportación fue exitosa
     */
    static bool exportar_superficie(const std::string& archivo,
                                     const Eigen::MatrixXd& malla,
                                     const Eigen::VectorXd& z_predicho);

    /**
     * @brief Exporta la función real a CSV (para comparación)
     * @param archivo Ruta del archivo
     * @param malla Matriz M×2 con coordenadas
     * @return true si la exportación fue exitosa
     */
    static bool exportar_funcion_real(const std::string& archivo,
                                       const Eigen::MatrixXd& malla);

    // Getters
    double get_epsilon() const { return epsilon_; }
    int get_resolucion() const { return resolucion_; }
    int get_M() const { return resolucion_ * resolucion_; }

private:
    double epsilon_;     // Parámetro de forma
    int resolucion_;     // Puntos por dimensión
    double min_val_;     // Límite inferior
    double max_val_;     // Límite superior

    /**
     * @brief Kernel Gaussiano: φ(r) = exp(-(ε·r)²)
     * @param r Distancia euclidiana
     * @return Valor del kernel
     */
    double kernel(double r) const;
};

#endif // EVALUADOR_HPP
```

### 3.2 Archivo `src/evaluador.cpp`

```cpp
#include "evaluador.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <filesystem>

Evaluador::Evaluador(double epsilon, int resolucion, double min_val, double max_val)
    : epsilon_(epsilon), resolucion_(resolucion), 
      min_val_(min_val), max_val_(max_val) {}

double Evaluador::kernel(double r) const {
    return std::exp(-std::pow(epsilon_ * r, 2));
}

Eigen::MatrixXd Evaluador::crear_malla() const {
    int M = resolucion_ * resolucion_;
    Eigen::MatrixXd malla(M, 2);
    
    double paso = (max_val_ - min_val_) / (resolucion_ - 1);
    
    std::cout << "  Creando malla " << resolucion_ << "x" << resolucion_ 
              << " = " << M << " puntos" << std::endl;
    std::cout << "    Dominio: [" << min_val_ << ", " << max_val_ << "]^2" << std::endl;
    std::cout << "    Paso: " << paso << std::endl;
    
    // Barrido por filas: primero y (i), luego x (j)
    // IMPORTANTE: Este orden debe ser consistente con Python para reshape(50, 50)
    int idx = 0;
    for (int i = 0; i < resolucion_; ++i) {
        for (int j = 0; j < resolucion_; ++j) {
            malla(idx, 0) = min_val_ + j * paso;  // x
            malla(idx, 1) = min_val_ + i * paso;  // y
            idx++;
        }
    }
    
    return malla;
}

Eigen::VectorXd Evaluador::evaluar(const Eigen::MatrixXd& malla,
                                    const Eigen::MatrixXd& centros,
                                    const Eigen::VectorXd& pesos) const {
    int M = malla.rows();      // 2500 puntos a evaluar
    int N = pesos.size();      // 100 centros
    Eigen::VectorXd z(M);
    
    std::cout << "  Evaluando superficie RBF..." << std::endl;
    std::cout << "    Puntos a evaluar: " << M << std::endl;
    std::cout << "    Centros (N): " << N << std::endl;
    std::cout << "    Evaluaciones del kernel: " << M * N << std::endl;
    
    for (int k = 0; k < M; ++k) {
        double suma = 0.0;
        for (int j = 0; j < N; ++j) {
            // Distancia euclidiana entre punto de malla k y centro j
            double dx = malla(k, 0) - centros(j, 0);
            double dy = malla(k, 1) - centros(j, 1);
            double r = std::sqrt(dx * dx + dy * dy);
            
            // Contribución del centro j: w_j · φ(||x_k - x_j||)
            suma += pesos(j) * kernel(r);
        }
        z(k) = suma;
    }
    
        std::cout << "    Rango z_predicho: [" << z.minCoeff() << ", " << z.maxCoeff() << "]" << std::endl;
    
    return z;
}

// ACTUALIZACIÓN CÓDIGO REAL (evaluador.cpp):
// - Evita sqrt() en bucle interno; usa r² = dx² + dy² directamente
// - Computa exp(-eps2 * r2) sin llamar a kernel() por rendimiento
// - El orden de malla (y, x) se mantiene para consistencia con Python reshape

Eigen::VectorXd Evaluador::evaluar_en_centros(const Eigen::MatrixXd& centros,
                                               const Eigen::VectorXd& pesos) const {
    return evaluar(centros, centros, pesos);
}

double Evaluador::calcular_rmse(const Eigen::VectorXd& y_real,
                                 const Eigen::VectorXd& y_predicho) {
    if (y_real.size() != y_predicho.size()) {
        std::cerr << "[ERROR] Dimensiones incompatibles en RMSE" << std::endl;
        return -1.0;
    }
    
    int N = y_real.size();
    double suma_sq = 0.0;
    
    for (int i = 0; i < N; ++i) {
        double diff = y_real(i) - y_predicho(i);
        suma_sq += diff * diff;
    }
    
    return std::sqrt(suma_sq / N);
}

Eigen::VectorXd Evaluador::funcion_real(const Eigen::MatrixXd& malla) {
    int M = malla.rows();
    Eigen::VectorXd z_real(M);
    
    for (int k = 0; k < M; ++k) {
        double x = malla(k, 0);
        double y = malla(k, 1);
        z_real(k) = std::sin(x) * std::cos(y);
    }
    
    return z_real;
}

bool Evaluador::exportar_superficie(const std::string& archivo,
                                     const Eigen::MatrixXd& malla,
                                     const Eigen::VectorXd& z_predicho) {
    // Crear directorio padre si no existe
    std::filesystem::path path(archivo);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }
    
    std::ofstream out(archivo);
    if (!out.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir: " << archivo << std::endl;
        return false;
    }
    
    out << "x,y,z_predicho\n";
    out << std::fixed << std::setprecision(6);
    
    for (int i = 0; i < malla.rows(); ++i) {
        out << malla(i, 0) << ","
            << malla(i, 1) << ","
            << z_predicho(i) << "\n";
    }
    
    out.close();
    std::cout << "[OK] Superficie exportada a: " << archivo << std::endl;
    std::cout << "     Filas: " << malla.rows() << " (+ header)" << std::endl;
    return true;
}

bool Evaluador::exportar_funcion_real(const std::string& archivo,
                                       const Eigen::MatrixXd& malla) {
    Eigen::VectorXd z_real = funcion_real(malla);
    
    std::filesystem::path path(archivo);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }
    
    std::ofstream out(archivo);
    if (!out.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir: " << archivo << std::endl;
        return false;
    }
    
    out << "x,y,z_real\n";
    out << std::fixed << std::setprecision(6);
    
    for (int i = 0; i < malla.rows(); ++i) {
        out << malla(i, 0) << ","
            << malla(i, 1) << ","
            << z_real(i) << "\n";
    }
    
    out.close();
    std::cout << "[OK] Función real exportada a: " << archivo << std::endl;
    return true;
}
```

---

## 4. Criterios de Aceptación del Módulo 3

| Criterio | Verificación |
|----------|--------------|
| **Malla 50×50** | `malla.rows() == 2500` |
| **Dominio correcto** | `x, y ∈ [-2, 2]` 
| **Paso uniforme** | `paso = 4/49 ≈ 0.0816` |
| **z_predicho razonable** | `z ∈ [-1.5, 1.5]` |
| **RMSE entrenamiento** | `RMSE ≈ σ = 0.1` |
| **RMSE generalización** | `RMSE < 0.3` |
| **Predicción en centros ≈ valores** | `max diff < 1e-3` |
| **CSV superficie generado** | `superficie_eps_X.csv` existe |
| **CSV función real generado** | `funcion_real.csv` existe |
| **CSV con 2500 filas** | `wc -l` = 2501 |

---

## 5. Checklist de Implementación

- [ ] Crear archivo `src/evaluador.hpp`
- [ ] Crear archivo `src/evaluador.cpp`

---

**Fin del documento SPEC-003.md**

--- 