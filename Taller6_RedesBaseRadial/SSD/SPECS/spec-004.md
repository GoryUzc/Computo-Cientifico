
# 📘 SPEC-004.md — Módulo 4: Exportador de Resultados

> **Documento de Especificación Técnica**  
> **Proyecto:** Taller 6 — Interpolación RBF  
> **Módulo:** `exportador.hpp` / `exportador.cpp`  
> **Responsabilidad:** Exportar todos los resultados a archivos CSV para visualización en Python

---

## 1. Propósito del Módulo

Este módulo es el **puente entre C++ y Python**. Su responsabilidad es exportar:

1. **Datos de entrenamiento** (100 puntos con ruido)
2. **Pesos calculados** para cada valor de ε (3 archivos)
3. **Superficies evaluadas** en malla 50×50 (3 archivos)
4. **Resumen comparativo** de métricas de los 3 escenarios

Todos los CSV deben ser compatibles con `pandas.read_csv()` para facilitar el análisis en Python.

---

## 2. Contratos de Entrada/Salida

### 2.1 Archivos a Generar

| Archivo | Contenido | Filas | Columnas |
|---------|-----------|-------|----------|
| `datos_entrenamiento.csv` | Puntos (x, y, z) con ruido | 100 | 3 |
| `pesos_eps_0.50.csv` | Pesos w para ε=0.5 | 100 | 2 |
| `pesos_eps_1.50.csv` | Pesos w para ε=1.5 | 100 | 2 |
| `pesos_eps_5.00.csv` | Pesos w para ε=5.0 | 100 | 2 |
| `superficie_eps_0.50.csv` | Malla evaluada ε=0.5 | 2500 | 3 |
| `superficie_eps_1.50.csv` | Malla evaluada ε=1.5 | 2500 | 3 |
| `superficie_eps_5.00.csv` | Malla evaluada ε=5.0 | 2500 | 3 |

### 2.2 Formato CSV Estándar

Todos los CSV deben cumplir:
- **Primera línea:** Header con nombres de columnas
- **Separador:** Coma (`,`)
- **Precisión:** 6 decimales para coordenadas, notación científica para errores
- **Codificación:** UTF-8
- **Sin espacios:** Después de las comas

---

## 3. Especificación de la Clase

### 3.1 Archivo `src/exportador.hpp`

```cpp
#ifndef EXPORTADOR_HPP
#define EXPORTADOR_HPP

#include <Eigen/Dense>
#include <string>
#include <vector>

struct MetricasEscenario {
    double epsilon;
    double error_entrenamiento;  // RMSE en puntos de entrenamiento
    double error_malla;          // RMSE vs función real en malla
    double numero_condicion;     // Número de condición de (Φ + λI)
    double tiempo_calculo;       // Tiempo de entrenamiento en segundos
};

class Exportador {
public:
    // ============================================
    // EXPORTACIÓN DE DATOS DE ENTRENAMIENTO
    // ============================================
    
    /**
     * @brief Exporta los 100 puntos de entrenamiento (x, y, z)
     * @param archivo Ruta del archivo CSV
     * @param puntos Matriz 100×2 con coordenadas (x, y)
     * @param valores Vector 100×1 con valores z (con ruido)
     * @return true si la exportación fue exitosa
     * 
     * Formato CSV:
     * x,y,z
     * -1.234567,0.567890,-0.823456
     * ...
     */
    static bool exportar_datos_entrenamiento(
        const std::string& archivo,
        const Eigen::MatrixXd& puntos,
        const Eigen::VectorXd& valores
    );

    // ============================================
    // EXPORTACIÓN DE PESOS
    // ============================================
    
    /**
     * @brief Exporta los pesos w calculados para un ε específico
     * @param archivo Ruta del archivo CSV
     * @param pesos Vector 100×1 con pesos w
     * @param epsilon Valor de ε usado para calcular los pesos
     * @return true si la exportación fue exitosa
     * 
     * Formato CSV:
     * indice,peso
     * 0,0.123456
     * 1,-0.234567
     * ...
     */
    static bool exportar_pesos(
        const std::string& archivo,
        const Eigen::VectorXd& pesos,
        double epsilon
    );

    // ============================================
    // EXPORTACIÓN DE SUPERFICIES
    // ============================================
    
    /**
     * @brief Exporta la superficie evaluada en malla 50×50
     * @param archivo Ruta del archivo CSV
     * @param malla Matriz 2500×2 con coordenadas (x, y)
     * @param z_predicho Vector 2500×1 con valores z predichos
     * @param epsilon Valor de ε usado para la evaluación
     * @return true si la exportación fue exitosa
     * 
     * Formato CSV:
     * x,y,z_predicho
     * -2.000000,-2.000000,0.123456
     * -1.918367,-2.000000,0.234567
     * ...
     */
    static bool exportar_superficie(
        const std::string& archivo,
        const Eigen::MatrixXd& malla,
        const Eigen::VectorXd& z_predicho,
        double epsilon
    );

#endif // EXPORTADOR_HPP
```

### 3.2 Archivo `src/exportador.cpp`

