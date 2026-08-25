#include "exportador.hpp"

#include <fstream>
#include <iomanip>
#include <cmath>

namespace {
    const double EPS_CERO = 1e-10;
}

std::vector<int> Exportador::calcular_grado_entrada(const Eigen::MatrixXd& M) {
    const int n = static_cast<int>(M.rows());
    std::vector<int> grado(n, 0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (std::abs(M(i, j)) > EPS_CERO) {
                grado[i]++;
            }
        }
    }
    return grado;
}

std::vector<int> Exportador::calcular_grado_salida(const Eigen::MatrixXd& M) {
    const int n = static_cast<int>(M.cols());
    std::vector<int> grado(n, 0);
    for (int j = 0; j < n; ++j) {
        for (int i = 0; i < n; ++i) {
            if (std::abs(M(i, j)) > EPS_CERO) {
                grado[j]++;
            }
        }
    }
    return grado;
}

bool Exportador::exportar_pagerank(const std::string& archivo,
                                   const ResultadoPageRank& resultado,
                                   const Eigen::MatrixXd& transicion) {
    std::ofstream f(archivo);
    if (!f.is_open()) return false;

    const int n = static_cast<int>(resultado.pagerank.size());
    auto entrada = calcular_grado_entrada(transicion);
    auto salida = calcular_grado_salida(transicion);

    f << std::fixed << std::setprecision(6);
    f << "pagina,pagerank,grado_entrada,grado_salida\n";
    for (int i = 0; i < n; ++i) {
        f << i << "," << resultado.pagerank(i) << ","
          << entrada[i] << "," << salida[i] << "\n";
    }
    return true;
}

bool Exportador::exportar_convergencia(const std::string& archivo,
                                       const ResultadoPageRank& resultado) {
    std::ofstream f(archivo);
    if (!f.is_open()) return false;

    const int n = static_cast<int>(resultado.pagerank.size());

    f << std::fixed << std::setprecision(6);
    f << "iteracion,error_norma";
    for (int i = 0; i < n; ++i) {
        f << ",pagerank_" << i;
    }
    f << "\n";

    const int num_iter = static_cast<int>(resultado.historial_error.size());
    for (int k = 0; k < num_iter; ++k) {
        f << k << "," << resultado.historial_error[k];
        const auto& vk = resultado.historial_pagerank[k];
        for (int i = 0; i < n; ++i) {
            f << "," << vk(i);
        }
        f << "\n";
    }
    return true;
}

bool Exportador::exportar_matriz_google(const std::string& archivo,
                                        const Eigen::MatrixXd& G) {
    return exportar_matriz_larga(archivo, G);
}

bool Exportador::exportar_matriz_transicion(const std::string& archivo,
                                            const Eigen::MatrixXd& M) {
    return exportar_matriz_larga(archivo, M);
}

bool Exportador::exportar_matriz_larga(const std::string& archivo,
                                       const Eigen::MatrixXd& matriz) {
    std::ofstream f(archivo);
    if (!f.is_open()) return false;

    const int n = static_cast<int>(matriz.rows());
    f << std::fixed << std::setprecision(6);
    f << "fila,columna,valor\n";
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            f << i << "," << j << "," << matriz(i, j) << "\n";
        }
    }
    return true;
}

bool Exportador::exportar_resumen(const std::string& archivo,
                                  const std::vector<ResultadoPageRank>& resultados,
                                  const std::vector<std::string>& nombres_casos) {
    std::ofstream f(archivo);
    if (!f.is_open()) return false;

    f << "caso,descripcion,n_paginas,iteraciones,error_final,convergencia,"
         "tiempo_segundos,pagerank_max,pagerank_min,suma_pagerank\n";

    f << std::fixed;
    const int num = static_cast<int>(resultados.size());
    for (int c = 0; c < num; ++c) {
        const auto& r = resultados[c];
        const int n = static_cast<int>(r.pagerank.size());
        double max_pr = -1e300, min_pr = 1e300, suma = 0.0;
        for (int i = 0; i < n; ++i) {
            const double v = r.pagerank(i);
            if (v > max_pr) max_pr = v;
            if (v < min_pr) min_pr = v;
            suma += v;
        }
        f << (c + 1) << ","
          << nombres_casos[c] << ","
          << n << ","
          << r.iteraciones << ","
          << std::scientific << std::setprecision(6) << r.error_final << ","
          << std::defaultfloat << (r.convergio ? "true" : "false") << ","
          << std::fixed << std::setprecision(6) << r.tiempo_ejecucion << ","
          << max_pr << ","
          << min_pr << ","
          << suma << "\n";
    }
    return true;
}
