#ifndef SOLVER_RK4_HPP
#define SOLVER_RK4_HPP

#include "modelo_seir.hpp"
#include <vector>

struct EstadoSEIR {
    double tiempo;
    Eigen::Vector4d y;       // [S, E, I, R]
    double conservacion;     // S + E + I + R - N
};

class SolverRK4 {
public:
    SolverRK4(const ModeloSEIR& modelo, double h, double t_final);
    
    // Resolver sistema para un escenario específico
    std::vector<EstadoSEIR> resolver(int escenario, const Eigen::Vector4d& y0);
    
    // Métricas
    double calcular_pico_infectados(const std::vector<EstadoSEIR>& historia) const;
    double calcular_dia_pico(const std::vector<EstadoSEIR>& historia) const;
    double calcular_recuperados_finales(const std::vector<EstadoSEIR>& historia) const;
    double calcular_error_max_conservacion(const std::vector<EstadoSEIR>& historia) const;

private:
    const ModeloSEIR& modelo_;
    double h_;
    double t_final_;
    
    // Paso individual de RK4
    Eigen::Vector4d paso_rk4(double t, const Eigen::Vector4d& y, int escenario);
};

#endif