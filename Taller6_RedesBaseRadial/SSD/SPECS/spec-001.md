# 📘 SPEC-001.md — Módulo 1: Generador de Datos de Entrenamiento

> **Documento de Especificación Técnica**  
> **Proyecto:** Taller 6 — Interpolación RBF  
> **Módulo:** `generador_datos.hpp` / `generador_datos.cpp`  
> **Responsabilidad:** Generar N puntos aleatorios en [-2,2]² con valores z = sin(x)cos(y) + ruido gaussiano

---

## 1. Propósito del Módulo

Este módulo es el **primer paso del pipeline**. Su única responsabilidad es:

1. Generar N=100 pares $(x_i, y_i)$ distribuidos uniformemente en $[-2, 2]^2$
2. Calcular $z_i = \sin(x_i)\cos(y_i) + \mathcal{N}(0, \sigma^2)$ con $\sigma = 0.1$
3. Almacenar los datos en estructuras Eigen (`MatrixXd` y `VectorXd`)
4. Exportar a `datos_entrenamiento.csv` con formato `x,y,z`

---

## 2. Contrato de Entrada/Salida

### 2.1 Entrada (Parámetros del Constructor)

| Parámetro | Tipo | Valor | Descripción |
|-----------|------|-------|-------------|
| `N` | `int` | 100 | Número de puntos de entrenamiento |
| `min_val` | `double` | -2.0 | Límite inferior del dominio |
| `max_val` | `double` | 2.0 | Límite superior del dominio |
| `ruido_sigma` | `double` | 0.1 | Desviación estándar del ruido |
| `seed` | `unsigned int` | 42 | Semilla para reproducibilidad |

### 2.2 Salida (Archivos CSV)

**`resultados/datos_entrenamiento.csv`:**
```csv
x,y,z
-1.234567,0.567890,-0.823456
0.987654,-1.123456,0.456789
...
```

### 2.3 Salida (Estructuras Eigen)

| Método | Retorno | Dimensiones |
|--------|---------|-------------|
| `get_puntos()` | `Eigen::MatrixXd` | $N \times 2$ (columnas: x, y) |
| `get_valores()` | `Eigen::VectorXd` | $N \times 1$ (valores z ruidosos) |
| `get_valores_limpios()` | `Eigen::VectorXd` | $N \times 1$ (valores z sin ruido, para referencia) |

---

## 3. Especificación de la Clase

### 3.1 Archivo `src/generador_datos.hpp`

```cpp
#ifndef GENERADOR_DATOS_HPP
#define GENERADOR_DATOS_HPP

#include <Eigen/Dense>
#include <string>

class GeneradorDatos {
public:
    /**
     * @brief Constructor del generador de datos
     * @param N Número de puntos a generar
     * @param min_val Límite inferior del dominio (ej. -2.0)
     * @param max_val Límite superior del dominio (ej. 2.0)
     * @param ruido_sigma Desviación estándar del ruido gaussiano
     * @param seed Semilla para reproducibilidad (default: 42)
     */
    GeneradorDatos(int N, double min_val, double max_val,
                   double ruido_sigma, unsigned int seed = 42);

    /**
     * @brief Genera los N puntos de entrenamiento
     *        Llena internamente puntos_ y valores_
     */
    void generar();

    /**
     * @brief Exporta los datos a un archivo CSV
     * @param archivo Ruta del archivo (ej. "../resultados/datos_entrenamiento.csv")
     * @return true si la exportación fue exitosa
     */
    bool exportar_csv(const std::string& archivo) const;

    // ============ Getters ============
    const Eigen::MatrixXd& get_puntos() const { return puntos_; }
    const Eigen::VectorXd& get_valores() const { return valores_; }
    const Eigen::VectorXd& get_valores_limpios() const { return valores_limpios_; }
    int get_N() const { return N_; }

private:
    int N_;                    // Número de puntos
    double min_val_;           // Límite inferior del dominio
    double max_val_;           // Límite superior del dominio
    double ruido_sigma_;       // Desviación estándar del ruido
    unsigned int seed_;        // Semilla para reproducibilidad

    Eigen::MatrixXd puntos_;         // N x 2: columnas [x, y]
    Eigen::VectorXd valores_;        // N x 1: z con ruido
    Eigen::VectorXd valores_limpios_; // N x 1: z sin ruido (referencia)

    /**
     * @brief Función objetivo: f(x, y) = sin(x) * cos(y)
     */
    double funcion_objetivo(double x, double y) const;
};

#endif // GENERADOR_DATOS_HPP
```

