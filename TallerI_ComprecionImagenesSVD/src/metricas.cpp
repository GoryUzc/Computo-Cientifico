#include "metricas.hpp"
#include <cmath>
#include <fstream>
#include <iomanip>

double normaFrobenius(const Eigen::MatrixXd& A) {
    return A.norm();
}

double calcularErrorRelativo(const Eigen::MatrixXd& A, const Eigen::MatrixXd& Ak) {
    double norma_original = normaFrobenius(A);
    if (norma_original < 1e-15) return 0.0;
    return normaFrobenius(A - Ak) / norma_original;
}

double calcularEnergia(const Eigen::VectorXd& sigma, int k) {
    int r = static_cast<int>(sigma.size());
    k = std::min(k, r);
    double suma_total = 0.0;
    double suma_k = 0.0;
    for (int i = 0; i < r; ++i) {
        double s2 = sigma[i] * sigma[i];
        suma_total += s2;
        if (i < k) suma_k += s2;
    }
    if (suma_total < 1e-15) return 100.0;
    return (suma_k / suma_total) * 100.0;
}

double calcularTasaCompresion(int m, int n, int k) {
    double original  = static_cast<double>(m * n);
    double comprimido = static_cast<double>(k * (m + n + 1));
    if (comprimido < 1e-15) return std::numeric_limits<double>::infinity();
    return original / comprimido;
}

std::vector<MetricasCompresion> calcularTodasLasMetricas(
    const Eigen::MatrixXd& A,
    const Eigen::VectorXd& sigma,
    const std::vector<int>& valores_k
) {
    int m = static_cast<int>(A.rows());
    int n = static_cast<int>(A.cols());
    int r = static_cast<int>(sigma.size());
    std::vector<MetricasCompresion> resultado;
    for (int k : valores_k) {
        k = std::min(k, r);
        MetricasCompresion mtr;
        mtr.k = k;
        mtr.energia_pct     = calcularEnergia(sigma, k);
        mtr.tasa_compresion = calcularTasaCompresion(m, n, k);
        mtr.error_relativo  = 0.0; // se completa después con reconstrucción
        resultado.push_back(mtr);
    }
    return resultado;
}

void completarErrorRelativo(
    std::vector<MetricasCompresion>& metricas,
    const Eigen::MatrixXd& A,
    const std::vector<Eigen::MatrixXd>& reconstrucciones
) {
    for (size_t i = 0; i < metricas.size(); ++i) {
        metricas[i].error_relativo = calcularErrorRelativo(A, reconstrucciones[i]);
    }
}

void guardarMetricasCSV(const std::string& ruta, const std::vector<MetricasCompresion>& metricas) {
    std::ofstream archivo(ruta);
    if (!archivo.is_open()) {
        throw std::runtime_error("No se pudo abrir archivo para guardar métricas: " + ruta);
    }
    archivo << "k,error_relativo,energia_pct,tasa_compresion\n";
    archivo << std::fixed << std::setprecision(10);
    for (const auto& m : metricas) {
        archivo << m.k << ","
                << m.error_relativo << ","
                << m.energia_pct << ","
                << m.tasa_compresion << "\n";
    }
}

void guardarValoresSingularesCSV(const std::string& ruta, const Eigen::VectorXd& sigma) {
    std::ofstream archivo(ruta);
    if (!archivo.is_open()) {
        throw std::runtime_error("No se pudo abrir archivo para guardar valores singulares: " + ruta);
    }
    archivo << "indice,valor_singular\n";
    for (int i = 0; i < sigma.size(); ++i) {
        archivo << (i + 1) << "," << sigma[i] << "\n";
    }
}
