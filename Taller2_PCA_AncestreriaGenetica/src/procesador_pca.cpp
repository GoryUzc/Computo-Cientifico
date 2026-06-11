#include "procesador_pca.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

static double calcularMedia(int n_snps, int n_individuos, const std::vector<double>& valores, int j) {
    double suma = 0.0;
    for (int i = 0; i < n_individuos; ++i) {
        suma += valores[static_cast<std::size_t>(j) * static_cast<std::size_t>(n_individuos) + i];
    }
    return suma / static_cast<double>(n_individuos);
}

ResultadoFiltrado filtrarSNPsConstantes(int n_snps, int n_individuos, const std::vector<double>& valores) {
    ResultadoFiltrado resultado;
    resultado.n_snps_filtrados = 0;
    std::vector<double> p(n_snps);
    for (int j = 0; j < n_snps; ++j) {
        double suma = 0.0;
        for (int i = 0; i < n_individuos; ++i) {
            suma += valores[static_cast<std::size_t>(j) * static_cast<std::size_t>(n_individuos) + i];
        }
        p[j] = suma / (2.0 * n_individuos);
    }
    for (int j = 0; j < n_snps; ++j) {
        if (p[j] > 0.0 && p[j] < 1.0) {
            resultado.indices_originales.push_back(j);
            resultado.p.push_back(p[j]);
            ++resultado.n_snps_filtrados;
        }
    }
    if (resultado.n_snps_filtrados == 0) {
        throw std::runtime_error("Todos los SNPs son constantes. No hay variacion para analizar.");
    }
    resultado.val_filtrados.resize(static_cast<std::size_t>(resultado.n_snps_filtrados) * static_cast<std::size_t>(n_individuos));
    for (int jf = 0; jf < resultado.n_snps_filtrados; ++jf) {
        int j_orig = resultado.indices_originales[jf];
        for (int i = 0; i < n_individuos; ++i) {
            resultado.val_filtrados[static_cast<std::size_t>(jf) * static_cast<std::size_t>(n_individuos) + i] =
                valores[static_cast<std::size_t>(j_orig) * static_cast<std::size_t>(n_individuos) + i];
        }
    }
    return resultado;
}

ResultadoNormalizado normalizarPatterson(const ResultadoFiltrado& filtrado) {
    ResultadoNormalizado resultado;
    resultado.n_snps_filtrados = filtrado.n_snps_filtrados;
    resultado.n_individuos = static_cast<int>(filtrado.val_filtrados.size()) / filtrado.n_snps_filtrados;
    resultado.p = filtrado.p;
    resultado.g_tilde.resize(filtrado.val_filtrados.size());
    for (int j = 0; j < filtrado.n_snps_filtrados; ++j) {
        double pj = filtrado.p[j];
        double denom = std::sqrt(2.0 * pj * (1.0 - pj));
        if (denom < 1e-15) {
            throw std::runtime_error("Denominador cercano a cero en normalizacion de Patterson.");
        }
        for (int i = 0; i < resultado.n_individuos; ++i) {
            double gij = filtrado.val_filtrados[static_cast<std::size_t>(j) * static_cast<std::size_t>(resultado.n_individuos) + i];
            resultado.g_tilde[static_cast<std::size_t>(j) * static_cast<std::size_t>(resultado.n_individuos) + i] = (gij - 2.0 * pj) / denom;
        }
double media = calcularMedia(filtrado.n_snps_filtrados, resultado.n_individuos, resultado.g_tilde, j);
        if (std::abs(media) > 1e-10) {
            throw std::runtime_error("Normalizacion fallida: media de SNP " + std::to_string(filtrado.indices_originales[j] + 1) +
                                     " es " + std::to_string(media));
        }
    }
    return resultado;
}
