#include <iostream>
#include <exception>
#include "cargador_csv.hpp"
#include "procesador_pca.hpp"
#include "svd_calculador.hpp"
#include "exportador.hpp"
#include "utils.hpp"

int main() {
    try {
        const std::string ruta_csv = "C:/Users/User/OneDrive/Desktop/Ejercicios dev/Computo cientifico/ComputoCientifico/Taller2_PCA_AncestreriaGenetica/data/datos_genotipo.csv";
        const std::string ruta_pca = "C:/Users/User/OneDrive/Desktop/Ejercicios dev/Computo cientifico/ComputoCientifico/Taller2_PCA_AncestreriaGenetica/resultados/resultado_pca.csv";
        const std::string ruta_var = "C:/Users/User/OneDrive/Desktop/Ejercicios dev/Computo cientifico/ComputoCientifico/Taller2_PCA_AncestreriaGenetica/resultados/varianza.csv";

        std::cout << "=== PASO 1: Cargar datos ===" << std::endl;
        DatosGenotipo datos = cargarCSV(ruta_csv);
        std::cout << "SNPs originales: " << datos.n_snps
                  << ", Individuos: " << datos.n_individuos << std::endl;

        std::cout << "\n=== PASO 2: Filtrar SNPs constantes ===" << std::endl;
        ResultadoFiltrado filtrado = filtrarSNPsConstantes(datos.n_snps, datos.n_individuos, datos.valores);
        std::cout << "SNPs filtrados: " << filtrado.n_snps_filtrados << std::endl;

        std::cout << "\n=== PASO 3: Normalizar Patterson ===" << std::endl;
        ResultadoNormalizado normalizado = normalizarPatterson(filtrado);

        std::cout << "\n=== PASO 4: Calcular PCA ===" << std::endl;
        Eigen::VectorXd valores_singulares;
        double var_pc1, var_pc2;
        Eigen::MatrixXd PC = calcularPCA(normalizado.g_tilde, normalizado.n_snps_filtrados,
                                         normalizado.n_individuos, valores_singulares, var_pc1, var_pc2);
        std::cout << "Varianza PC1: " << var_pc1 << "%, PC2: " << var_pc2 << "%" << std::endl;

        std::cout << "\n=== PASO 5: Exportar resultados ===" << std::endl;
        exportarPCA(ruta_pca, PC, datos.nombres_individuos);
        exportarVarianza(ruta_var, var_pc1, var_pc2);
        std::cout << "Exportados: " << ruta_pca << ", " << ruta_var << std::endl;

        std::cout << "\nPipeline completado con exito." << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}
