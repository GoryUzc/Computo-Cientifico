# 📘 SPEC-005.md — Módulo 5: Orquestador Principal (`main.cpp`)

> **Documento de Especificación Técnica**  
> **Proyecto:** Taller 6 — Interpolación RBF  
> **Módulo:** `main.cpp`  
> **Responsabilidad:** Orquestar el flujo completo del proyecto, mostrar progreso en consola y coordinar la exportación de todos los resultados

---

## 1. Propósito del Módulo

Este módulo es el **orquestador central** del proyecto. Su responsabilidad es:

1. **Coordinar** la ejecución secuencial de los 4 módulos previos
2. **Mostrar en consola** el progreso de cada etapa del algoritmo
3. **Iterar sobre los 3 valores de ε** (0.5, 1.5, 5.0)
4. **Calcular métricas comparativas** entre los escenarios
5. **Exportar el resumen final** con todos los resultados

---

## 2. Flujo de Execution (Diagrama)

```
┌─────────────────────────────────────────────────────────────────┐
│                    FLUJO PRINCIPAL (main.cpp)                    │
└─────────────────────────────────────────────────────────────────┘

    [INICIO]
        │
        ▼
┌─────────────────────────────────────┐
│ FASE 1: Generar Datos               │
│ - 100 puntos aleatorios             │
│ - z = sin(x)cos(y) + ruido          │
│ - Exportar datos_entrenamiento.csv  │
└─────────────────────────────────────┘
        │
        ▼
┌─────────────────────────────────────┐
│ FASE 2: Crear Malla de Evaluación   │
│ - 50×50 = 2500 puntos              │
│ - Dominio [-2, 2]²                  │
│ - Exportar funcion_real.csv         │
└─────────────────────────────────────┘
        │
        ▼
┌─────────────────────────────────────────────────────────────┐
│ FASE 3: Bucle sobre ε ∈ {0.5, 1.5, 5.0}                    │
│                                                              │
│   Para cada ε:                                              │
│   ┌────────────────────────────────────────────────────┐   │
│   │ 3a. Construir matriz Φ (100×100)                   │   │
│   │ 3b. Regularizar: Φ + λI                           │   │
│   │ 3c. Resolver sistema → pesos w                    │   │
│   │ 3d. Exportar pesos_eps_X.csv                      │   │
│   │ 3e. Evaluar superficie en malla                   │   │
│   │ 3f. Exportar superficie_eps_X.csv                 │   │
│   │ 3g. Calcular métricas                             │   │
│   └────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
        │
        ▼
┌─────────────────────────────────────┐
│ FASE 4: Exportar Resumen            │
│ - resumen_comparativo.csv           │
│ - Métricas de los 3 escenarios      │
└─────────────────────────────────────┘
        │
        ▼
    [FIN]
```

---

## 3. Especificación de `main.cpp`

### 3.1 Archivo `src/main.cpp`

