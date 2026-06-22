// exportador.hpp
#ifndef EXPORTADOR_HPP
#define EXPORTADOR_HPP

#include <Eigen/Dense>
#include <string>
#include "malla.hpp"

class Exportador {
public:
    static bool exportar_temperatura(const std::string& archivo,
                                      const Malla& malla,
                                      const Eigen::VectorXd& u);

    static bool exportar_temperatura_completa(const std::string& archivo,
                                               const Malla& malla,
                                               const Eigen::VectorXd& u,
                                               double Tupper, double Tinf,
                                               double Tleft, double Tright);
};

#endif