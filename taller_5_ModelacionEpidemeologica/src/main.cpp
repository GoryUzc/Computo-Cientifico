#include <iostream>
#include <iomanip>
#include "modelo_seir.hpp"
#include "solver_rk4.hpp"
#include "exportador.hpp"

int main() {
    std::cout << "=== MODELO EPIDEMIOLÓGICO SEIR ===" << std::endl;
    std::cout << "Solver: Runge-Kutta 4to Orden (RK4)" << std::endl;
    
    // Parámetros del modelo
    const double N = 3e7;
    const double sigma = 0.2;   // 1/5 días
    const double gamma = 0.1;   // 1/10 días
    
    ModeloSEIR modelo(N, sigma, gamma);
    
    // Parámetros de simulación
    const double h = 0.1;       // Paso temporal (días)
    const double t_final = 200; // Tiempo final (días)
    
    SolverRK4 solver(modelo, h, t_final);
    
    // Condiciones iniciales: S₀ = N-100, E₀ = 0, I₀ = 100, R₀ = 0
    Eigen::Vector4d y0;
    y0 << N - 100.0, 0.0, 100.0, 0.0;
    
    std::cout << "\nCondiciones iniciales:" << std::endl;
    std::cout << "  S(0) = " << y0(0) << std::endl;
    std::cout << "  E(0) = " << y0(1) << std::endl;
    std::cout << "  I(0) = " << y0(2) << std::endl;
    std::cout << "  R(0) = " << y0(3) << std::endl;
    std::cout << "  N(0) = " << y0.sum() << std::endl;
    
    // ============================================
    // ESCENARIO A: Epidemia Libre
    // ============================================
    std::cout << "\n=== ESCENARIO A: Epidemia Libre ===" << std::endl;
    std::cout << "  β = 0.6 (constante)" << std::endl;
    std::cout << "  R₀ = " << modelo.calcular_R0(1) << std::endl;
    
    auto historia_A = solver.resolver(1, y0);
    
    double pico_A = solver.calcular_pico_infectados(historia_A);
    double dia_pico_A = solver.calcular_dia_pico(historia_A);
    double recuperados_A = solver.calcular_recuperados_finales(historia_A);
    double error_A = solver.calcular_error_max_conservacion(historia_A);
    
    std::cout << "  Pico de infectados: " << pico_A << std::endl;
    std::cout << "  Día del pico: " << dia_pico_A << std::endl;
    std::cout << "  Recuperados finales: " << recuperados_A << std::endl;
    std::cout << "  Error máx conservación: " << error_A << std::endl;
    
    // ============================================
    // ESCENARIO B: Con Intervención
    // ============================================
    std::cout << "\n=== ESCENARIO B: Con Intervención ===" << std::endl;
    std::cout << "  β = 0.6 (t < 30), β = 0.25 (t >= 30)" << std::endl;
    std::cout << "  R₀ inicial = 6.0, R₀ intervención = 2.5" << std::endl;
    
    auto historia_B = solver.resolver(2, y0);
    
    double pico_B = solver.calcular_pico_infectados(historia_B);
    double dia_pico_B = solver.calcular_dia_pico(historia_B);
    double recuperados_B = solver.calcular_recuperados_finales(historia_B);
    double error_B = solver.calcular_error_max_conservacion(historia_B);
    
    std::cout << "  Pico de infectados: " << pico_B << std::endl;
    std::cout << "  Día del pico: " << dia_pico_B << std::endl;
    std::cout << "  Recuperados finales: " << recuperados_B << std::endl;
    std::cout << "  Error máx conservación: " << error_B << std::endl;
    
    // ============================================
    // EXPORTAR RESULTADOS
    // ============================================
    std::cout << "\n=== EXPORTANDO RESULTADOS ===" << std::endl;
    
    Exportador::exportar_evolucion("../../resultados/escenario_A.csv", historia_A);
    Exportador::exportar_evolucion("../../resultados/escenario_B.csv", historia_B);
    Exportador::exportar_conservacion("../../resultados/conservacion_A.csv", historia_A);
    Exportador::exportar_conservacion("../../resultados/conservacion_B.csv", historia_B);
    Exportador::exportar_resumen("../../resultados/resumen.csv",
                                  pico_A, dia_pico_A, recuperados_A, error_A,
                                  pico_B, dia_pico_B, recuperados_B, error_B);
    
    std::cout << "Resultados exportados a resultados/" << std::endl;
    
    return 0;
}