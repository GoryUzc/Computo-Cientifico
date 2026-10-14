#ifndef RBF_CORE_HPP
#define RBF_CORE_HPP

#include <Eigen/Dense>
#include <string>

class RBFCore {
public:
    RBFCore(double epsilon, double lambda);

    Eigen::MatrixXd construir_Phi(const Eigen::MatrixXd& puntos) const;
    Eigen::MatrixXd regularizar(const Eigen::MatrixXd& Phi) const;
    Eigen::VectorXd resolver(const Eigen::MatrixXd& Phi_reg, 
                             const Eigen::VectorXd& y) const;
    double calcular_condicion(const Eigen::MatrixXd& A) const;
    static bool exportar_pesos(const std::string& archivo, 
                               const Eigen::VectorXd& pesos);

    double get_epsilon() const { return epsilon_; }
    double get_lambda() const { return lambda_; }

private:
    double epsilon_;
    double lambda_;

    double kernel(double r) const;
    double distancia(const Eigen::MatrixXd& puntos, int i, int j) const;
};

#endif // RBF_CORE_HPP