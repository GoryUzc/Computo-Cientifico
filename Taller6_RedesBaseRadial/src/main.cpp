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

struct MetricasEscenario {
    double epsilon;
    double error_entrenamiento;
    double error_malla;
    double numero_condicion;
    double tiempo_entrenamiento;
    double tiempo_evaluacion;
};

void imprimir_separador(char c = '=', int longitud = 70) {
    std::cout << std::string(longitud, c) << std::endl;
}

void imprimir_fase(const std::string& titulo) {
    std::cout << std::endl;
    imprimir_separador('=');
    std::cout << "  " << titulo << std::endl;
    imprimir_separador('=');
}

int main() {
    imprimir_fase("TALLER 6: INTERPOLACION MEDIANTE REDES DE BASE RADIAL (RBF)");
    std::cout << "  Funcion objetivo: f(x, y) = sin(x) * cos(y)" << std::endl;
    std::cout << "  Dominio: [-2, 2] x [-2, 2]" << std::endl;
    std::cout << "  Kernel: Gaussiano phi(r) = exp(-(eps * r)^2)" << std::endl;
    std::cout << "  Regularizacion: Tikhonov (lambda = 1e-6)" << std::endl;
    
    std::filesystem::create_directories("../resultados");
    std::cout << "\n[INFO] Directorio de resultados creado: ../resultados/" << std::endl;
    
    const int N = 100;
    const double min_val = -2.0;
    const double max_val = 2.0;
    const double ruido_sigma = 0.1;
    const unsigned int seed = 42;
    const double lambda = 1e-6;
    const int resolucion_malla = 50;
    
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
    
    // FASE 1
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
    
    const auto& puntos = generador.get_puntos();
    const auto& valores = generador.get_valores();
    
    // FASE 2
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
    
    // FASE 3
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
        
        // 3a
        std::cout << "\n  [3a] Construyendo matriz de Gram Phi..." << std::endl;
        auto inicio_phi = std::chrono::high_resolution_clock::now();
        RBFCore rbf(eps, lambda);
        Eigen::MatrixXd Phi = rbf.construir_Phi(puntos);
        auto fin_phi = std::chrono::high_resolution_clock::now();
        double tiempo_phi = std::chrono::duration<double>(fin_phi - inicio_phi).count();
        std::cout << "    - Dimension: " << Phi.rows() << "x" << Phi.cols() << std::endl;
        std::cout << "    - Tiempo: " << std::fixed << std::setprecision(4) 
                  << tiempo_phi << " s" << std::endl;
        
        // 3b
        std::cout << "\n  [3b] Aplicando regularizacion de Tikhonov..." << std::endl;
        Eigen::MatrixXd Phi_reg = rbf.regularizar(Phi);
        
        // 3c
        std::cout << "\n  [3c] Resolviendo sistema lineal..." << std::endl;
        auto inicio_solve = std::chrono::high_resolution_clock::now();
        Eigen::VectorXd w = rbf.resolver(Phi_reg, valores);
        auto fin_solve = std::chrono::high_resolution_clock::now();
        m.tiempo_entrenamiento = std::chrono::duration<double>(fin_solve - inicio_solve).count();
        std::cout << "    - Dimension de w: " << w.size() << std::endl;
        std::cout << "    - Rango de pesos: [" << w.minCoeff() << ", " << w.maxCoeff() << "]" << std::endl;
        std::cout << "    - Tiempo de resolucion: " << std::fixed << std::setprecision(4) 
                  << m.tiempo_entrenamiento << " s" << std::endl;
        
        // 3d
        std::cout << "\n  [3d] Calculando numero de condicion..." << std::endl;
        m.numero_condicion = rbf.calcular_condicion(Phi_reg);
        std::cout << "    - Numero de condicion: " << std::scientific << m.numero_condicion << std::endl;
        
        // 3e
        std::cout << "\n  [3e] Exportando pesos..." << std::endl;
        std::string nombre_pesos = "../resultados/pesos_eps_" + std::to_string(eps) + ".csv";
        bool ok_pesos = Exportador::exportar_pesos(nombre_pesos, w, eps);
        if (ok_pesos) {
            std::cout << "    - Archivo generado: resultados/pesos_eps_" << eps << ".csv" << std::endl;
        }
        
        // 3f
        std::cout << "\n  [3f] Evaluando superficie en malla..." << std::endl;
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
        
        // 3g
        std::cout << "\n  [3g] Calculando errores..." << std::endl;
        Eigen::VectorXd z_en_centros = eval_eps.evaluar_en_centros(puntos, w);
        m.error_entrenamiento = Evaluador::calcular_rmse(valores, z_en_centros);
        std::cout << "    - Error de entrenamiento (RMSE): " << std::scientific 
                  << m.error_entrenamiento << std::endl;
        
        Eigen::VectorXd z_real_malla = Evaluador::funcion_real(malla);
        m.error_malla = Evaluador::calcular_rmse(z_real_malla, z_predicho);
        std::cout << "    - Error de malla vs funcion real (RMSE): " << std::scientific 
                  << m.error_malla << std::endl;
        
        // 3h
        std::cout << "\n  [3h] Exportando superficie..." << std::endl;
        std::string nombre_superficie = "../resultados/superficie_eps_" + std::to_string(eps) + ".csv";
        bool ok_superficie = Exportador::exportar_superficie(nombre_superficie, malla, z_predicho, eps);
        if (ok_superficie) {
            std::cout << "    - Archivo generado: resultados/superficie_eps_" << eps << ".csv" << std::endl;
        }
        
        metricas.push_back(m);
        std::cout << "\n  [OK] Escenario epsilon=" << eps << " completado" << std::endl;
    }
    
    // REPORTE FINAL
    imprimir_fase("REPORTE FINAL - RESUMEN DE RESULTADOS");
    
    std::cout << "\n  Escenarios evaluados: " << epsilons.size() << std::endl;
    std::cout << std::endl;
    
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