#ifndef MODELO_SEIR_HPP
#define MODELO_SEIR_HPP

#include <Eigen/Dense>

class ModeloSEIR {
public:
    ModeloSEIR(double N, double sigma, double gamma);
    
    // Función f(t, y) del sistema SEIR
    // y = [S, E, I, R]
    Eigen::Vector4d evaluar(double t, const Eigen::Vector4d& y, 
                            int escenario) const;
    
    // Obtener β según escenario y tiempo
    double obtener_beta(double t, int escenario) const;
    
    // Calcular R₀ teórico
    double calcular_R0(int escenario) const;
    
    // Getters
    double get_N() const { return N_; }
    double get_sigma() const { return sigma_; }
    double get_gamma() const { return gamma_; }

private:
    double N_;       // Población total
    double sigma_;   // Tasa de progresión E → I (1/5 días)
    double gamma_;   // Tasa de recuperación I → R (1/10 días)
};

#endif