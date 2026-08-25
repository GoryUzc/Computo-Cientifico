#ifndef EXPORTADOR_HPP
#define EXPORTADOR_HPP

#include "pagerank_core.hpp"
#include <string>
#include <vector>

class Exportador {
public:
    // Exportar vector de PageRank final (con grados de entrada/salida desde M)
    static bool exportar_pagerank(const std::string& archivo,
                                  const ResultadoPageRank& resultado,
                                  const Eigen::MatrixXd& transicion);
    
    // Exportar historial de convergencia
    static bool exportar_convergencia(const std::string& archivo,
                                      const ResultadoPageRank& resultado);
    
    // Exportar matriz de Google G (formato largo fila,columna,valor)
    static bool exportar_matriz_google(const std::string& archivo,
                                       const Eigen::MatrixXd& G);
    
    // Exportar matriz de transición M (formato largo fila,columna,valor)
    static bool exportar_matriz_transicion(const std::string& archivo,
                                           const Eigen::MatrixXd& M);
    
    // Exportar resumen comparativo de todos los casos
    static bool exportar_resumen(const std::string& archivo,
                                 const std::vector<ResultadoPageRank>& resultados,
                                 const std::vector<std::string>& nombres_casos);

private:
    // Grados de entrada (por fila) y salida (por columna) derivados de M
    static std::vector<int> calcular_grado_entrada(const Eigen::MatrixXd& M);
    static std::vector<int> calcular_grado_salida(const Eigen::MatrixXd& M);

    // Exportar cualquier matriz en formato largo (n×n filas)
    static bool exportar_matriz_larga(const std::string& archivo,
                                      const Eigen::MatrixXd& matriz);
};

#endif
