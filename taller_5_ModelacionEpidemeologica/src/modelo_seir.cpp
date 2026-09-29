#include "modelo_seir.hpp"

ModeloSEIR::ModeloSEIR(double N, double sigma, double gamma)
    : N_(N), sigma_(sigma), gamma_(gamma) {}

Eigen::Vector4d ModeloSEIR::evaluar(double t, const Eigen::Vector4d& y, 
                                     int escenario) const {
    double beta = obtener_beta(t, escenario);
    double S = y(0), E = y(1), I = y(2), R = y(3);
    
    double contagio = beta * S * I / N_;
    
    Eigen::Vector4d dydt;
    dydt(0) = -contagio;              // dS/dt
    dydt(1) = contagio - sigma_ * E;  // dE/dt
    dydt(2) = sigma_ * E - gamma_ * I;// dI/dt
    dydt(3) = gamma_ * I;             // dR/dt
    
    return dydt;
}

double ModeloSEIR::obtener_beta(double t, int escenario) const {
    if (escenario == 1) {
        return 0.6;  // Escenario A: constante
    } else {
        return (t < 30.0) ? 0.6 : 0.25;  // Escenario B: intervención
    }
}

double ModeloSEIR::calcular_R0(int escenario) const {
    // Para escenario B, retornamos el R₀ inicial (β=0.6)
    return 0.6 / gamma_;
}