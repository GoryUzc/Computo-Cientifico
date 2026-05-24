#ifndef METRICAS_HPP
#define METRICAS_HPP

#include <string>
#include <vector>
#include <Eigen/Dense>

/**
 * @brief Contenedor de métricas para un valor de k dado.
 */
struct MetricasCompresion {
    int k;                   ///< Rango de la aproximación
    double error_relativo;   ///< ||A - A_k||_F / ||A||_F
    double energia_pct;      ///< Energía capturada en %
    double tasa_compresion;  ///< (m*n) / (k*(m+n+1))
};

double normaFrobenius(const Eigen::MatrixXd& A);

double calcularErrorRelativo(const Eigen::MatrixXd& A, const Eigen::MatrixXd& Ak);

double calcularEnergia(const Eigen::VectorXd& sigma, int k);

double calcularTasaCompresion(int m, int n, int k);

/**
 * @brief Calcula energía y tasa para cada k. El error_relativo se completa
 *        después de reconstruir, vía completarErrorRelativo().
 */
std::vector<MetricasCompresion> calcularTodasLasMetricas(
    const Eigen::MatrixXd& A,
    const Eigen::VectorXd& sigma,
    const std::vector<int>& valores_k
);

/**
 * @brief Carga el error_relativo en el vector de métricas ya calculadas.
 */
void completarErrorRelativo(
    std::vector<MetricasCompresion>& metricas,
    const Eigen::MatrixXd& A,
    const std::vector<Eigen::MatrixXd>& reconstrucciones
);

void guardarMetricasCSV(const std::string& ruta, const std::vector<MetricasCompresion>& metricas);

void guardarValoresSingularesCSV(const std::string& ruta, const Eigen::VectorXd& sigma);

#endif // METRICAS_HPP
