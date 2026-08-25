#ifndef CASOS_PRUEBA_HPP
#define CASOS_PRUEBA_HPP

#include <Eigen/Dense>
#include <string>
#include <vector>

struct CasoPrueba {
    int id;
    std::string nombre;
    std::string descripcion;
    Eigen::MatrixXd transicion;  // Matriz M ya normalizada
};

class CasosPrueba {
public:
    // Obtener todos los casos de prueba
    static std::vector<CasoPrueba> obtener_todos();
    
    // Obtener un caso específico
    static CasoPrueba obtener_caso(int id);
    
    // Generar matrices de prueba (proporcionadas por el profesor)
    static Eigen::MatrixXd caso_1();  // 3x3 con sumidero
    static Eigen::MatrixXd caso_2();  // 4x4 regular
    static Eigen::MatrixXd caso_3();  // 8x8 complejo
    static Eigen::MatrixXd caso_4();  // 5x5 con estructura
    static Eigen::MatrixXd caso_5();  // 3x3 con sumidero
};

#endif
