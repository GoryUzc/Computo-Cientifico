// ensamblador.hpp
#ifndef ENSAMBLADOR_HPP
#define ENSAMBLADOR_HPP

#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <string>

#include "malla.hpp"

struct CondicionesFrontera {
    double Tsupper;     // y = L
    double Tinf;        // y = 0
    double Tleft;       // x = 0
    double Tright;      // x = L
};

struct FuenteCalor {
    bool activa;
    double intensidad;   // Q
    int nodo_i;          // Posición x del nodo fuente
    int nodo_j;          // Posición y del nodo fuente
};

class Ensamblador {
public:
    Ensamblador(const Malla& malla, 
                const CondicionesFrontera& fronteras,
                const FuenteCalor& fuente);

    Eigen::SparseMatrix<double> construir_matriz_A();
    Eigen::VectorXd construir_vector_b();

    // Exportación de patron de matriz
    void exportar_patron_matriz(const std::string& archivo,
                                const Eigen::SparseMatrix<double>& A);

private:
    const Malla& malla_;
    CondicionesFrontera fronteras_;
    FuenteCalor fuente_;

    void agregar_contribucion_frontera(int k, int i, int j, Eigen::VectorXd& b);
    void agregar_contribucion_fuente(int k, int i, int j, Eigen::VectorXd& b);
};
#endif