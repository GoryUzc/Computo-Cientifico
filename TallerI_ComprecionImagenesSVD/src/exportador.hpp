#ifndef EXPORTADOR_HPP
#define EXPORTADOR_HPP

#include <string>
#include <vector>
#include <Eigen/Dense>

/**
 * @brief Guarda una matriz Eigen::MatrixXd como imagen PNG en escala de grises.
 * Los valores se clamplean al rango [0, 255].
 */
void guardarPNGGris(const std::string& ruta, const Eigen::MatrixXd& matriz);

/**
 * @brief Guarda tres canales (R, G, B) como imagen PNG a color.
 * Los valores se clamplean al rango [0, 255].
 */
void guardarPNGColor(const std::string& ruta,
                     const Eigen::MatrixXd& R,
                     const Eigen::MatrixXd& G,
                     const Eigen::MatrixXd& B);

#endif // EXPORTADOR_HPP
