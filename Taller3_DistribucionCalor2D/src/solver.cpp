#include "solver.hpp"
#include <iostream>
#include <Eigen/IterativeLinearSolvers>

Solver::Solver(double tolerancia, int max_iter)
    : tolerancia_(tolerancia),
      max_iter_(max_iter),
      iteraciones_(0),
      error_final_(0.0),
      convergencia_(false) {}

Eigen::VectorXd Solver::resolver(const Eigen::SparseMatrix<double>& A,
                                  const Eigen::VectorXd& b) {
    // 1. Verificar dimensiones compatibles
    if (A.rows() != A.cols()) {
        std::cerr << "[ERROR] Matriz A no es cuadrada: " 
                  << A.rows() << "x" << A.cols() << std::endl;
        return Eigen::VectorXd();
    }
    
    if (A.rows() != b.size()) {
        std::cerr << "[ERROR] Dimensiones incompatibles: A(" 
                  << A.rows() << ") vs b(" << b.size() << ")" << std::endl;
        return Eigen::VectorXd();
    }

    // 2. Verificar que A no esté vacía
    if (A.nonZeros() == 0) {
        std::cerr << "[ERROR] Matriz A está vacía (0 elementos no nulos)" << std::endl;
        return Eigen::VectorXd();
    }

    // 3. Crear e configurar el solver BiCGSTAB con precondicionador IncompleteLUT
    Eigen::BiCGSTAB<Eigen::SparseMatrix<double>, Eigen::IncompleteLUT<double>> solver;
    solver.setTolerance(tolerancia_);
    solver.setMaxIterations(max_iter_);

    // 4. PASO CRÍTICO: Inicializar con la matriz (OBLIGATORIO)
    solver.compute(A);
    
    if (solver.info() != Eigen::Success) {
        std::cerr << "[ERROR] solver.compute(A) falló." << std::endl;
        convergencia_ = false;
        return Eigen::VectorXd();
    }
    
    // 5. Resolver el sistema
    Eigen::VectorXd u = solver.solve(b);
    
    // 6. Guardar estadísticas
    iteraciones_ = solver.iterations();
    error_final_ = solver.error();
    convergencia_ = (solver.info() == Eigen::Success);
    
    if (!convergencia_) {
        std::cerr << "[ADVERTENCIA] El solver no convergió." << std::endl;
        std::cerr << "  Iteraciones: " << iteraciones_ << std::endl;
        std::cerr << "  Error final: " << error_final_ << std::endl;
    } else {
        std::cout << "  Solver convergió en " << iteraciones_ 
                  << " iteraciones (error: " << error_final_ << ")" << std::endl;
    }
    
    return u;
}

double Solver::calcular_residuo(const Eigen::SparseMatrix<double>& A,
                                 const Eigen::VectorXd& u,
                                 const Eigen::VectorXd& b) {
    return (A * u - b).norm();
}

int Solver::get_iteraciones() const { return iteraciones_; }
double Solver::get_error_final() const { return error_final_; }
bool Solver::convergencia() const { return convergencia_; }
