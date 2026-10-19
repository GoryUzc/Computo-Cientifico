#pragma once

#include <vector>
#include <Eigen/Dense>
#include "data_generator.hpp"
#include "neural_network.hpp"

struct ExperimentResult {
    unsigned int seed;
    double alpha;
    std::vector<double> loss_history;   // epochs+1 elementos (incluye época 0)
    Eigen::VectorXd final_predictions;  // (n)
    Eigen::VectorXd W1, W2, b1;         // pesos finales
    double b2;
    bool converged;                     // loss final < umbral (0.1)
    int epoch_converged;                // primera época con loss < 0.1, -1 si nunca
};

class Trainer {
public:
    static std::vector<ExperimentResult> run_experiments(
        const Dataset& data,
        int hidden_size,
        const std::vector<double>& alphas,
        const std::vector<unsigned int>& seeds,
        int epochs,
        double convergence_threshold = 0.1);
};