### 3.2 Archivo `src/generador_datos.cpp`

```cpp
#include "generador_datos.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <random>
#include <filesystem>

GeneradorDatos::GeneradorDatos(int N, double min_val, double max_val,
                               double ruido_sigma, unsigned int seed)
    : N_(N), min_val_(min_val), max_val_(max_val),
      ruido_sigma_(ruido_sigma), seed_(seed) {

    // Reservar espacio en las matrices Eigen
    puntos_.resize(N_, 2);
    valores_.resize(N_);
    valores_limpios_.resize(N_);
}

double GeneradorDatos::funcion_objetivo(double x, double y) const {
    return std::sin(x) * std::cos(y);
}

void GeneradorDatos::generar() {
    // Generadores aleatorios con semilla fija
    std::mt19937 gen(seed_);
    std::uniform_real_distribution<double> dist_xy(min_val_, max_val_);
    std::normal_distribution<double> ruido(0.0, ruido_sigma_);

    for (int i = 0; i < N_; ++i) {
        // 1. Generar coordenadas (x, y) uniformes en [-2, 2]^2
        double x = dist_xy(gen);
        double y = dist_xy(gen);

        puntos_(i, 0) = x;
        puntos_(i, 1) = y;

        // 2. Calcular z = sin(x)*cos(y) + ruido
        double z_limpio = funcion_objetivo(x, y);
        double z_ruidoso = z_limpio + ruido(gen);

        valores_limpios_(i) = z_limpio;
        valores_(i) = z_ruidoso;
    }

    std::cout << "[OK] Generados " << N_ << " puntos de entrenamiento" << std::endl;
    std::cout << "     Dominio: [" << min_val_ << ", " << max_val_ << "]^2" << std::endl;
    std::cout << "     Ruido gaussiano: sigma = " << ruido_sigma_ << std::endl;
    std::cout << "     Semilla: " << seed_ << std::endl;
}

bool GeneradorDatos::exportar_csv(const std::string& archivo) const {
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

    // Header
    out << "x,y,z\n";
    out << std::fixed << std::setprecision(6);

    // Datos
    for (int i = 0; i < N_; ++i) {
        out << puntos_(i, 0) << ","
            << puntos_(i, 1) << ","
            << valores_(i) << "\n";
    }

    out.close();
    std::cout << "[OK] Datos exportados a: " << archivo << std::endl;
    std::cout << "     Filas: " << N_ << " (+ header)" << std::endl;
    return true;
}
```

---

## 3. Criterios de Aceptación del Módulo 1

| Criterio | Verificación | 
|----------|--------------|
| **100 puntos generados** | `puntos.rows() == 100`| 
| **Dominio correcto** | `x, y ∈ [-2, 2]`|
| **Ruido aplicado** | `valores != valores_limpios`| 
| **Reproducibilidad** | Mismos datos con `seed=42`| 
| **CSV generado** | `resultados/datos_entrenamiento.csv` existe| 
| **CSV con header** | Primera línea: `x,y,z`|
| **CSV con 100 filas** | `wc -l` = 101 (header + 100 datos)|
| **Formato numérico** | 6 decimales de precisión |

---

## 9. Checklist de Implementación

- [ ] Crear archivo `src/generador_datos.hpp`
- [ ] Crear archivo `src/generador_datos.cpp`


**Fin del documento SPEC-001.md**

---
