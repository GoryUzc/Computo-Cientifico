# 📘 SPEC-002.md — Módulo 2: Núcleo RBF (Construcción de Φ, Regularización y Solver)

> **Documento de Especificación Técnica**  
> **Proyecto:** Taller 6 — Interpolación RBF  
> **Módulo:** `rbf_core.hpp` / `rbf_core.cpp`  
> **Responsabilidad:** Construir la matriz de Gram Φ, aplicar regularización de Tikhonov, resolver el sistema lineal (Φ + λI)w = y

---

## 1. Propósito del Módulo

Este módulo es el **corazón matemático** del proyecto. Su responsabilidad es:

1. **Construir la matriz de Gram** Φ ∈ ℝ^(N×N) usando el kernel Gaussiano
2. **Aplicar regularización de Tikhonov** sumando λI a la diagonal
3. **Resolver el sistema lineal** (Φ + λI)w = y para obtener los pesos w
4. **Calcular el número de condición** de la matriz regularizada
5. **Exportar los pesos** a CSV para análisis posterior

---

## 2. Contrato de Entrada/Salida

### 2.1 Entrada (Parámetros del Constructor)

| Parámetro | Tipo | Valor | Descripción |
|-----------|------|-------|-------------|
| `epsilon` | `double` | 0.5, 1.5, 5.0 | Parámetro de forma del kernel Gaussiano |
| `lambda` | `double` | 1e-6 | Parámetro de regularización de Tikhonov |

### 2.2 Entrada (Datos de Entrenamiento)

| Parámetro | Tipo | Dimensiones | Descripción |
|-----------|------|-------------|-------------|
| `puntos` | `Eigen::MatrixXd` | N × 2 | Coordenadas (x, y) de los centros |
| `valores` | `Eigen::VectorXd` | N × 1 | Valores objetivo z (con ruido) |

### 2.3 Salida (Estructuras Eigen)

| Método | Retorno | Dimensiones | Descripción |
|--------|---------|-------------|-------------|
| `construir_Phi()` | `Eigen::MatrixXd` | N × N | Matriz de Gram |
| `regularizar()` | `Eigen::MatrixXd` | N × N | Φ + λI |
| `resolver()` | `Eigen::VectorXd` | N × 1 | Vector de pesos w |
| `calcular_condicion()` | `double` | escalar | Número de condición de Φ + λI |

### 2.4 Salida (Archivos CSV)

**`resultados/pesos_eps_X.csv`:**
```csv
indice,peso
0,0.123456
1,-0.234567
...
```

---

## 3. Especificación de la Clase

### 3.1 Archivo `src/rbf_core.hpp`

```cpp
#ifndef RBF_CORE_HPP
#define RBF_CORE_HPP

#include <Eigen/Dense>
#include <string>

class RBFCore {
public:
    /**
     * @brief Constructor del núcleo RBF
     * @param epsilon Parámetro de forma del kernel Gaussiano
     * @param lambda Parámetro de regularización de Tikhonov
     */
    RBFCore(double epsilon, double lambda);

    /**
     * @brief Construye la matriz de Gram Φ
     * @param puntos Matriz N×2 con coordenadas (x, y) de los centros
     * @return Matriz Φ de tamaño N×N
     * 
     * Φ_ij = exp(-ε² ||x_i - x_j||²)
     */
    Eigen::MatrixXd construir_Phi(const Eigen::MatrixXd& puntos) const;

    /**
     * @brief Aplica regularización de Tikhonov: Φ + λI
     * @param Phi Matriz de Gram original
     * @return Matriz regularizada
     */
    Eigen::MatrixXd regularizar(const Eigen::MatrixXd& Phi) const;

    /**
     * @brief Resuelve el sistema lineal (Φ + λI)w = y
     * @param Phi_reg Matriz regularizada
     * @param y Vector de valores objetivo
     * @return Vector de pesos w
     */
    Eigen::VectorXd resolver(const Eigen::MatrixXd& Phi_reg, 
                             const Eigen::VectorXd& y) const;

    /**
     * @brief Calcula el número de condición de una matriz
     * @param A Matriz a analizar
     * @return Número de condición (λ_max / λ_min)
     */
    double calcular_condicion(const Eigen::MatrixXd& A) const;

    /**
     * @brief Exporta los pesos a un archivo CSV
     * @param archivo Ruta del archivo
     * @param pesos Vector de pesos
     * @return true si la exportación fue exitosa
     */
    static bool exportar_pesos(const std::string& archivo, 
                               const Eigen::VectorXd& pesos);

    // Getters
    double get_epsilon() const { return epsilon_; }
    double get_lambda() const { return lambda_; }

private:
    double epsilon_;  // Parámetro de forma
    double lambda_;   // Parámetro de regularización

    /**
     * @brief Kernel Gaussiano: φ(r) = exp(-(ε·r)²)
     * @param r Distancia euclidiana
     * @return Valor del kernel
     */
    double kernel(double r) const;

    /**
     * @brief Calcula la distancia euclidiana entre dos puntos 2D
     * @param puntos Matriz de puntos
     * @param i Índice del primer punto
     * @param j Índice del segundo punto
     * @return Distancia euclidiana
     */
    double distancia(const Eigen::MatrixXd& puntos, int i, int j) const;
};

#endif // RBF_CORE_HPP
```

