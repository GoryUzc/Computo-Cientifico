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
        std::cout << "    Convergencia: " << (resultado.convergio ? "Sí" : "No") << std::endl;
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
