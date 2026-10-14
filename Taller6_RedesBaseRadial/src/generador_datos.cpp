#include "generador_datos.hpp"
#include <fstream>
#include <iomanip>
#include <random>
#include <cmath>
#include <filesystem>
#include <iostream>

GeneradorDatos::GeneradorDatos(int N, double min_val, double max_val,
                                 double ruido_sigma, unsigned int seed)
    : N_(N),
      min_val_(min_val),
      max_val_(max_val),
      ruido_sigma_(ruido_sigma),
      seed_(seed) {

    puntos_.resize(N_, 2);
    valores_.resize(N_);
    valores_limpios_.resize(N_);
}

void GeneradorDatos::generar() {
    std::mt19937 gen(seed_);
    std::uniform_real_distribution<double> dist_xy(min_val_, max_val_);
    std::normal_distribution<double> ruido(0.0, ruido_sigma_);

    for (int i = 0; i < N_; ++i) {
        double x = dist_xy(gen);
        double y = dist_xy(gen);

        puntos_(i, 0) = x;
        puntos_(i, 1) = y;

        double z_limpio = funcion_objetivo(x, y);
        double z_ruidoso = z_limpio + ruido(gen);

        valores_limpios_(i) = z_limpio;
        valores_(i) = z_ruidoso;
    }

    std::cout << "[OK] Generados " << N_ << " puntos de entrenamiento" << std::endl;
    std::cout << "     Dominio: [" << min_val_ << ", " << max_val_ << "]^2" << std::endl;
    std::cout << "     Ruido gaussiano: sigma = " << ruido_sigma_ << std::endl;
    std::cout << "     Semilla: " << seed_ << std::endl;
}

double GeneradorDatos::funcion_objetivo(double x, double y) const {
    return std::sin(x) * std::cos(y);
}

bool GeneradorDatos::exportar_csv(const std::string& archivo) const {
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

    for (int i = 0; i < N_; ++i) {
        out << puntos_(i, 0) << ","
            << puntos_(i, 1) << ","
            << valores_(i) << "\n";
    }

    out.close();
    std::cout << "[OK] Datos exportados a: " << archivo << std::endl;
    std::cout << "     Filas: " << N_ << " (+ header)" << std::endl;
    return true;
}
