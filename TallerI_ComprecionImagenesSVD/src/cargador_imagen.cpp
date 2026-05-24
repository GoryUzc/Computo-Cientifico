#include "cargador_imagen.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Eigen::MatrixXd cargarImagenGris(const std::string& ruta) {
    int ancho, alto, canales;
    unsigned char* datos = stbi_load(ruta.c_str(), &ancho, &alto, &canales, 1);
    if (!datos) {
        throw std::runtime_error("No se pudo cargar la imagen: " + ruta);
    }
    Eigen::MatrixXd matriz(alto, ancho);
    for (int i = 0; i < alto; ++i) {
        for (int j = 0; j < ancho; ++j) {
            matriz(i, j) = static_cast<double>(datos[i * ancho + j]);
        }
    }
    stbi_image_free(datos);
    return matriz;
}

std::vector<Eigen::MatrixXd> cargarImagenColor(const std::string& ruta) {
    int ancho, alto, canales;
    unsigned char* datos = stbi_load(ruta.c_str(), &ancho, &alto, &canales, 3);
    if (!datos) {
        throw std::runtime_error("No se pudo cargar la imagen: " + ruta);
    }
    std::vector<Eigen::MatrixXd> canales_mat(3);
    canales_mat[0] = Eigen::MatrixXd(alto, ancho);
    canales_mat[1] = Eigen::MatrixXd(alto, ancho);
    canales_mat[2] = Eigen::MatrixXd(alto, ancho);
    for (int i = 0; i < alto; ++i) {
        for (int j = 0; j < ancho; ++j) {
            int idx = (i * ancho + j) * 3;
            canales_mat[0](i, j) = static_cast<double>(datos[idx + 0]);
            canales_mat[1](i, j) = static_cast<double>(datos[idx + 1]);
            canales_mat[2](i, j) = static_cast<double>(datos[idx + 2]);
        }
    }
    stbi_image_free(datos);
    return canales_mat;
}

std::pair<int, int> obtenerDimensiones(const std::string& ruta) {
    int ancho, alto, canales;
    if (!stbi_info(ruta.c_str(), &ancho, &alto, &canales)) {
        throw std::runtime_error("No se pudo leer info de la imagen: " + ruta);
    }
    return {ancho, alto};
}
