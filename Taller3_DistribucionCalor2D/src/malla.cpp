// malla.cpp
#include "malla.hpp"

Malla::Malla(int n_interno, double longitud)
    : n_(n_interno), 
      N_(n_interno * n_interno), 
      h_(longitud / (n_interno + 1)), 
      L_(longitud) {}

int Malla::get_n() const {
    return n_;
}

int Malla::get_N() const {
    return N_;
}

double Malla::get_h() const {
    return h_;
}

double Malla::get_longitud() const {
    return L_;
}

int Malla::indice_2d_a_1d(int i, int j) const {
    if (i < 1 || i > n_ || j < 1 || j > n_) {
        throw std::out_of_range("Índices fuera de rango");
    }
    return (i - 1) * n_ + (j - 1);
}

void Malla::indice_1d_a_2d(int k, int& i, int& j) const {
    if (k < 0 || k >= N_) {
        throw std::out_of_range("Índice fuera de rango");
    }
    i = k / n_ + 1;
    j = k % n_ + 1;
}

double Malla::coordenada_x(int i) const {
    return i * h_;
}

double Malla::coordenada_y(int j) const {
    return j * h_;
}