#ifndef EXPORTADOR_HPP
#define EXPORTADOR_HPP

#include <Eigen/Dense>
#include <string>
#include <vector>

class Exportador {
public:
    static bool exportar_datos_entrenamiento(
        const std::string& archivo,
        const Eigen::MatrixXd& puntos,
        const Eigen::VectorXd& valores
    );

    static bool exportar_pesos(
        const std::string& archivo,
        const Eigen::VectorXd& pesos,
        double epsilon
    );

    static bool exportar_superficie(
        const std::string& archivo,
        const Eigen::MatrixXd& malla,
        const Eigen::VectorXd& z_predicho,
        double epsilon
    );
};

#endif // EXPORTADOR_HPP