#ifndef PAGERANK_CORE_HPP
#define PAGERANK_CORE_HPP

#include <Eigen/Dense>
#include <vector>
#include <string>

struct ResultadoPageRank {
    Eigen::VectorXd pagerank;              // Vector final de PageRank
    int iteraciones;                        // Iteraciones hasta convergencia
    double error_final;                     // ||v_{k+1} - v_k||₂
    bool convergio;                         // true si convergió antes de max_iter
    double tiempo_ejecucion;                // Tiempo en segundos
    std::vector<double> historial_error;    // Error por iteración (para gráfica)
    std::vector<Eigen::VectorXd> historial_pagerank;  // Vector en cada iteración
};

class PageRank {
public:
    // Constructor con matriz de transición M (ya normalizada) y factor de amortiguamiento
    PageRank(const Eigen::MatrixXd& transicion, double d = 0.85);
    
    // Construir matriz de Google G = d·M + (1-d)/n · J
    Eigen::MatrixXd construir_matriz_google();
    
    // Ejecutar método de potencias
    ResultadoPageRank ejecutar(double tolerancia = 1e-8, int max_iter = 1000);
    
    // Getters
    int get_n_paginas() const;
    double get_factor_amortiguamiento() const;
    const Eigen::MatrixXd& get_matriz_google() const;
    const Eigen::MatrixXd& get_matriz_transicion() const;
    
    // Validaciones
    bool validar_matriz_transicion() const;
    Eigen::VectorXd calcular_suma_columnas() const;

private:
    Eigen::MatrixXd transicion_;     // Matriz de transición M (entrada)
    Eigen::MatrixXd google_;         // Matriz de Google G (construida)
    double d_;                        // Factor de amortiguamiento
    int n_;                           // Número de páginas
};

#endif
