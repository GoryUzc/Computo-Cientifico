#include "solver_rk4.hpp"

SolverRK4::SolverRK4(const ModeloSEIR& modelo, double h, double t_final)
    : modelo_(modelo), h_(h), t_final_(t_final) {}

Eigen::Vector4d SolverRK4::paso_rk4(double t, const Eigen::Vector4d& y, 
                                     int escenario) {
    // Las 4 etapas evalúan f en tiempos diferentes
    // Cada evaluación obtendrá el β correcto según t
    Eigen::Vector4d k1 = modelo_.evaluar(t, y, escenario);
    Eigen::Vector4d k2 = modelo_.evaluar(t + h_/2.0, y + (h_/2.0) * k1, escenario);
    Eigen::Vector4d k3 = modelo_.evaluar(t + h_/2.0, y + (h_/2.0) * k2, escenario);
    Eigen::Vector4d k4 = modelo_.evaluar(t + h_, y + h_ * k3, escenario);
    
    return y + (h_ / 6.0) * (k1 + 2.0*k2 + 2.0*k3 + k4);
}

std::vector<EstadoSEIR> SolverRK4::resolver(int escenario, 
                                             const Eigen::Vector4d& y0) {
    std::vector<EstadoSEIR> historia;
    historia.reserve(static_cast<size_t>(t_final_ / h_) + 1);
    
    Eigen::Vector4d y = y0;
    double t = 0.0;
    double N0 = y.sum();
    
    // Estado inicial
    EstadoSEIR estado_inicial;
    estado_inicial.tiempo = t;
    estado_inicial.y = y;
    estado_inicial.conservacion = y.sum() - N0;
    historia.push_back(estado_inicial);
    
    // Ciclo principal
    int num_pasos = static_cast<int>(t_final_ / h_);
    for (int i = 0; i < num_pasos; ++i) {
        y = paso_rk4(t, y, escenario);
        t += h_;
        
        EstadoSEIR estado;
        estado.tiempo = t;
        estado.y = y;
        estado.conservacion = y.sum() - N0;
        historia.push_back(estado);
    }
    
    return historia;
}

double SolverRK4::calcular_pico_infectados(const std::vector<EstadoSEIR>& historia) const {
    double pico = 0.0;
    for (const auto& estado : historia) {
        if (estado.y(2) > pico) {
            pico = estado.y(2);
        }
    }
    return pico;
}

double SolverRK4::calcular_dia_pico(const std::vector<EstadoSEIR>& historia) const {
    double pico = 0.0;
    double dia_pico = 0.0;
    for (const auto& estado : historia) {
        if (estado.y(2) > pico) {
            pico = estado.y(2);
            dia_pico = estado.tiempo;
        }
    }
    return dia_pico;
}

double SolverRK4::calcular_recuperados_finales(const std::vector<EstadoSEIR>& historia) const {
    if (!historia.empty()) {
        return historia.back().y(3);  // R final
    }
    return 0.0;
}

double SolverRK4::calcular_error_max_conservacion(const std::vector<EstadoSEIR>& historia) const {
    double error_max = 0.0;
    for (const auto& estado : historia) {
        double error_abs = std::abs(estado.conservacion);
        if (error_abs > error_max) {
            error_max = error_abs;
        }
    }
    return error_max;
}