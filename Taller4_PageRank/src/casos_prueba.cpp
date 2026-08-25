#include "casos_prueba.hpp"

#include <stdexcept>

Eigen::MatrixXd CasosPrueba::caso_1() {
    // 3x3 con sumidero (columna 3 es sumidero)
    Eigen::MatrixXd M(3, 3);
    M << 0.0, 0.0, 0.0,
         1.0/2.0, 0.0, 0.0,
         1.0/2.0, 1.0, 0.0;
    return M;
}

Eigen::MatrixXd CasosPrueba::caso_2() {
    // 4x4 regular (todas las columnas suman 1)
    Eigen::MatrixXd M(4, 4);
    M << 0.0, 1.0/2.0, 0.0, 1.0/2.0,
         1.0/2.0, 0.0, 1.0/2.0, 0.0,
         0.0, 0.0, 1.0/2.0, 1.0/2.0,
         1.0/2.0, 0.0, 1.0/2.0, 0.0;
    return M;
}

Eigen::MatrixXd CasosPrueba::caso_3() {
    // 8x8 complejo
    Eigen::MatrixXd M(8, 8);
    M << 0.0, 1.0/2.0, 1.0/3.0, 0.0, 0.0, 0.0, 0.0, 0.0,
         1.0/2.0, 0.0, 1.0/3.0, 0.0, 0.0, 0.0, 0.0, 0.0,
         1.0/2.0, 1.0/2.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0,
         0.0, 0.0, 0.0, 1.0/3.0, 0.0, 0.0, 0.0, 0.0,
         0.0, 0.0, 0.0, 0.0, 0.0, 1.0/2.0, 1.0/3.0, 0.0,
         0.0, 0.0, 0.0, 0.0, 1.0/2.0, 0.0, 1.0/3.0, 0.0,
         0.0, 0.0, 0.0, 0.0, 1.0/2.0, 1.0/2.0, 0.0, 1.0,
         0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0/3.0, 0.0;
    return M;
}

Eigen::MatrixXd CasosPrueba::caso_4() {
    // 5x5 con estructura
    Eigen::MatrixXd M(5, 5);
    M << 0.0, 0.0, 1.0/2.0, 0.0, 0.0,
         1.0, 0.0, 0.0, 0.0, 0.0,
         0.0, 1.0, 0.0, 0.0, 0.0,
         0.0, 0.0, 1.0/2.0, 0.0, 1.0,
         0.0, 0.0, 0.0, 1.0, 0.0;
    return M;
}

Eigen::MatrixXd CasosPrueba::caso_5() {
    // 3x3 con sumidero (columna 3 es sumidero)
    Eigen::MatrixXd M(3, 3);
    M << 0.0, 1.0, 1.0,
         1.0, 0.0, 0.0,
         0.0, 0.0, 0.0;
    return M;
}

std::vector<CasoPrueba> CasosPrueba::obtener_todos() {
    std::vector<CasoPrueba> casos;
    
    casos.push_back({1, "Caso 1", "3x3 con sumidero (columna 3)", caso_1()});
    casos.push_back({2, "Caso 2", "4x4 regular", caso_2()});
    casos.push_back({3, "Caso 3", "8x8 complejo", caso_3()});
    casos.push_back({4, "Caso 4", "5x5 con estructura", caso_4()});
    casos.push_back({5, "Caso 5", "3x3 con sumidero (columna 3)", caso_5()});
    
    return casos;
}

CasoPrueba CasosPrueba::obtener_caso(int id) {
    switch(id) {
        case 1: return {1, "Caso 1", "3x3 con sumidero (columna 3)", caso_1()};
        case 2: return {2, "Caso 2", "4x4 regular", caso_2()};
        case 3: return {3, "Caso 3", "8x8 complejo", caso_3()};
        case 4: return {4, "Caso 4", "5x5 con estructura", caso_4()};
        case 5: return {5, "Caso 5", "3x3 con sumidero (columna 3)", caso_5()};
        default: throw std::invalid_argument("Caso no válido");
    }
}
