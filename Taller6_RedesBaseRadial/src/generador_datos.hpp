#ifndef GENERADOR_DATOS_HPP
#define GENERADOR_DATOS_HPP

#include <Eigen/Dense>
#include <string>

class GeneradorDatos {
public:
    /**
     * @brief Constructor del generador de datos
     * @param N Número de puntos a generar
     * @param min_val Límite inferior del dominio (ej. -2.0)
     * @param max_val Límite superior del dominio (ej. 2.0)
     * @param ruido_sigma Desviación estándar del ruido gaussiano
     * @param seed Semilla para reproducibilidad (default: 42)
     */
    GeneradorDatos(int N, double min_val, double max_val,
                   double ruido_sigma, unsigned int seed = 42);

    /**
     * @brief Genera los N puntos de entrenamiento
     *        Llena internamente puntos_ y valores_
     */
    void generar();

    /**
     * @brief Exporta los datos a un archivo CSV
     * @param archivo Ruta del archivo (ej. "../resultados/datos_entrenamiento.csv")
     * @return true si la exportación fue exitosa
     */
    bool exportar_csv(const std::string& archivo) const;

    // ============ Getters ============
    const Eigen::MatrixXd& get_puntos() const { return puntos_; }
    const Eigen::VectorXd& get_valores() const { return valores_; }
    const Eigen::VectorXd& get_valores_limpios() const { return valores_limpios_; }
    int get_N() const { return N_; }

private:
    int N_;                    // Número de puntos
    double min_val_;           // Límite inferior del dominio
    double max_val_;           // Límite superior del dominio
    double ruido_sigma_;       // Desviación estándar del ruido
    unsigned int seed_;        // Semilla para reproducibilidad

    Eigen::MatrixXd puntos_;         // N x 2: columnas [x, y]
    Eigen::VectorXd valores_;        // N x 1: z con ruido
    Eigen::VectorXd valores_limpios_; // N x 1: z sin ruido (referencia)

    /**
     * @brief Función objetivo: f(x, y) = sin(x) * cos(y)
     */
    double funcion_objetivo(double x, double y) const;
};

#endif // GENERADOR_DATOS_HPP