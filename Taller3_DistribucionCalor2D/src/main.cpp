// main.cpp
#include <iostream>
#include "malla.hpp"
#include "ensamblador.hpp"
#include "solver.hpp"
#include "exportador.hpp"

int main() {
    std::cout << "=== SIMULACIÓN DE DISTRIBUCIÓN DE CALOR 2D ===" << std::endl;

    const int n = 20;
    const double L = 1.0;
    Malla malla(n, L);

    std::cout << "Dominio: [0, " << L << "] × [0, " << L << "]" << std::endl;
    std::cout << "Nodos internos: " << n << " × " << n << " = " << n * n << std::endl;
    std::cout << "Espaciado h: " << malla.get_h() << std::endl;

    // Condiciones de frontera
    CondicionesFrontera fronteras;
    fronteras.Tsupper = 100.0;
    fronteras.Tinf = 0.0;
    fronteras.Tleft = 50.0;
    fronteras.Tright = 50.0;

    // Fuente de calor
    FuenteCalor fuente;
    fuente.activa = true;
    fuente.intensidad = 1000.0;
    fuente.nodo_i = (n + 1) / 2; // 10 para n=20
    fuente.nodo_j = (n + 1) / 2;

    std::cout << "\nConstruyendo sistema..." << std::endl;
    Ensamblador ensamblador(malla, fronteras, fuente);
    auto A = ensamblador.construir_matriz_A();
    auto b = ensamblador.construir_vector_b();

    std::cout << "  Matriz A: " << A.rows() << " × " << A.cols() << " (no nulos: " << A.nonZeros() << ")" << std::endl;
    std::cout << "  Vector b: " << b.size() << " × 1" << std::endl;

    std::cout << "\nResolviendo con BiCGSTAB..." << std::endl;
    Solver solver(1e-10, 1000);
    auto u = solver.resolver(A, b);

    std::cout << "  Iteraciones: " << solver.get_iteraciones() << std::endl;
    std::cout << "  Error final: " << solver.get_error_final() << std::endl;
    std::cout << "  Convergencia: " << (solver.convergencia() ? "Sí" : "No") << std::endl;

    double residuo = solver.calcular_residuo(A, u, b);
    std::cout << "  Residuo ||Au - b||: " << residuo << std::endl;

    std::cout << "\nExportando resultados..." << std::endl;
    Exportador::exportar_temperatura("../resultados/temperatura.csv", malla, u);
    Exportador::exportar_temperatura_completa("../resultados/temperatura_completa.csv", malla, u,
                                               fronteras.Tsupper, fronteras.Tinf,
                                               fronteras.Tleft, fronteras.Tright);
    ensamblador.exportar_patron_matriz("../resultados/matriz_A.csv", A);

    std::cout << "\n=== CASO 2: SIN FUENTE DE CALOR ===" << std::endl;
    fuente.activa = false;
    Ensamblador ensamblador2(malla, fronteras, fuente);
    auto A2 = ensamblador2.construir_matriz_A();
    auto b2 = ensamblador2.construir_vector_b();
    Solver solver2(1e-10, 1000);
    auto u2 = solver2.resolver(A2, b2);
    Exportador::exportar_temperatura("../resultados/temperatura_sin_fuente.csv", malla, u2);
    Exportador::exportar_temperatura_completa("../resultados/temperatura_sin_fuente_completa.csv", malla, u2,
                                               fronteras.Tsupper, fronteras.Tinf,
                                               fronteras.Tleft, fronteras.Tright);
    std::cout << "Resultados sin fuente exportados." << std::endl;

    return 0;
}
