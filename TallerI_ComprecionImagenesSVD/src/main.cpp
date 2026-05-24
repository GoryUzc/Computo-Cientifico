#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#endif
#include "cargador_imagen.hpp"
#include "svd_procesador.hpp"
#include "metricas.hpp"
#include "exportador.hpp"

namespace fs = std::filesystem;

/**
 * Resuelve la raíz del proyecto buscando hacia arriba desde el directorio
 * de trabajo actual (o desde la carpeta del ejecutable en Windows) hasta
 * encontrar el archivo CMakeLists.txt.
 */
static std::string detectarRaizProyecto() {
    fs::path dir = fs::current_path();
    for (int i = 0; i < 12; ++i) {
        if (fs::exists(dir / "CMakeLists.txt")) return dir.string();
        if (dir.has_parent_path() && dir != dir.parent_path())
            dir = dir.parent_path();
        else
            break;
    }
    // Fallback: usar carpeta del ejecutable
    char buf[MAX_PATH];
    DWORD n = GetModuleFileNameA(NULL, buf, MAX_PATH);
    if (n > 0 && n < MAX_PATH) {
        dir = fs::path(buf).parent_path();
        for (int i = 0; i < 12; ++i) {
            if (fs::exists(dir / "CMakeLists.txt")) return dir.string();
            if (dir.has_parent_path() && dir != dir.parent_path())
                dir = dir.parent_path();
            else
                break;
        }
    }
    return fs::current_path().string();
}

void imprimirProgreso(int paso, int total, const std::string& mensaje) {
    std::cout << "[" << paso << "/" << total << "] " << mensaje << std::endl;
}

int main(int argc, char* argv[]) {
    std::string raiz = detectarRaizProyecto();

    std::string ruta_imagen = raiz + "/assets/grises/edificio_gris_1.png";
    std::string modo = "gris";
    std::string id_imagen = "1";  // ID para archivos de salida únicos
    std::vector<int> valores_k = {5, 20, 50, 100, 200};

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--k" && i + 1 < argc) {
            std::string ks = argv[++i];
            valores_k.clear();
            std::stringstream ss(ks);
            std::string tok;
            while (std::getline(ss, tok, ',')) {
                valores_k.push_back(std::stoi(tok));
            }
        } else if (arg == "--modo" && i + 1 < argc) {
            modo = argv[++i];
        } else if (arg == "--imagen" && i + 1 < argc) {
            ruta_imagen = argv[++i];
        } else if (arg == "--id" && i + 1 < argc) {
            id_imagen = argv[++i];
        }
    }

    try {
        // Crear directorios de salida si no existen
        fs::create_directories(raiz + "/resultados/reconstrucciones");
        fs::create_directories(raiz + "/resultados/datos");

        // PASO 1
        auto [ancho, alto] = obtenerDimensiones(ruta_imagen);
        imprimirProgreso(1, 6, "Cargando imagen: " + ruta_imagen +
                                " (" + std::to_string(ancho) + "x" + std::to_string(alto) + ")");

        std::vector<Eigen::MatrixXd> canales;
        std::vector<ResultadoSVD> svd_por_canal;

        if (modo == "color") {
            canales = cargarImagenColor(ruta_imagen);
        } else {
            canales.push_back(cargarImagenGris(ruta_imagen));
        }

        // PASO 2: Calcular SVD por canal
        for (auto& canal : canales) {
            svd_por_canal.push_back(calcularSVD(canal));
        }

        imprimirProgreso(2, 6, "Calculando SVD con BDCSVD...");
        std::cout << "    Rango completo: r = " << svd_por_canal[0].rango << std::endl;

        // PASO 3: aproximaciones
        imprimirProgreso(3, 6, "Generando aproximaciones de rango bajo...");
        std::cout << std::fixed << std::setprecision(4);

        const Eigen::MatrixXd& A = canales[0];
        auto metricas = calcularTodasLasMetricas(A, svd_por_canal[0].sigma, valores_k);
        std::vector<Eigen::MatrixXd> reconstrucciones;

        for (size_t idx = 0; idx < valores_k.size(); ++idx) {
            int k = metricas[idx].k;

            std::cout << "    k=" << std::setw(3) << k << ": ";

            // Reconstruir
            Eigen::MatrixXd Ak;
            if (modo == "gris") {
                Ak = reconstruirRangoK(svd_por_canal[0], k);
            } else {
                auto canales_color = reconstruirRangoKColor(svd_por_canal, k);
                Ak = canales_color[0]; // métricas sobre primer canal
            }
            reconstrucciones.push_back(Ak);

            // Guardar PNG
            {
                std::stringstream ss;
                ss << raiz << "/resultados/reconstrucciones/ak_" << id_imagen << "_" << k << ".png";
                std::string ruta_out = ss.str();
                if (modo == "color") {
                    auto canales_color = reconstruirRangoKColor(svd_por_canal, k);
                    guardarPNGColor(ruta_out, canales_color[0], canales_color[1], canales_color[2]);
                } else {
                    guardarPNGGris(ruta_out, Ak);
                }
            }

            // Actualizar métricas con reconstrucción
            metricas[idx].error_relativo = calcularErrorRelativo(A, Ak);

            std::cout << "Error=" << std::setprecision(4) << metricas[idx].error_relativo
                      << " | Energia=" << std::setprecision(2) << metricas[idx].energia_pct << "%"
                      << " | Compresion=" << std::setprecision(1) << metricas[idx].tasa_compresion << "x"
                      << std::endl;
        }

// PASO 4: Exportar CSV
        imprimirProgreso(4, 6, "Exportando metricas a resultados/datos/metricas_" + id_imagen + ".csv");
        guardarMetricasCSV(raiz + "/resultados/datos/metricas_" + id_imagen + ".csv", metricas);
        guardarValoresSingularesCSV(raiz + "/resultados/datos/valores_singulares_" + id_imagen + ".csv",
                                     svd_por_canal[0].sigma);

        // PASO 5: Python (generar gráficos para esta imagen)
        imprimirProgreso(5, 6, "Ejecutando scripts Python para graficos...");
        {
            std::string py_dir = raiz + "/python";
            std::string cmd = "cd /d \"" + py_dir + "\" && python analisis_cuantitativo.py " + id_imagen + " 2>NUL || "
                              "python3 analisis_cuantitativo.py " + id_imagen + " 2>NUL || "
                              "echo \"[WARN] Python no disponible; omite graficos.\"";
            system(cmd.c_str());
        }

        // PASO 6: Resumen
        imprimirProgreso(6, 6, "Completado para imagen " + id_imagen);

        std::cout << "\n>>> Completado para imagen " << id_imagen << ". Revisa resultados/reconstrucciones/ y resultados/datos/\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n>>> ERROR: " << e.what() << std::endl;
        return 1;
    }
}
