#ifndef EXPORTADOR_HPP
#define EXPORTADOR_HPP

#include "solver_rk4.hpp"
#include <string>

class Exportador {
public:
    // Exportar evolución temporal
    static bool exportar_evolucion(const std::string& archivo,
                                    const std::vector<EstadoSEIR>& historia);
    
    // Exportar verificación de conservación
    static bool exportar_conservacion(const std::string& archivo,
                                       const std::vector<EstadoSEIR>& historia);
    
    // Exportar resumen comparativo
    static bool exportar_resumen(const std::string& archivo,
                                  double pico_A, double dia_pico_A, 
                                  double recuperados_A, double error_A,
                                  double pico_B, double dia_pico_B, 
                                  double recuperados_B, double error_B);
};

#endif