/**
 * @file test_reconstruccion_exacta.cpp
 * @brief Test unitario: verifica que la reconstrucción SVD con k = rango completo
 *        reproduce la matriz original con error < 1e-10 (criterio SPEC-000 §12 #3).
 *
 * Usa una matriz sintética 512×512 de rango completo para garantizar
 * reproducibilidad sin depender de archivos externos.
 */
#include <iostream>
#include <iomanip>
#include <cmath>
#include <Eigen/Dense>
#include <Eigen/SVD>

int main() {
    constexpr int M = 512;
    constexpr int N = 512;

    // ── 1. Matriz sintética de rango completo ─────────────────────
    Eigen::MatrixXd A = Eigen::MatrixXd::Random(M, N);   // valores en [-1, 1]
    double norma_original = A.norm();

    std::cout << "[TEST] Iniciando prueba de reconstruccion exacta...\n";
    std::cout << "       Dimension: " << M << "x" << N << "\n";
    std::cout << "       Rango esperado: " << std::min(M, N) << "\n\n";

    // ── 2. SVD completa con BDCSVD ────────────────────────────────
    Eigen::BDCSVD<Eigen::MatrixXd> svd(
        A,
        Eigen::ComputeFullU | Eigen::ComputeFullV
    );

    Eigen::VectorXd sigma = svd.singularValues();
    Eigen::MatrixXd U      = svd.matrixU();
    Eigen::MatrixXd Vt     = svd.matrixV().transpose();
    int rango = static_cast<int>(sigma.size());

    std::cout << "[TEST] SVD calculada. Valores singulares:\n";
    std::cout << "       sigma[0]   = " << std::scientific << sigma(0)   << "\n";
    std::cout << "       sigma[1]   = " << sigma(1)   << "\n";
    std::cout << "       sigma[r-1] = " << sigma(rango - 1)
              << std::fixed << "  (r = " << rango << ")\n\n";

    // ── 3. Reconstruir con k = rango completo ────────────────────
    int k = rango;

    Eigen::MatrixXd U_k  = U.leftCols(k);
    Eigen::VectorXd s_k  = sigma.head(k);
    Eigen::MatrixXd Vt_k = Vt.topRows(k);

    Eigen::MatrixXd A_k = U_k * s_k.asDiagonal() * Vt_k;

    // ── 4. Error relativo en norma de Frobenius ───────────────────
    double error_relativo = (A - A_k).norm() / norma_original;

    constexpr double UMBRAL = 1e-10;
    bool paso = error_relativo < UMBRAL;

    // ── 5. Reporte ────────────────────────────────────────────────
    std::cout << std::scientific;
    std::cout << "[TEST] Reconstruccion exacta k=r: error = " << std::setprecision(6)
              << error_relativo << "\n";
    std::cout << "[TEST] Umbral exigido: < " << UMBRAL << "\n";
    std::cout << std::fixed;
    std::cout << "[TEST] Resultado: "
              << (paso ? "PASS" : "FAIL") << "\n\n";

    if (paso) {
        std::cout << ">>> El criterio de aceptacion SPEC-000 #3 se CUMPLE.\n";
        std::cout << "    La reconstruccion con k = r produce error < 1e-10.\n";
    } else {
        std::cerr << ">>> FALLO: error " << std::scientific << error_relativo
                  << " supera el umbral " << UMBRAL << std::fixed << "\n";
    }

    return paso ? 0 : 1;
}
