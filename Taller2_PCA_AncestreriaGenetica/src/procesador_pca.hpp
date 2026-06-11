#ifndef PROCESADOR_PCA_HPP
#define PROCESADOR_PCA_HPP

#include <vector>
#include <string>

struct ResultadoFiltrado {
    std::vector<double> val_filtrados;
    std::vector<double> p;
    std::vector<int> indices_originales;
    int n_snps_filtrados;
};

struct ResultadoNormalizado {
    std::vector<double> g_tilde;
    int n_snps_filtrados;
    int n_individuos;
    std::vector<double> p;
};

ResultadoFiltrado filtrarSNPsConstantes(int n_snps, int n_individuos, const std::vector<double>& valores);
ResultadoNormalizado normalizarPatterson(const ResultadoFiltrado& filtrado);

#endif
