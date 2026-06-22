// solver.hpp
#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <Eigen/IterativeLinearSolvers>

class Solver {
public:
    Solver(double tolerancia = 1e-10, int max_iter = 1000);

    Eigen::VectorXd resolver(const Eigen::SparseMatrix<double>& A, 
                            const Eigen::VectorXd& b);

    double calcular_residuo(const Eigen::SparseMatrix<double>& A, 
                           const Eigen::VectorXd& u, 
                           const Eigen::VectorXd& b);

    int get_iteraciones() const;
    double get_error_final() const;
    bool convergencia() const;

private:
    double tolerancia_;
    int max_iter_;
    int iteraciones_;
    double error_final_;
    bool convergencia_;
};
#endif