### 3.2 Archivo `src/rbf_core.cpp`

```cpp
#include "rbf_core.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <filesystem>
#include <Eigen/Eigenvalues>

RBFCore::RBFCore(double epsilon, double lambda)
    : epsilon_(epsilon), lambda_(lambda) {}

double RBFCore::kernel(double r) const {
    return std::exp(-std::pow(epsilon_ * r, 2));
}

// ACTUALIZACIÓN CÓDIGO REAL: kernel usa `double arg = epsilon_ * r; return std::exp(-(arg * arg));`
// (equivalente algebraico, optimizado para evitar pow())
}

double RBFCore::distancia(const Eigen::MatrixXd& puntos, int i, int j) const {
    double dx = puntos(i, 0) - puntos(j, 0);
    double dy = puntos(i, 1) - puntos(j, 1);
    return std::sqrt(dx * dx + dy * dy);
}

Eigen::MatrixXd RBFCore::construir_Phi(const Eigen::MatrixXd& puntos) const {
    int N = puntos.rows();
    Eigen::MatrixXd Phi(N, N);

    std::cout << "  Construyendo matriz de Gram Phi (" << N << "x" << N << ")..." << std::endl;
    std::cout << "    Epsilon: " << epsilon_ << std::endl;

    // Aprovechar simetría: solo calcular mitad superior
    for (int i = 0; i < N; ++i) {
        Phi(i, i) = 1.0;  // Diagonal: distancia = 0 → exp(0) = 1
        
        for (int j = i + 1; j < N; ++j) {
            double r = distancia(puntos, i, j);
            double phi_ij = kernel(r);
            Phi(i, j) = phi_ij;
            Phi(j, i) = phi_ij;  // Simetría
        }
    }

    // Validaciones
    std::cout << "    Diagonal min: " << Phi.diagonal().minCoeff() 
              << " (esperado: 1.0)" << std::endl;
    std::cout << "    Simetría error: " << (Phi - Phi.transpose()).norm() 
              << " (esperado: ~0)" << std::endl;

    return Phi;
}

Eigen::MatrixXd RBFCore::regularizar(const Eigen::MatrixXd& Phi) const {
    int N = Phi.rows();
    Eigen::MatrixXd Phi_reg = Phi;

    std::cout << "  Aplicando regularización de Tikhonov (lambda = " 
              << lambda_ << ")..." << std::endl;

    // Sumar lambda a la diagonal
    for (int i = 0; i < N; ++i) {
        Phi_reg(i, i) += lambda_;
    }

    return Phi_reg;
}

Eigen::VectorXd RBFCore::resolver(const Eigen::MatrixXd& Phi_reg, 
                                   const Eigen::VectorXd& y) const {
    std::cout << "  Resolviendo sistema lineal (Phi + lambda*I)w = y..." << std::endl;

    // Usar LDLT para matrices simétricas definidas positivas
    Eigen::LDLT<Eigen::MatrixXd> solver(Phi_reg);
    
    if (solver.info() != Eigen::Success) {
        std::cerr << "[ERROR] La matriz no se pudo descomponer con LDLT" << std::endl;
        std::cerr << "  Intentando con ColPivHouseholderQR..." << std::endl;
        
        // Fallback: QR con pivoteo (más robusto)
        Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qr_solver(Phi_reg);
        if (qr_solver.info() != Eigen::Success) {
            std::cerr << "[ERROR] QR también falló" << std::endl;
            return Eigen::VectorXd();
        }
        return qr_solver.solve(y);
    }

    Eigen::VectorXd w = solver.solve(y);

    // Verificar residuo
    Eigen::VectorXd residuo = Phi_reg * w - y;
    double error_relativo = residuo.norm() / y.norm();
    
    std::cout << "    Solver exitoso (LDLT)" << std::endl;
    std::cout << "    Error relativo del residuo: " << error_relativo << std::endl;
    std::cout << "    Rango de pesos: [" << w.minCoeff() << ", " << w.maxCoeff() << "]" << std::endl;

    return w;
}

double RBFCore::calcular_condicion(const Eigen::MatrixXd& A) const {
    // Usar descomposición de valores propios para matrices simétricas
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(A);
    
    if (solver.info() != Eigen::Success) {
        std::cerr << "[WARNING] No se pudieron calcular los valores propios" << std::endl;
        return std::numeric_limits<double>::infinity();
    }

    Eigen::VectorXd eigenvalues = solver.eigenvalues();
    double lambda_max = eigenvalues.maxCoeff();
    double lambda_min = eigenvalues.minCoeff();

    if (std::abs(lambda_min) < 1e-15) {
        std::cerr << "[WARNING] Valor propio mínimo cercano a cero: " 
                  << lambda_min << std::endl;
        return std::numeric_limits<double>::infinity();
    }

    double condicion = std::abs(lambda_max / lambda_min);
    std::cout << "    Valores propios: min=" << lambda_min 
              << ", max=" << lambda_max << std::endl;
    std::cout << "    Número de condición: " << condicion << std::endl;

    return condicion;
}

bool RBFCore::exportar_pesos(const std::string& archivo, 
                              const Eigen::VectorXd& pesos) {
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

    out << "indice,peso\n";
    out << std::fixed << std::setprecision(6);

    for (int i = 0; i < pesos.size(); ++i) {
        out << i << "," << pesos(i) << "\n";
    }

    out.close();
    std::cout << "[OK] Pesos exportados a: " << archivo << std::endl;
    std::cout << "     Filas: " << pesos.size() << " (+ header)" << std::endl;
    return true;
}
```

