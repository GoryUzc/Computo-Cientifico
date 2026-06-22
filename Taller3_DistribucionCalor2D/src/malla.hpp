// malla.hpp
#ifndef MALLA_HPP
#define MALLA_HPP

#include <stdexcept>

class Malla {
public:
    Malla(int n_interno, double longitud);

    int get_n() const;
    int get_N() const;
    double get_h() const;
    double get_longitud() const;

    int indice_2d_a_1d(int i, int j) const;
    void indice_1d_a_2d(int k, int& i, int& j) const;

    double coordenada_x(int i) const;
    double coordenada_y(int j) const;

private:
    int n_;   // Nodos internos por lado
    int N_;   // Total de incógnitas
    double h_; // Espaciado
    double L_; // Longitud
};

#endif
