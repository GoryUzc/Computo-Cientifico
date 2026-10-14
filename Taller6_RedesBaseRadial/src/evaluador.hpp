#ifndef EVALUADOR_HPP
#define EVALUADOR_HPP

#include <Eigen/Dense>
#include <string>

class Evaluador {
public:
    Evaluador(double epsilon, int resolucion = 50, 
              double min_val = -2.0, double max_val = 2.0);

    Eigen::MatrixXd crear_malla() const;
    Eigen::VectorXd evaluar(const Eigen::MatrixXd& malla,
                            const Eigen::MatrixXd& centros,
                            const Eigen::VectorXd& pesos) const;
    Eigen::VectorXd evaluar_en_centros(const Eigen::MatrixXd& centros,
                                        const Eigen::VectorXd& pesos) const;
    static double calcular_rmse(const Eigen::VectorXd& y_real,
                                const Eigen::VectorXd& y_predicho);
    static Eigen::VectorXd funcion_real(const Eigen::MatrixXd& malla);
    static bool exportar_superficie(const std::string& archivo,
                                     const Eigen::MatrixXd& malla,
                                     const Eigen::VectorXd& z_predicho);
    static bool exportar_funcion_real(const std::string& archivo,
                                       const Eigen::MatrixXd& malla);

    double get_epsilon() const { return epsilon_; }
    int get_resolucion() const { return resolucion_; }
    int get_M() const { return resolucion_ * resolucion_; }

private:
    double epsilon_;
    int resolucion_;
    double min_val_;
    double max_val_;

    double kernel(double r) const;
};

#endif // EVALUADOR_HPP