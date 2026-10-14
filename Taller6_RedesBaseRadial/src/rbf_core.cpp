#include "rbf_core.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <filesystem>
#include <Eigen/Eigenvalues>

RBFCore::RBFCore(double epsilon, double lambda)
    : epsilon_(epsilon), lambda_(lambda) {}

double RBFCore::kernel(double r) const {
    double arg = epsilon_ * r;
    return std::exp(-(arg * arg));
}

double RBFCore::distancia(const Eigen::MatrixXd& puntos, int i, int j) const {
    double dx = puntos(i, 0) - puntos(j, 0);
    double dy = puntos(i, 1) - puntos(j, 1);
    return std::sqrt(dx * dx + dy * dy);
}

Eigen::MatrixXd RBFCore::construir_Phi(const Eigen::MatrixXd& puntos) const {
    int N = puntos.rows();
    Eigen::MatrixXd Phi(N, N);

    std::cout << "  Construyendo matriz de Gram Phi (" << N << "x" << N << ")..." << std::endl;
    std::cout << "    Epsilon: " << epsilon_ << std::endl;

    for (int i = 0; i < N; ++i) {
        Phi(i, i) = 1.0;
        for (int j = i + 1; j < N; ++j) {
            double r = distancia(puntos, i, j);
            double phi_ij = kernel(r);
            Phi(i, j) = phi_ij;
            Phi(j, i) = phi_ij;
        }
    }

    std::cout << "    Diagonal min: " << Phi.diagonal().minCoeff() 
              << " (esperado: 1.0)" << std::endl;
    std::cout << "    Simetría error: " << (Phi - Phi.transpose()).norm() 
              << " (esperado: ~0)" << std::endl;

    return Phi;
}

Eigen::MatrixXd RBFCore::regularizar(const Eigen::MatrixXd& Phi) const {
    int N = Phi.rows();
    Eigen::MatrixXd Phi_reg = Phi;

    std::cout << "  Aplicando regularización de Tikhonov (lambda = " 
              << lambda_ << ")..." << std::endl;

    for (int i = 0; i < N; ++i) {
        Phi_reg(i, i) += lambda_;
    }

    return Phi_reg;
}

Eigen::VectorXd RBFCore::resolver(const Eigen::MatrixXd& Phi_reg, 
                                   const Eigen::VectorXd& y) const {
    std::cout << "  Resolviendo sistema lineal (Phi + lambda*I)w = y..." << std::endl;

    Eigen::LDLT<Eigen::MatrixXd> solver(Phi_reg);
    
    if (solver.info() != Eigen::Success) {
        std::cerr << "[ERROR] La matriz no se pudo descomponer con LDLT" << std::endl;
        std::cerr << "  Intentando con ColPivHouseholderQR..." << std::endl;
        
        Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qr_solver(Phi_reg);
        if (qr_solver.info() != Eigen::Success) {
            std::cerr << "[ERROR] QR también falló" << std::endl;
            return Eigen::VectorXd();
        }
        return qr_solver.solve(y);
    }

    Eigen::VectorXd w = solver.solve(y);

    Eigen::VectorXd residuo = Phi_reg * w - y;
    double error_relativo = residuo.norm() / y.norm();
    
    std::cout << "    Solver exitoso (LDLT)" << std::endl;
    std::cout << "    Error relativo del residuo: " << error_relativo << std::endl;
    std::cout << "    Rango de pesos: [" << w.minCoeff() << ", " << w.maxCoeff() << "]" << std::endl;

    return w;
}

double RBFCore::calcular_condicion(const Eigen::MatrixXd& A) const {
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(A);
    
    if (solver.info() != Eigen::Success) {
        std::cerr << "[WARNING] No se pudieron calcular los valores propios" << std::endl;
        return std::numeric_limits<double>::infinity();
    }

    Eigen::VectorXd eigenvalues = solver.eigenvalues();
    double lambda_max = eigenvalues.maxCoeff();
    double lambda_min = eigenvalues.minCoeff();

    if (std::abs(lambda_min) < 1e-15) {
        std::cerr << "[WARNING] Valor propio mínimo cercano a cero: " 
                  << lambda_min << std::endl;
        return std::numeric_limits<double>::infinity();
    }

    double condicion = std::abs(lambda_max / lambda_min);
    std::cout << "    Valores propios: min=" << lambda_min 
              << ", max=" << lambda_max << std::endl;
    std::cout << "    Número de condición: " << condicion << std::endl;

    return condicion;
}

bool RBFCore::exportar_pesos(const std::string& archivo, 
                              const Eigen::VectorXd& pesos) {
    std::filesystem::path path(archivo);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream out(archivo);
    if (!out.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir: " << archivo << std::endl;
        return false;
    }

    out << "indice,peso\n";
    out << std::fixed << std::setprecision(6);

    for (int i = 0; i < pesos.size(); ++i) {
        out << i << "," << pesos(i) << "\n";
    }

    out.close();
    std::cout << "[OK] Pesos exportados a: " << archivo << std::endl;
    std::cout << "     Filas: " << pesos.size() << " (+ header)" << std::endl;
    return true;
}