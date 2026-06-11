#include "svd_calculador.hpp"
#include <Eigen/SVD>
#include <stdexcept>

Eigen::MatrixXd SnpMatFromVector(int n_snps, int n_individuos, const std::vector<double>& data) {
    Eigen::MatrixXd G(n_snps, n_individuos);
    for (int j = 0; j < n_snps; ++j) {
        for (int i = 0; i < n_individuos; ++i) {
            G(j, i) = data[static_cast<std::size_t>(j) * static_cast<std::size_t>(n_individuos) + i];
        }
    }
    return G;
}

Eigen::MatrixXd calcularPCA(const std::vector<double>& g_tilde,
                            int n_snps_filtrados,
                            int n_individuos,
                            Eigen::VectorXd& valores_singulares,
                            double& var_pc1,
                            double& var_pc2) {
    Eigen::MatrixXd G_tilde = SnpMatFromVector(n_snps_filtrados, n_individuos, g_tilde);
    Eigen::MatrixXd Gt = G_tilde.transpose();
    Eigen::BDCSVD<Eigen::MatrixXd> svd(Gt, Eigen::ComputeThinU | Eigen::ComputeThinV);
    valores_singulares = svd.singularValues();
    Eigen::VectorXd sigmas = valores_singulares;
    double suma_cuadrados = sigmas.squaredNorm();
    if (suma_cuadrados < 1e-15) {
        throw std::runtime_error("SVD convergio a matriz nula.");
    }
    var_pc1 = (sigmas(0) * sigmas(0) / suma_cuadrados) * 100.0;
    var_pc2 = (sigmas(1) * sigmas(1) / suma_cuadrados) * 100.0;
    Eigen::MatrixXd U = svd.matrixU();
    Eigen::VectorXd Sigma_vec = sigmas.head(30);
    Eigen::MatrixXd PC = U * Sigma_vec.asDiagonal();
    return PC;
}