```cpp
#include <iostream>
#include <iomanip>
#include <chrono>
#include <filesystem>
#include <vector>
#include <string>

#include "generador_datos.hpp"
#include "rbf_core.hpp"
#include "evaluador.hpp"
#include "exportador.hpp"

// Estructura para almacenar métricas de cada escenario
struct MetricasEscenario {
    double epsilon;
    double error_entrenamiento;
    double error_malla;
    double numero_condicion;
    double tiempo_entrenamiento;  // segundos
    double tiempo_evaluacion;     // segundos
};

// Función para imprimir separadores visuales
void imprimir_separador(char c = '=', int longitud = 70) {
    std::cout << std::string(longitud, c) << std::endl;
}

// Función para imprimir encabezado de fase
void imprimir_fase(const std::string& titulo) {
    std::cout << std::endl;
    imprimir_separador('=');
    std::cout << "  " << titulo << std::endl;
    imprimir_separador('=');
}

int main() {

    // CONFIGURACION GLOBAL
    imprimir_fase("TALLER 6: INTERPOLACION MEDIANTE REDES DE BASE RADIAL (RBF)");
    std::cout << "  Funcion objetivo: f(x, y) = sin(x) * cos(y)" << std::endl;
    std::cout << "  Dominio: [-2, 2] x [-2, 2]" << std::endl;
    std::cout << "  Kernel: Gaussiano phi(r) = exp(-(eps * r)^2)" << std::endl;
    std::cout << "  Regularizacion: Tikhonov (lambda = 1e-6)" << std::endl;
    
    // Crear directorio de resultados
    std::filesystem::create_directories("../resultados");
    std::cout << "\n[INFO] Directorio de resultados creado: ../resultados/" << std::endl;
    
    /
    // PARAMETROS
    /
    const int N = 100;                    // Puntos de entrenamiento
    const double min_val = -2.0;          // Límite inferior del dominio
    const double max_val = 2.0;           // Límite superior del dominio
    const double ruido_sigma = 0.1;       // Desviación estándar del ruido
    const unsigned int seed = 42;         // Semilla para reproducibilidad
    const double lambda = 1e-6;           // Parámetro de regularización
    const int resolucion_malla = 50;      // 50x50 = 2500 puntos
    
    // Valores de epsilon a estudiar
    std::vector<double> epsilons = {0.5, 1.5, 5.0};
    std::vector<std::string> etiquetas_eps = {"pequeno", "optimo", "grande"};
    
    std::cout << "\n[CONFIG] Parametros:" << std::endl;
    std::cout << "  - N puntos de entrenamiento: " << N << std::endl;
    std::cout << "  - Ruido gaussiano (sigma): " << ruido_sigma << std::endl;
    std::cout << "  - Regularizacion (lambda): " << lambda << std::endl;
    std::cout << "  - Resolucion de malla: " << resolucion_malla << "x" << resolucion_malla << std::endl;
    std::cout << "  - Epsilon a estudiar: ";
    for (double eps : epsilons) std::cout << eps << " ";
    std::cout << std::endl;
    
    /
    // FASE 1: GENERAR DATOS DE ENTRENAMIENTO
    /
    imprimir_fase("FASE 1: Generacion de Datos de Entrenamiento");
    
    auto inicio_fase1 = std::chrono::high_resolution_clock::now();
    
    GeneradorDatos generador(N, min_val, max_val, ruido_sigma, seed);
    generador.generar();
    
    bool ok_datos = generador.exportar_csv("../resultados/datos_entrenamiento.csv");
    if (!ok_datos) {
        std::cerr << "[ERROR] No se pudieron exportar los datos de entrenamiento" << std::endl;
        return 1;
    }
    
    auto fin_fase1 = std::chrono::high_resolution_clock::now();
    double tiempo_fase1 = std::chrono::duration<double>(fin_fase1 - inicio_fase1).count();
    
    std::cout << "\n[RESULTADO] Archivo generado: resultados/datos_entrenamiento.csv" << std::endl;
    std::cout << "  - Filas: " << N << " (+ header)" << std::endl;
    std::cout << "  - Columnas: x, y, z" << std::endl;
    std::cout << "  - Tiempo de ejecucion: " << std::fixed << std::setprecision(4) 
              << tiempo_fase1 << " s" << std::endl;
    
    // Obtener referencias a los datos
    const auto& puntos = generador.get_puntos();
    const auto& valores = generador.get_valores();
    
    /
    // FASE 2: CREAR MALLA Y FUNCION REAL
    /
    imprimir_fase("FASE 2: Creacion de Malla de Evaluacion");
    
    auto inicio_fase2 = std::chrono::high_resolution_clock::now();
    
    Evaluador evaluador(epsilons[0], resolucion_malla, min_val, max_val);
    Eigen::MatrixXd malla = evaluador.crear_malla();
    
    bool ok_real = evaluador.exportar_funcion_real("../resultados/funcion_real.csv", malla);
    if (!ok_real) {
        std::cerr << "[ERROR] No se pudo exportar la funcion real" << std::endl;
        return 1;
    }
    
    auto fin_fase2 = std::chrono::high_resolution_clock::now();
    double tiempo_fase2 = std::chrono::duration<double>(fin_fase2 - inicio_fase2).count();
    
    std::cout << "\n[RESULTADO] Archivo generado: resultados/funcion_real.csv" << std::endl;
    std::cout << "  - Filas: " << malla.rows() << " (+ header)" << std::endl;
    std::cout << "  - Columnas: x, y, z_real" << std::endl;
    std::cout << "  - Tiempo de ejecucion: " << std::fixed << std::setprecision(4) 
              << tiempo_fase2 << " s" << std::endl;
    
    // FASE 3: BUCLE SOBRE LOS VALORES DE EPSILON
    imprimir_fase("FASE 3: Entrenamiento y Evaluacion RBF (Bucle sobre epsilon)");
    
    std::vector<MetricasEscenario> metricas;
    metricas.reserve(epsilons.size());
    
    for (size_t idx = 0; idx < epsilons.size(); ++idx) {
        double eps = epsilons[idx];
        const std::string& etiqueta = etiquetas_eps[idx];
        
        std::cout << std::endl;
        imprimir_separador('-');
        std::cout << "  Escenario " << (idx + 1) << "/" << epsilons.size() 
                  << ": epsilon = " << eps << " (" << etiqueta << ")" << std::endl;
        imprimir_separador('-');
        
        MetricasEscenario m;
        m.epsilon = eps;
        
        // 3a. Construir matriz Phi
        std::cout << "\n  [3a] Construyendo matriz de Gram Phi..." << std::endl;
        auto inicio_phi = std::chrono::high_resolution_clock::now();
        
        RBFCore rbf(eps, lambda);
        Eigen::MatrixXd Phi = rbf.construir_Phi(puntos);
        
        auto fin_phi = std::chrono::high_resolution_clock::now();
        double tiempo_phi = std::chrono::duration<double>(fin_phi - inicio_phi).count();
        
        std::cout << "    - Dimension: " << Phi.rows() << "x" << Phi.cols() << std::endl;
        std::cout << "    - Tiempo: " << std::fixed << std::setprecision(4) 
                  << tiempo_phi << " s" << std::endl;
        
        // 3b. Regularizar
        std::cout << "\n  [3b] Aplicando regularizacion de Tikhonov..." << std::endl;
        Eigen::MatrixXd Phi_reg = rbf.regularizar(Phi);
        std::cout << "    - Lambda: " << lambda << std::endl;
        
        // 3c. Resolver sistema
        std::cout << "\n  [3c] Resolviendo sistema lineal (Phi + lambda*I)w = y..." << std::endl;
        auto inicio_solve = std::chrono::high_resolution_clock::now();
        
        Eigen::VectorXd w = rbf.resolver(Phi_reg, valores);
        
        auto fin_solve = std::chrono::high_resolution_clock::now();
        m.tiempo_entrenamiento = std::chrono::duration<double>(fin_solve - inicio_solve).count();
        
        std::cout << "    - Dimension de w: " << w.size() << std::endl;
        std::cout << "    - Rango de pesos: [" << w.minCoeff() << ", " << w.maxCoeff() << "]" << std::endl;
        std::cout << "    - Tiempo de resolucion: " << std::fixed << std::setprecision(4) 
                  << m.tiempo_entrenamiento << " s" << std::endl;
        
        // 3d. Calcular numero de condicion
        std::cout << "\n  [3d] Calculando numero de condicion..." << std::endl;
        m.numero_condicion = rbf.calcular_condicion(Phi_reg);
        std::cout << "    - Numero de condicion: " << std::scientific << m.numero_condicion << std::endl;
        
        // 3e. Exportar pesos
        std::cout << "\n  [3e] Exportando pesos..." << std::endl;
        std::string nombre_pesos = "../resultados/pesos_eps_" + std::to_string(eps) + ".csv";
        bool ok_pesos = Exportador::exportar_pesos(nombre_pesos, w, eps);
        if (ok_pesos) {
            std::cout << "    - Archivo generado: resultados/pesos_eps_" << eps << ".csv" << std::endl;
        }
    a
        // 3f. Evaluar superficie en malla
        std::cout << "\n  [3f] Evaluando superficie en malla " 
                  << resolucion_malla << "x" << resolucion_malla << "..." << std::endl;
        
        Evaluador eval_eps(eps, resolucion_malla, min_val, max_val);
        
        auto inicio_eval = std::chrono::high_resolution_clock::now();
        
        Eigen::VectorXd z_predicho = eval_eps.evaluar(malla, puntos, w);
        
        auto fin_eval = std::chrono::high_resolution_clock::now();
        m.tiempo_evaluacion = std::chrono::duration<double>(fin_eval - inicio_eval).count();
        
        std::cout << "    - Puntos evaluados: " << z_predicho.size() << std::endl;
        std::cout << "    - Rango z_predicho: [" << z_predicho.minCoeff() << ", " 
                  << z_predicho.maxCoeff() << "]" << std::endl;
        std::cout << "    - Tiempo de evaluacion: " << std::fixed << std::setprecision(4) 
                  << m.tiempo_evaluacion << " s" << std::endl;
        
        
        // 3g. Calcular errores
        std::cout << "\n  [3g] Calculando errores..." << std::endl;
        
        // Error de entrenamiento: RMSE en los centros
        Eigen::VectorXd z_en_centros = eval_eps.evaluar_en_centros(puntos, w);
        m.error_entrenamiento = Evaluador::calcular_rmse(valores, z_en_centros);
        std::cout << "    - Error de entrenamiento (RMSE): " << std::scientific 
                  << m.error_entrenamiento << std::endl;
        
        // Error de malla: RMSE vs funcion real
        Eigen::VectorXd z_real_malla = Evaluador::funcion_real(malla);
        m.error_malla = Evaluador::calcular_rmse(z_real_malla, z_predicho);
        std::cout << "    - Error de malla vs funcion real (RMSE): " << std::scientific 
                  << m.error_malla << std::endl;
        
        // 3h. Exportar superficie
        std::cout << "\n  [3h] Exportando superficie..." << std::endl;
        std::string nombre_superficie = "../resultados/superficie_eps_" + std::to_string(eps) + ".csv";
        bool ok_superficie = Exportador::exportar_superficie(nombre_superficie, malla, z_predicho, eps);
        if (ok_superficie) {
            std::cout << "    - Archivo generado: resultados/superficie_eps_" << eps << ".csv" << std::endl;
        }
        
        // Guardar métricas
        metricas.push_back(m);
        
        std::cout << "\n  [OK] Escenario epsilon=" << eps << " completado" << std::endl;
    }
    
    // REPORTE FINAL
    imprimir_fase("REPORTE FINAL - RESUMEN DE RESULTADOS");
    
    std::cout << "\n  Escenarios evaluados: " << epsilons.size() << std::endl;
    std::cout << std::endl;
    
    // Tabla de resultado
    std::cout << std::left << std::setw(10) << "Epsilon"
              << std::left << std::setw(18) << "Error Train"
              << std::left << std::setw(18) << "Error Malla"
              << std::left << std::setw(18) << "Num. Condicion"
              << std::left << std::setw(15) << "Tiempo (s)"
              << std::endl;
    imprimir_separador('-');
    
    std::cout << std::scientific << std::setprecision(3);
    for (const auto& m : metricas) {
        std::cout << std::left << std::setw(10) << std::fixed << std::setprecision(2) << m.epsilon
                  << std::left << std::setw(18) << std::scientific << std::setprecision(3) << m.error_entrenamiento
                  << std::left << std::setw(18) << m.error_malla
                  << std::left << std::setw(18) << m.numero_condicion
                  << std::left << std::setw(15) << std::fixed << std::setprecision(4) 
                  << (m.tiempo_entrenamiento + m.tiempo_evaluacion)
                  << std::endl;
    }
    
    std::cout << std::endl;
    imprimir_separador('=');
    
    // LISTADO DE ARCHIVOS GENERADOS
    imprimir_fase("ARCHIVOS GENERADOS");
    
    std::cout << "\n  Directorio: resultados/" << std::endl;
    std::cout << "  ----------------------------------------" << std::endl;
    std::cout << "  1. datos_entrenamiento.csv      (100 puntos con ruido)" << std::endl;
    std::cout << "  2. funcion_real.csv             (2500 puntos sin ruido)" << std::endl;
    for (size_t i = 0; i < epsilons.size(); ++i) {
        std::cout << "  " << (3 + 2*i) << ". pesos_eps_" << std::fixed << std::setprecision(2) 
                  << epsilons[i] << ".csv         (100 pesos)" << std::endl;
        std::cout << "  " << (4 + 2*i) << ". superficie_eps_" << std::fixed << std::setprecision(2) 
                  << epsilons[i] << ".csv    (2500 puntos)" << std::endl;
    }
    std::cout << "  " << (3 + 2*epsilons.size()) << ". resumen_comparativo.csv     (metricas de los 3 escenarios)" << std::endl;
    
    std::cout << "\n  Total de archivos: " << (3 + 2*epsilons.size()) << std::endl;
    
    imprimir_separador('=');
    std::cout << "\n[FIN] Proceso completado exitosamente." << std::endl;
    std::cout << "[INFO] Siguiente paso: ejecutar python/visualizar_rbf.py para generar las graficas." << std::endl;
    
    return 0;
}
```
---

