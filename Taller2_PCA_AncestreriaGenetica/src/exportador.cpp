#include "exportador.hpp"
#include <fstream>
#include <iomanip>
#include <stdexcept>

void exportarPCA(const std::string& ruta,
                 const Eigen::MatrixXd& PC,
                 const std::vector<std::string>& nombres_individuos) {
    std::ofstream archivo(ruta);
    if (!archivo) {
        throw std::runtime_error("No se pudo crear archivo: " + ruta);
    }
    archivo << "Individuo,PC1,PC2\n";
    archivo << std::fixed << std::setprecision(10);
    for (int i = 0; i < PC.rows(); ++i) {
        archivo << nombres_individuos[i] << "," << PC(i, 0) << "," << PC(i, 1) << "\n";
    }
}

void exportarVarianza(const std::string& ruta, double var_pc1, double var_pc2) {
    std::ofstream archivo(ruta);
    if (!archivo) {
        throw std::runtime_error("No se pudo crear archivo: " + ruta);
    }
    archivo << "Componente,VarianzaExplicada\n";
    archivo << std::fixed << std::setprecision(10);
    archivo << "PC1," << var_pc1 << "\n";
    archivo << "PC2," << var_pc2 << "\n";
}
