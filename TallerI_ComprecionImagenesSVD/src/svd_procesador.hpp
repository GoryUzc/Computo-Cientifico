#ifndef SVD_PROCESADOR_HPP
#define SVD_PROCESADOR_HPP

#include <Eigen/Dense>
#include <Eigen/SVD>
#include <vector>
#include <string>

/**
 * @brief Resultado completo de la descomposición SVD de una imagen.
 *
 * A = U * Sigma * Vt, donde Sigma es un vector de valores singulares (no la matriz diagonal completa).
 */
struct ResultadoSVD {
    Eigen::MatrixXd U;       ///< Matriz izquierda (m x r)
    Eigen::VectorXd sigma;   ///< Valores singulares ordenados descendentemente
    Eigen::MatrixXd Vt;      ///< Matriz derecha transpuesta (r x n)
    int rango;               ///< Rango efectivo = número de valores singulares
};

/**
 * @brief Calcula la descomposición SVD completa de una matriz A usando Eigen::BDCSVD.
 *
 * BDCSVD (bidiagonalización divide-and-conquer) está recomendado para matrices grandes (> 100x100).
 * Calcula U completo y V completo (ComputeFullU | ComputeFullV).
 */
ResultadoSVD calcularSVD(const Eigen::MatrixXd& A);

/**
 * @brief Reconstruye la aproximación de rango-k: A_k = U[:,:k] * diag(sigma[:k]) * Vt[:k,:]
 *
 * @param resultado  Resultado SVD precomputado
 * @param k          Rango de la aproximación (k <= rango)
 * @return Matriz Eigen::MatrixXd con la imagen aproximada
 */
Eigen::MatrixXd reconstruirRangoK(const ResultadoSVD& resultado, int k);

/**
 * @brief Versión para imagen a color: procesa cada canal independientemente.
 */
std::vector<Eigen::MatrixXd> reconstruirRangoKColor(
    const std::vector<ResultadoSVD>& svdPorCanal,
    int k
);

#endif // SVD_PROCESADOR_HPP
