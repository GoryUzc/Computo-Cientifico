#include "exporter.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

void Exporter::export_loss_history(const std::string& filename,
                                   const std::vector<double>& loss_history) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    file << "epoch,loss\n";
    for (size_t i = 0; i < loss_history.size(); ++i) {
        file << i << "," << loss_history[i] << "\n";
    }
    file.close();
}

void Exporter::export_predictions(const std::string& filename,
                                  const Eigen::VectorXd& X,
                                  const Eigen::VectorXd& Y_real,
                                  const Eigen::VectorXd& Y_pred) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    file << "x,y_real,y_pred\n";
    for (int i = 0; i < X.size(); ++i) {
        file << X(i) << "," << Y_real(i) << "," << Y_pred(i) << "\n";
    }
    file.close();
}

void Exporter::export_summary(const std::string& filename,
                              const std::vector<ExperimentResult>& results) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    
    file << "alpha,seed,loss_final,convergio,epoca_convergencia\n";
    for (const auto& result : results) {
        file << std::fixed << std::setprecision(6)
             << result.alpha << ","
             << result.seed << ","
             << result.loss_history.back() << ","
             << (result.converged ? "true" : "false") << ","
             << result.epoch_converged << "\n";
    }
    file.close();
}