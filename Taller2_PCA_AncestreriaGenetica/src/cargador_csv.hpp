#ifndef CARGADOR_CSV_HPP
#define CARGADOR_CSV_HPP
#include <string>
#include <vector>

#include <cstddef>

struct DatosGenotipo {
    int n_snps;
    int n_individuos;
    std::vector<std::string> nombres_snps;
    std::vector<std::string> nombres_individuos;
    std::vector<double> valores;
};

DatosGenotipo cargarCSV(const std::string& ruta);

#endif