## 5. CMakeLists.txt Actualizado
```cpp
cmake_minimum_required(VERSION 3.16)
project(Taller6_RBF LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Eigen3
find_package(Eigen3 REQUIRED)

# Ejecutable principal (todos los módulos)
add_executable(taller6_rbf
    src/main.cpp
    src/generador_datos.cpp
    src/rbf_core.cpp
    src/evaluador.cpp
    src/exportador.cpp
)

# Enlazar Eigen
target_link_libraries(taller6_rbf PRIVATE Eigen3::Eigen)

# Incluir directorio de headers
target_include_directories(taller6_rbf PRIVATE src)


# Optimizaciones por compilador
if(MSVC)
    target_compile_options(taller6_rbf PRIVATE /W4 /O2)
else()
    target_compile_options(taller6_rbf PRIVATE -Wall -Wextra -O2)
endif()
```
---

## 8. Criterios de Aceptación

| Criterio | Verificación | 
| :--- | :--- |
| Compilación exitosa | `cmake --build .` sin errores |
| Ejecución completa | Todas las 4 fases terminan |
| 9 archivos CSV generados | `ls resultados/` |
| Progreso visible en consola | Mensajes de cada fase |
| Tiempos de ejecución | Cronometraje por fase |
| Tabla de métricas final | Impresa en consola |
| Mensajes de error claros | Si algún archivo falla |

---

## 9. Checklist de Implementación

- [ ] Crear archivo `src/main.cpp` con el código proporcionado
- [ ] Actualizar `CMakeLists.txt` con los 5 módulos
- [ ] Verificar que `generador_datos.hpp/cpp` esté implementado
- [ ] Verificar que `rbf_core.hpp/cpp` esté implementado
- [ ] Verificar que `evaluador.hpp/cpp` esté implementado
- [ ] Verificar que `exportador.hpp/cpp` esté implementado


---

## 10. Notas de actualización (código real vs spec)

- `main.cpp` coincide con el flujo de 4 fases descrito.
- El archivo `resumen_comparativo.csv` se menciona en el listado de salidas y en `main.cpp`, pero **no se escribe realmente** (no hay llamada a `Exportador::exportar_resumen`).
- El `CMakeLists.txt` real incluye `VCPKG_APPLOCAL_DEPS OFF`, `find_package(Eigen3 CONFIG REQUIRED)` y opciones de compilador agresivas (`/O2 /fp:fast` para MSVC; `-O3 -ffast-math -march=native` para GCC/Clang).

---

**Fin del documento SPEC-005.md**
