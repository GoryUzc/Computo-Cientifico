#include "exportador.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>

bool Exportador::exportar_evolucion(const std::string& archivo,
                                     const std::vector<EstadoSEIR>& historia) {
    std::ofstream out(archivo);
    if (!out.is_open()) {
        return false;
    }
    
    out << "tiempo,S,E,I,R" << std::endl;
    for (const auto& estado : historia) {
        out << std::fixed << std::setprecision(1) << estado.tiempo << ","
            << std::setprecision(1) << estado.y(0) << ","
            << std::setprecision(1) << estado.y(1) << ","
            << std::setprecision(1) << estado.y(2) << ","
            << std::setprecision(1) << estado.y(3) << std::endl;
    }
    
    out.close();
    return true;
}

bool Exportador::exportar_conservacion(const std::string& archivo,
                                        const std::vector<EstadoSEIR>& historia) {
    std::ofstream out(archivo);
    if (!out.is_open()) {
        return false;
    }
    
    out << "tiempo,error_absoluto" << std::endl;
    for (const auto& estado : historia) {
        double error = std::abs(estado.conservacion);
        out << std::fixed << std::setprecision(6) << estado.tiempo << ","
            << std::setprecision(6) << error << std::endl;
    }
    
    out.close();
    return true;
}

bool Exportador::exportar_resumen(const std::string& archivo,
                                  double pico_A, double dia_pico_A, 
                                  double recuperados_A, double error_A,
                                  double pico_B, double dia_pico_B, 
                                  double recuperados_B, double error_B) {
    std::ofstream out(archivo);
    if (!out.is_open()) {
        return false;
    }
    
    out << "metrica,escenario_A,escenario_B" << std::endl;
    out << "pico_infectados," << std::setprecision(1) << pico_A << "," << std::setprecision(1) << pico_B << std::endl;
    out << "dia_pico," << std::setprecision(1) << dia_pico_A << "," << std::setprecision(1) << dia_pico_B << std::endl;
    out << "recuperados_finales," << std::setprecision(1) << recuperados_A << "," << std::setprecision(1) << recuperados_B << std::endl;
    out << "error_max_conservacion," << std::setprecision(6) << error_A << "," << std::setprecision(6) << error_B << std::endl;
    
    out.close();
    return true;
}