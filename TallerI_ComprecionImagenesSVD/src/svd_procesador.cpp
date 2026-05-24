#include "svd_procesador.hpp"

ResultadoSVD calcularSVD(const Eigen::MatrixXd& A) {
    Eigen::BDCSVD<Eigen::MatrixXd> svd(
        A,
        Eigen::ComputeFullU | Eigen::ComputeFullV
    );
    ResultadoSVD resultado;
    resultado.U = svd.matrixU();
    resultado.sigma = svd.singularValues();
    resultado.Vt = svd.matrixV().transpose();
    resultado.rango = static_cast<int>(resultado.sigma.size());
    return resultado;
}

Eigen::MatrixXd reconstruirRangoK(const ResultadoSVD& resultado, int k) {
    k = std::min(k, resultado.rango);
    Eigen::MatrixXd U_k = resultado.U.leftCols(k);
    Eigen::VectorXd sigma_k = resultado.sigma.head(k);
    Eigen::MatrixXd Vt_k = resultado.Vt.topRows(k);
    Eigen::MatrixXd A_k = U_k * sigma_k.asDiagonal() * Vt_k;
    return A_k;
}

std::vector<Eigen::MatrixXd> reconstruirRangoKColor(
    const std::vector<ResultadoSVD>& svdPorCanal,
    int k
) {
    std::vector<Eigen::MatrixXd> canales_reconstruidos(3);
    for (int c = 0; c < 3; ++c) {
        canales_reconstruidos[c] = reconstruirRangoK(svdPorCanal[c], k);
    }
    return canales_reconstruidos;
}
