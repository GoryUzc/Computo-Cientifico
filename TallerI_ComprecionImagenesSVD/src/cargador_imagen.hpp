#ifndef CARGADOR_IMAGEN_HPP
#define CARGADOR_IMAGEN_HPP

#include <string>
#include <vector>
#include <Eigen/Dense>

/**
 * @brief Carga una imagen desde disco y la convierte a una matriz Eigen::MatrixXd en escala de grises.
 *
 * Internamente usa stb_image (incluido en include/stb/stb_image.h).
 * El valor de cada píxel se normaliza al rango [0.0, 255.0] en formato double.
 */
Eigen::MatrixXd cargarImagenGris(const std::string& ruta);

/**
 * @brief Carga una imagen RGB desde disco y devuelve 3 canales como matrices Eigen columna-stack.
 * Cada canal es Eigen::MatrixXd con valores [0.0, 255.0].
 */
std::vector<Eigen::MatrixXd> cargarImagenColor(const std::string& ruta);

/**
 * @brief Devuelve el ancho y alto de una imagen sin cargarla completamente a memoria.
 */
std::pair<int, int> obtenerDimensiones(const std::string& ruta);

#endif // CARGADOR_IMAGEN_HPP
