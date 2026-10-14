#include "evaluador.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <filesystem>

Evaluador::Evaluador(double epsilon, int resolucion, double min_val, double max_val)
    : epsilon_(epsilon), resolucion_(resolucion), 
      min_val_(min_val), max_val_(max_val) {}

double Evaluador::kernel(double r) const {
    double arg = epsilon_ * r;
    return std::exp(-(arg * arg));
}

Eigen::MatrixXd Evaluador::crear_malla() const {
    int M = resolucion_ * resolucion_;
    Eigen::MatrixXd malla(M, 2);
    
    double paso = (max_val_ - min_val_) / (resolucion_ - 1);
    
    std::cout << "  Creando malla " << resolucion_ << "x" << resolucion_ 
              << " = " << M << " puntos" << std::endl;
    std::cout << "    Dominio: [" << min_val_ << ", " << max_val_ << "]^2" << std::endl;
    std::cout << "    Paso: " << paso << std::endl;
    
    int idx = 0;
    for (int i = 0; i < resolucion_; ++i) {
        for (int j = 0; j < resolucion_; ++j) {
            malla(idx, 0) = min_val_ + j * paso;  // x
            malla(idx, 1) = min_val_ + i * paso;  // y
            idx++;
        }
    }
    
    return malla;
}

Eigen::VectorXd Evaluador::evaluar(const Eigen::MatrixXd& malla,
                                    const Eigen::MatrixXd& centros,
                                    const Eigen::VectorXd& pesos) const {
    int M = malla.rows();
    int N = pesos.size();
    Eigen::VectorXd z(M);
    
    std::cout << "  Evaluando superficie RBF..." << std::endl;
    std::cout << "    Puntos a evaluar: " << M << std::endl;
    std::cout << "    Centros (N): " << N << std::endl;
    std::cout << "    Evaluaciones del kernel: " << M * N << std::endl;
    
    double eps2 = epsilon_ * epsilon_;
    for (int k = 0; k < M; ++k) {
        double suma = 0.0;
        for (int j = 0; j < N; ++j) {
            double dx = malla(k, 0) - centros(j, 0);
            double dy = malla(k, 1) - centros(j, 1);
            double r2 = dx * dx + dy * dy;
            suma += pesos(j) * std::exp(-eps2 * r2);
        }
        z(k) = suma;
    }
    
    std::cout << "    Rango z_predicho: [" << z.minCoeff() << ", " << z.maxCoeff() << "]" << std::endl;
    
    return z;
}

Eigen::VectorXd Evaluador::evaluar_en_centros(const Eigen::MatrixXd& centros,
                                               const Eigen::VectorXd& pesos) const {
    return evaluar(centros, centros, pesos);
}

double Evaluador::calcular_rmse(const Eigen::VectorXd& y_real,
                                 const Eigen::VectorXd& y_predicho) {
    if (y_real.size() != y_predicho.size()) {
        std::cerr << "[ERROR] Dimensiones incompatibles en RMSE" << std::endl;
        return -1.0;
    }
    
    int N = y_real.size();
    double suma_sq = 0.0;
    
    for (int i = 0; i < N; ++i) {
        double diff = y_real(i) - y_predicho(i);
        suma_sq += diff * diff;
    }
    
    return std::sqrt(suma_sq / N);
}

Eigen::VectorXd Evaluador::funcion_real(const Eigen::MatrixXd& malla) {
    int M = malla.rows();
    Eigen::VectorXd z_real(M);
    
    for (int k = 0; k < M; ++k) {
        double x = malla(k, 0);
        double y = malla(k, 1);
        z_real(k) = std::sin(x) * std::cos(y);
    }
    
    return z_real;
}

bool Evaluador::exportar_superficie(const std::string& archivo,
                                     const Eigen::MatrixXd& malla,
                                     const Eigen::VectorXd& z_predicho) {
    std::filesystem::path path(archivo);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }
    
    std::ofstream out(archivo);
    if (!out.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir: " << archivo << std::endl;
        return false;
    }
    
    out << "x,y,z_predicho\n";
    out << std::fixed << std::setprecision(6);
    
    for (int i = 0; i < malla.rows(); ++i) {
        out << malla(i, 0) << ","
            << malla(i, 1) << ","
            << z_predicho(i) << "\n";
    }
    
    out.close();
    std::cout << "[OK] Superficie exportada a: " << archivo << std::endl;
    std::cout << "     Filas: " << malla.rows() << " (+ header)" << std::endl;
    return true;
}

bool Evaluador::exportar_funcion_real(const std::string& archivo,
                                       const Eigen::MatrixXd& malla) {
    Eigen::VectorXd z_real = funcion_real(malla);
    
    std::filesystem::path path(archivo);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }
    
    std::ofstream out(archivo);
    if (!out.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir: " << archivo << std::endl;
        return false;
    }
    
    out << "x,y,z_real\n";
    out << std::fixed << std::setprecision(6);
    
    for (int i = 0; i < malla.rows(); ++i) {
        out << malla(i, 0) << ","
            << malla(i, 1) << ","
            << z_real(i) << "\n";
    }
    
    out.close();
    std::cout << "[OK] Función real exportada a: " << archivo << std::endl;
    return true;
}