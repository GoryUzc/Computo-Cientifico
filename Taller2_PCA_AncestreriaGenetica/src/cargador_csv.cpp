#include "cargador_csv.hpp"
#include "csv.h"
#include <stdexcept>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

DatosGenotipo cargarCSV(const std::string& ruta) {
    DatosGenotipo datos;

    constexpr int N_COLUMNAS = 101;
    constexpr int N_SNPS = 100;
    // Usar double_quote_escape para manejar comillas como "Individuo_1"
    using ReaderType = io::CSVReader<N_COLUMNAS, io::trim_chars<' ', '\t'>, io::double_quote_escape<',','"'>>;

    ReaderType reader(ruta);

    const int MAX_INDIVIDUOS = 30;
    int n_individuos = 0;
    std::vector<std::string> individuos;
    std::vector<std::vector<int>> filas_raw;

    // Variables para leer la fila
    std::string cols[101];

    while (reader.read_row(
        cols[0], cols[1], cols[2], cols[3], cols[4], cols[5], cols[6], cols[7], cols[8], cols[9],
        cols[10], cols[11], cols[12], cols[13], cols[14], cols[15], cols[16], cols[17], cols[18], cols[19],
        cols[20], cols[21], cols[22], cols[23], cols[24], cols[25], cols[26], cols[27], cols[28], cols[29],
        cols[30], cols[31], cols[32], cols[33], cols[34], cols[35], cols[36], cols[37], cols[38], cols[39],
        cols[40], cols[41], cols[42], cols[43], cols[44], cols[45], cols[46], cols[47], cols[48], cols[49],
        cols[50], cols[51], cols[52], cols[53], cols[54], cols[55], cols[56], cols[57], cols[58], cols[59],
        cols[60], cols[61], cols[62], cols[63], cols[64], cols[65], cols[66], cols[67], cols[68], cols[69],
        cols[70], cols[71], cols[72], cols[73], cols[74], cols[75], cols[76], cols[77], cols[78], cols[79],
        cols[80], cols[81], cols[82], cols[83], cols[84], cols[85], cols[86], cols[87], cols[88], cols[89],
        cols[90], cols[91], cols[92], cols[93], cols[94], cols[95], cols[96], cols[97], cols[98], cols[99],
        cols[100])) {

        // Saltar header: la primera fila tiene columnas con nombres SNP_X
        // Detectamos si cols[1] es "SNP_1" (el header)
        if (!cols[1].empty() && cols[1] == "SNP_1") {
            continue;
        }

        if (n_individuos >= MAX_INDIVIDUOS) {
            throw std::runtime_error("El CSV contiene mas de 30 individuos.");
        }

        std::string nombre_ind = cols[0];

        std::vector<int> snp_vals(N_SNPS);

        for (int j = 0; j < N_SNPS; ++j) {
            std::istringstream iss(cols[j + 1]);
            int val;
            if (!(iss >> val)) {
                std::cerr << "DEBUG: cols[" << (j + 1) << "] = '" << cols[j + 1] << "'" << std::endl;
                throw std::runtime_error("Valor no entero en SNP_" + std::to_string(j + 1) +
                                        ", individuo '" + nombre_ind + "'");
            }
            if (val != 0 && val != 1 && val != 2) {
                throw std::runtime_error("Valor invalido en SNP_" + std::to_string(j + 1) +
                                        ", individuo " + nombre_ind + ": " + std::to_string(val));
            }
            snp_vals[j] = val;
        }

        individuos.push_back(nombre_ind);
        filas_raw.push_back(std::move(snp_vals));
        ++n_individuos;
    }

    if (n_individuos == 0) {
        throw std::runtime_error("El archivo CSV esta vacio o no se pudo leer.");
    }

    // Configurar estructura de salida
    datos.n_individuos = n_individuos;
    datos.n_snps = N_SNPS;
    datos.nombres_individuos = std::move(individuos);
    datos.nombres_snps.reserve(N_SNPS);
    for (int j = 0; j < N_SNPS; ++j) {
        datos.nombres_snps.push_back("SNP_" + std::to_string(j + 1));
    }

    // Convertir a vector plano (column-major para Eigen)
    datos.valores.resize(static_cast<std::size_t>(N_SNPS) * static_cast<std::size_t>(n_individuos));
    for (int i = 0; i < n_individuos; ++i) {
        for (int j = 0; j < N_SNPS; ++j) {
            datos.valores[static_cast<std::size_t>(j) * static_cast<std::size_t>(n_individuos) + i] =
                static_cast<double>(filas_raw[i][j]);
        }
    }

    return datos;
}