// exportador.cpp
#include "exportador.hpp"
#include <Eigen/Dense>
#include <fstream>
#include <string>
#include "malla.hpp"

bool Exportador::exportar_temperatura(const std::string& archivo,
                                      const Malla& malla,
                                      const Eigen::VectorXd& u) {
    std::ofstream ofs(archivo);
    if (!ofs.is_open()) {
        return false;
    }
    ofs << "x,y,temperatura\n";

    int n = malla.get_n();
    double h = malla.get_h();

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            int k = malla.indice_2d_a_1d(i, j);
            double x = i * h;
            double y = j * h;
            ofs << x << "," << y << "," << u[k] << "\n";
        }
    }
    return true;
}

bool Exportador::exportar_temperatura_completa(const std::string& archivo,
                                                const Malla& malla,
                                                const Eigen::VectorXd& u,
                                                double Tupper, double Tinf,
                                                double Tleft, double Tright) {
    std::ofstream ofs(archivo);
    if (!ofs.is_open()) {
        return false;
    }
    ofs << "x,y,temperatura,tipo\n";

    int n = malla.get_n();
    double h = malla.get_h();
    int total = (n+2)*(n+2); // Incluye bordes
    // Podemos mapear índices de (i,j) con i,j ∈ [0, n+1] donde 0 y n+1 son fronteras

    // Recorrer todas las posiciones (i,j) con i=0..n+1, j=0..n+1
    for (int i = 0; i <= n+1; ++i) {
        for (int j = 0; j <= n+1; ++j) {
            double x = i * h;
            double y = j * h;
            double temp;
            std::string tipo = "interno";

            // Determinar si está en frontera
            bool frontera = false;
            if (i == 0) { temp = Tleft;  frontera = true; tipo = "frontera"; }
            else if (i == n+1) { temp = Tright; frontera = true; tipo = "frontera"; }
            else if (j == 0) { temp = Tinf;  frontera = true; tipo = "frontera"; }
            else if (j == n+1) { temp = Tupper; frontera = true; tipo = "frontera"; }
            else {
                // nodo interno: usar u de la solución (índices i-1, j-1)
                int k = malla.indice_2d_a_1d(i, j); // i,j desde 1..n
                temp = u[k];
            }

            ofs << x << "," << y << "," << temp << "," << tipo << "\n";
        }
    }
    return true;
}