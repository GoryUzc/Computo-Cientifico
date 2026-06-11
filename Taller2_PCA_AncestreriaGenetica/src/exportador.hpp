#ifndef EXPORTADOR_HPP
#define EXPORTADOR_HPP

#include <string>
#include <vector>
#include <Eigen/Dense>

void exportarPCA(const std::string& ruta,
                 const Eigen::MatrixXd& PC,
                 const std::vector<std::string>& nombres_individuos);

void exportarVarianza(const std::string& ruta, double var_pc1, double var_pc2);

#endif