---

## 4. Algoritmos Clave

### 4.1 Construcción de Φ (Aprovechando Simetría)

```
Para i = 0 hasta N-1:
    Phi(i, i) = 1.0  // Distancia a sí mismo = 0 → exp(0) = 1
    
    Para j = i+1 hasta N-1:
        r = sqrt((x_i - x_j)² + (y_i - y_j)²)
        phi_ij = exp(-(ε·r)²)
        Phi(i, j) = phi_ij
        Phi(j, i) = phi_ij  // Simetría
```

**Optimización:** Solo calculamos la mitad superior de la matriz, reduciendo el costo de O(N²) a O(N²/2).

### 4.2 Regularización de Tikhonov

```
Phi_reg = Phi
Para i = 0 hasta N-1:
    Phi_reg(i, i) += lambda
```

**Efecto:** Empuja los valores propios lejos de cero, garantizando que la matriz sea definida positiva.

### 4.3 Resolución del Sistema Lineal

```
solver = LDLT(Phi_reg)
Si solver.info() == Success:
    w = solver.solve(y)
Sino:
    // Fallback a QR
    qr_solver = ColPivHouseholderQR(Phi_reg)
    w = qr_solver.solve(y)
```

**¿Por qué LDLT?**
- Φ + λI es simétrica (por construcción)
- Φ + λI es definida positiva (porque λ > 0)
- LDLT es óptimo para este tipo de matrices: O(N³/3) vs O(N³) de LU

---


## 5. Criterios de Aceptación del Módulo 2

| Criterio | Verificación | 
|----------|--------------|
| **Φ es simétrica** | `(Phi - Phi.transpose()).norm()` |
| **Diagonal de Φ = 1** | `Phi.diagonal().minCoeff()` | 
| **Φ + λI es definida positiva** | `solver.info() == Eigen::Success` | 
| **Residuo del sistema** | `||(Φ+λI)w - y|| / ||y||` |
| **Pesos razonables** | `w.maxCoeff() - w.minCoeff()` | 
| **Número de condición** | `calcular_condicion()` |
| **CSV generado** | `pesos_eps_X.csv` existe | 
| **CSV con header** | Primera línea: `indice,peso` | 
| **CSV con N filas** | `wc -l` = N+1 | 

---

## 6. Checklist de Implementación

- [ ] Crear archivo `src/rbf_core.hpp`
- [ ] Crear archivo `src/rbf_core.cpp`


## 7. Notas Importantes 

### 7.1 ¿Por qué LDLT y no LU?

- **LDLT**(Descomposición de Cholesky modificada con pivoteo) está optimizado para matrices simétricas definidas positivas
- Costo: O(N³/3) vs O(N³) de LU
- Φ + λI es simétrica (por construcción) y definida positiva (porque λ > 0)

### 7.2 ¿Qué pasa si λ = 0?

- Φ puede ser singular (mal condicionada)
- LDLT fallará o producirá pesos gigantes
- La regularización es **esencial** para estabilidad numérica

### 7.3 ¿Cómo elegir ε?

- **ε pequeño (0.5):** Campanas anchas → Φ mal condicionada → oscilaciones
- **ε óptimo (1.5-2.0):** Balance entre suavidad y precisión
- **ε grande (5.0):** Campanas estrechas → Φ ≈ I → picos aislados

---

**Fin del documento SPEC-002.md**

