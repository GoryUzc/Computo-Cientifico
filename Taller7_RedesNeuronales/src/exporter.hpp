#pragma once

#include <vector>
#include <string>
#include <Eigen/Dense>
#include "trainer.hpp"

class Exporter {
public:
    static void export_loss_history(const std::string& filename,
                                    const std::vector<double>& loss_history);
    
    static void export_predictions(const std::string& filename,
                                   const Eigen::VectorXd& X,
                                   const Eigen::VectorXd& Y_real,
                                   const Eigen::VectorXd& Y_pred);
    
    static void export_summary(const std::string& filename,
                               const std::vector<ExperimentResult>& results);
};