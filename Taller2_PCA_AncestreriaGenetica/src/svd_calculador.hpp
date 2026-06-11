#ifndef SVD_CALCULADOR_HPP
#define SVD_CALCULADOR_HPP

#include <vector>
#include <Eigen/Dense>

Eigen::MatrixXd SnpMatFromVector(int n_snps, int n_individuos, const std::vector<double>& data);
Eigen::MatrixXd calcularPCA(const std::vector<double>& g_tilde,
                          int n_snps_filtrados,
                          int n_individuos,
                          Eigen::VectorXd& valores_singulares,
                          double& var_pc1,
                          double& var_pc2);

#endif