```cpp
#include "exportador.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <filesystem>

// ============================================
// EXPORTACIÓN DE DATOS DE ENTRENAMIENTO
// ============================================

bool Exportador::exportar_datos_entrenamiento(
    const std::string& archivo,
    const Eigen::MatrixXd& puntos,
    const Eigen::VectorXd& valores) {
    
    // Verificar dimensiones
    if (puntos.rows() != valores.size()) {
        std::cerr << "[ERROR] Dimensiones incompatibles: puntos=" 
                  << puntos.rows() << ", valores=" << valores.size() << std::endl;
        return false;
    }
    
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
    for (int i = 0; i < puntos.rows(); ++i) {
        out << puntos(i, 0) << ","
            << puntos(i, 1) << ","
            << valores(i) << "\n";
    }
    
    out.close();
    std::cout << "[OK] Datos de entrenamiento exportados a: " << archivo << std::endl;
    std::cout << "     Puntos: " << puntos.rows() << std::endl;
    return true;
}

// ============================================
// EXPORTACIÓN DE PESOS
// ============================================

bool Exportador::exportar_pesos(
    const std::string& archivo,
    const Eigen::VectorXd& pesos,
    double epsilon) {
    
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
    out << "indice,peso\n";
    out << std::fixed << std::setprecision(6);
    
    // Datos
    for (int i = 0; i < pesos.size(); ++i) {
        out << i << "," << pesos(i) << "\n";
    }
    
    out.close();
    std::cout << "[OK] Pesos exportados a: " << archivo << std::endl;
    std::cout << "     Epsilon: " << epsilon << std::endl;
    std::cout << "     Pesos: " << pesos.size() << std::endl;
    std::cout << "     Rango: [" << pesos.minCoeff() << ", " << pesos.maxCoeff() << "]" << std::endl;
    return true;
}

// ============================================
// EXPORTACIÓN DE SUPERFICIES
// ============================================

bool Exportador::exportar_superficie(
    const std::string& archivo,
    const Eigen::MatrixXd& malla,
    const Eigen::VectorXd& z_predicho,
    double epsilon) {
    
    // Verificar dimensiones
    if (malla.rows() != z_predicho.size()) {
        std::cerr << "[ERROR] Dimensiones incompatibles: malla=" 
                  << malla.rows() << ", z_predicho=" << z_predicho.size() << std::endl;
        return false;
    }
    
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
    out << "x,y,z_predicho\n";
    out << std::fixed << std::setprecision(6);
    
    // Datos
    for (int i = 0; i < malla.rows(); ++i) {
        out << malla(i, 0) << ","
            << malla(i, 1) << ","
            << z_predicho(i) << "\n";
    }
    
    out.close();
    std::cout << "[OK] Superficie exportada a: " << archivo << std::endl;
    std::cout << "     Epsilon: " << epsilon << std::endl;
    std::cout << "     Puntos de malla: " << malla.rows() << std::endl;
    std::cout << "     Rango z: [" << z_predicho.minCoeff() << ", " << z_predicho.maxCoeff() << "]" << std::endl;
    return true;
}

```

---

## 4. Ejemplos de Archivos CSV Generados

### 4.1 `datos_entrenamiento.csv`

```csv
x,y,z
-1.234567,0.567890,-0.823456
0.987654,-1.123456,0.456789
-0.456789,1.789012,-0.234567
...
```

**Características:**
- 100 filas de datos + 1 header
- Coordenadas en rango [-2, 2]
- Valores z = sin(x)cos(y) + ruido gaussiano (σ=0.1)

### 4.2 `pesos_eps_1.50.csv`

```csv
indice,peso
0,0.123456
1,-0.234567
2,0.345678
...
```

**Características:**
- 100 filas de datos + 1 header
- Índices de 0 a 99
- Pesos pueden ser positivos o negativos
- Magnitud típica: [-10, 10]

### 4.3 `superficie_eps_1.50.csv`

```csv
x,y,z_predicho
-2.000000,-2.000000,0.123456
-1.918367,-2.000000,0.234567
-1.836735,-2.000000,0.345678
...
```

**Características:**
- 2500 filas de datos + 1 header
- Malla regular 50×50 en [-2, 2]²
- Paso: (2 - (-2)) / (50 - 1) ≈ 0.0816
- Valores z_predicho en rango [-1.5, 1.5] aproximadamente


---

## 5. Criterios de Aceptación

| Criterio | Verificación |
|----------|--------------|
| **Datos entrenamiento** | `datos_entrenamiento.csv` tiene 100 filas |
| **Pesos** | Cada archivo `pesos_eps_X.csv` tiene 100 filas |
| **Superficies** | Cada archivo `superficie_eps_X.csv` tiene 2500 filas |
| **Resumen** | `resumen_comparativo.csv` tiene 3 filas | Sí |
| **Compatibilidad pandas** | `pd.read_csv()` funciona sin errores |

---

## 6. Checklist de Implementación

- [ ] Crear archivo `src/exportador.hpp`
- [ ] Crear archivo `src/exportador.cpp`

---

## 7. Actualización según código (`exportador.cpp`)

- El código real usa mensajes de log concisos (no imprime `epsilon` ni rango de pesos en cada exportación).
- No existe generación de `resumen_comparativo.csv` desde este módulo; `main.cpp` lo referencia solo para listado de archivos generados.
- El struct `MetricasEscenario` está definido en `main.cpp`, no en `exportador.hpp`.

---

**Fin del documento SPEC-004.md**

---

