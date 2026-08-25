#include "pagerank_core.hpp"

#include <chrono>
#include <cmath>
#include <stdexcept>

PageRank::PageRank(const Eigen::MatrixXd& transicion, double d)
    : transicion_(transicion), d_(d), n_(static_cast<int>(transicion.rows())) {
    if (transicion_.rows() != transicion_.cols()) {
        throw std::invalid_argument("La matriz de transicion debe ser cuadrada");
    }
    if (n_ <= 0) {
        throw std::invalid_argument("La matriz de transicion debe tener dimension positiva");
    }
}

Eigen::MatrixXd PageRank::construir_matriz_google() {
    Eigen::MatrixXd J = Eigen::MatrixXd::Ones(n_, n_);

    // G = d·M + (1-d)/n · J
    google_ = d_ * transicion_ + (1.0 - d_) / n_ * J;

    return google_;
}

ResultadoPageRank PageRank::ejecutar(double tolerancia, int max_iter) {
    ResultadoPageRank resultado;
    resultado.convergio = false;
    resultado.iteraciones = 0;
    resultado.error_final = 0.0;
    resultado.historial_error.reserve(max_iter + 1);
    resultado.historial_pagerank.reserve(max_iter + 1);

    // 1. Vector inicial uniforme
    Eigen::VectorXd v = Eigen::VectorXd::Constant(n_, 1.0 / n_);
    Eigen::VectorXd v_anterior;

    auto inicio = std::chrono::high_resolution_clock::now();

    // 2. Guardar estado inicial (requerido por SPEC-002 para índice de iteración)
    resultado.historial_error.push_back(0.0);
    resultado.historial_pagerank.push_back(v);

    // 3. Iterar método de potencias
    for (int k = 0; k < max_iter; ++k) {
        v_anterior = v;
        v = google_ * v;

        // 4. Verificar convergencia
        double error = (v - v_anterior).norm();
        resultado.historial_error.push_back(error);
        resultado.historial_pagerank.push_back(v);

        resultado.iteraciones = k + 1;
        resultado.error_final = error;

        if (error < tolerancia) {
            resultado.convergio = true;
            break;
        }
    }

    // 5. Normalizar (corrección por punto flotante)
    v = v / v.sum();

    auto fin = std::chrono::high_resolution_clock::now();
    resultado.tiempo_ejecucion = std::chrono::duration<double>(fin - inicio).count();
    resultado.pagerank = v;

    return resultado;
}

int PageRank::get_n_paginas() const {
    return n_;
}

double PageRank::get_factor_amortiguamiento() const {
    return d_;
}

const Eigen::MatrixXd& PageRank::get_matriz_google() const {
    return google_;
}

const Eigen::MatrixXd& PageRank::get_matriz_transicion() const {
    return transicion_;
}

bool PageRank::validar_matriz_transicion() const {
    Eigen::VectorXd sumas = calcular_suma_columnas();

    for (int j = 0; j < n_; ++j) {
        double suma = sumas(j);
        // Columna válida (suma = 1) o sumidero (suma = 0).
        // Tolerar exacto 1.0 o exacto 0.0; usar > 1e-6 para evitar
        // falsos positivos por punto flotante en sumideros.
        if (std::abs(suma - 1.0) > 1e-6 && suma > 1e-6) {
            return false;
        }
    }

    return true;
}

Eigen::VectorXd PageRank::calcular_suma_columnas() const {
    return transicion_.colwise().sum();
}
