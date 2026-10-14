#include "exportador.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <filesystem>

bool Exportador::exportar_datos_entrenamiento(
    const std::string& archivo,
    const Eigen::MatrixXd& puntos,
    const Eigen::VectorXd& valores) {
    
    if (puntos.rows() != valores.size()) {
        std::cerr << "[ERROR] Dimensiones incompatibles" << std::endl;
        return false;
    }
    
    std::filesystem::path path(archivo);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }
    
    std::ofstream out(archivo);
    if (!out.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir: " << archivo << std::endl;
        return false;
    }
    
    out << "x,y,z\n";
    out << std::fixed << std::setprecision(6);
    
    for (int i = 0; i < puntos.rows(); ++i) {
        out << puntos(i, 0) << ","
            << puntos(i, 1) << ","
            << valores(i) << "\n";
    }
    
    out.close();
    std::cout << "[OK] Datos exportados a: " << archivo << std::endl;
    return true;
}

bool Exportador::exportar_pesos(
    const std::string& archivo,
    const Eigen::VectorXd& pesos,
    double epsilon) {
    
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
    return true;
}

bool Exportador::exportar_superficie(
    const std::string& archivo,
    const Eigen::MatrixXd& malla,
    const Eigen::VectorXd& z_predicho,
    double epsilon) {
    
    if (malla.rows() != z_predicho.size()) {
        std::cerr << "[ERROR] Dimensiones incompatibles en superficie" << std::endl;
        return false;
    }
    
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
    return true;
}