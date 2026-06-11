#ifndef UTILS_HPP
#define UTILS_HPP

#include <Eigen/Dense>
#include <vector>

inline Eigen::Map<Eigen::MatrixXd> vectorAMatrizSNP(std::vector<double>& v, int n_snps, int n_individuos) {
    return Eigen::Map<Eigen::MatrixXd>(v.data(), n_snps, n_individuos);
}

#endif
