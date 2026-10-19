#include <iostream>
#include <filesystem>
#include "data_generator.hpp"
#include "trainer.hpp"
#include "exporter.hpp"

namespace fs = std::filesystem;

int main() {
    // Crear carpeta resultados si no existe
    fs::create_directories("resultados");
    
    // 1. Generar dataset de entrenamiento una sola vez (seed 7, sigma 0.5)
    Dataset training_data = DataGenerator::generate(100, 0.5, 7);
    
    // Guardar datos de entrenamiento
    Exporter::export_predictions("resultados/datos_entrenamiento.csv",
                                 training_data.X, training_data.Y, training_data.Y);
    
    // 2. Generar función real para comparación (sin ruido, seed 8, ordenada)
    Dataset real_func = DataGenerator::generate(500, 0.001, 8);
    // Ordenar por x para visualizar mejor (opcional)
    std::vector<int> indices(real_func.X.size());
    for (size_t i = 0; i < indices.size(); ++i) indices[i] = i;
    
    std::sort(indices.begin(), indices.end(),
              [&](int a, int b) { return real_func.X(a) < real_func.X(b); });
    
    Eigen::VectorXd sorted_X(500);
    Eigen::VectorXd sorted_Y(500);
    for (int i = 0; i < 500; ++i) {
        sorted_X(i) = real_func.X(indices[i]);
        sorted_Y(i) = real_func.Y(indices[i]);
    }
    
    Exporter::export_predictions("resultados/funcion_real.csv",
                                 sorted_X, sorted_Y, sorted_Y);
    
    std::cout << "Generado dataset de entrenamiento (100 muestras) y función real (500 muestras) en resultados/\n\n";
    
    // 3. Parámetros del experimento
    const int hidden_size = 20;      // Neuronas ocultas
    const int epochs = 10000;         // Número de épocas
    const double convergence_threshold = 0.1; // Umbral de convergencia
    
    std::vector<unsigned int> seeds = {42, 123, 456, 789, 1024};  // 5 semillas
    std::vector<double> alphas = {0.001, 0.01, 0.5};               // 3 tasas de aprendizaje
    
    // 4. Ejecutar experimentos (5 semillas × 3 alphas = 15)
    std::cout << "Ejecutando 15 experimentos...\n";
    std::vector<ExperimentResult> results = Trainer::run_experiments(
        training_data, hidden_size, alphas, seeds, epochs, convergence_threshold
    );
    
    std::cout << "\nTodos los experimentos finalizados.\n\n";
    
    // 5. Exportar resultados de cada experimento
    int file_index = 0;
    for (const auto& result : results) {
        // Exportar historial de pérdida
        std::string loss_filename = "resultados/entrenamiento_alfa_";
        loss_filename += std::to_string(static_cast<int>(result.alpha * 1000)) + "_seed_" 
                         + std::to_string(result.seed) + ".csv";
        Exporter::export_loss_history(loss_filename, result.loss_history);
        
        // Exportar predicciones
        std::string pred_filename = "resultados/predicciones_alfa_";
        pred_filename += std::to_string(static_cast<int>(result.alpha * 1000)) + "_seed_"
                        + std::to_string(result.seed) + ".csv";
        Exporter::export_predictions(pred_filename, training_data.X, training_data.Y, result.final_predictions);
        
        file_index++;
    }
    
    // 6. Exportar resumen comparativo
    Exporter::export_summary("resultados/resumen_comparativo.csv", results);
    
    std::cout << "Resultados exportados a resultados/\n";
    
    return 0;
}