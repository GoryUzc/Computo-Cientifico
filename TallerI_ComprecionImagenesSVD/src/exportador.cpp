#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "exportador.hpp"
#include <algorithm>
#include <cstring>

static unsigned char clamp255(double v) {
    long iv = static_cast<long>(v + 0.5);
    if (iv < 0) return 0;
    if (iv > 255) return 255;
    return static_cast<unsigned char>(iv);
}

void guardarPNGGris(const std::string& ruta, const Eigen::MatrixXd& matriz) {
    int m = static_cast<int>(matriz.rows());
    int n = static_cast<int>(matriz.cols());
    std::vector<unsigned char> pixeles(static_cast<size_t>(m) * n);
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            pixeles[static_cast<size_t>(i) * n + j] = clamp255(matriz(i, j));
        }
    }
    int ok = stbi_write_png(ruta.c_str(), n, m, 1, pixeles.data(), n * 1);
    if (!ok) {
        throw std::runtime_error("No se pudo guardar PNG: " + ruta);
    }
}

void guardarPNGColor(const std::string& ruta,
                     const Eigen::MatrixXd& R,
                     const Eigen::MatrixXd& G,
                     const Eigen::MatrixXd& B) {
    int m = static_cast<int>(R.rows());
    int n = static_cast<int>(R.cols());
    std::vector<unsigned char> pixeles(static_cast<size_t>(m) * n * 3);
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            size_t idx = (static_cast<size_t>(i) * n + j) * 3;
            pixeles[idx + 0] = clamp255(R(i, j));
            pixeles[idx + 1] = clamp255(G(i, j));
            pixeles[idx + 2] = clamp255(B(i, j));
        }
    }
    int ok = stbi_write_png(ruta.c_str(), n, m, 3, pixeles.data(), n * 3);
    if (!ok) {
        throw std::runtime_error("No se pudo guardar PNG color: " + ruta);
    }
}
