#include "trainer.hpp"
#include <iostream>

std::vector<ExperimentResult> Trainer::run_experiments(
    const Dataset& data,
    int hidden_size,
    const std::vector<double>& alphas,
    const std::vector<unsigned int>& seeds,
    int epochs,
    double convergence_threshold) {
    
    std::vector<ExperimentResult> results;
    int n = data.X.size();
    
    for (double alpha : alphas) {
        for (unsigned int seed : seeds) {
            NeuralNetwork nn(hidden_size, alpha);
            nn.initialize_weights(seed);
            
            std::vector<double> loss_history;
            loss_history.reserve(epochs + 1);
            
            // Época 0 (inicialización)
            Eigen::VectorXd Y_hat_initial = nn.forward(data.X);
            Eigen::VectorXd error_initial = Y_hat_initial - data.Y;
            double initial_loss = error_initial.squaredNorm() / (2.0 * n);
            loss_history.push_back(initial_loss);
            
            int epoch_converged = -1;
            bool converged = false;
            
            for (int epoch = 0; epoch < epochs; ++epoch) {
                double loss = nn.train_step(data.X, data.Y);
                loss_history.push_back(loss);
                
                if (!converged && loss < convergence_threshold) {
                    epoch_converged = epoch + 1; // +1 porque ya tenemos la época 0
                    converged = true;
                }
            }
            
            ExperimentResult result;
            result.seed = seed;
            result.alpha = alpha;
            result.loss_history = loss_history;
            result.final_predictions = nn.get_predictions();
            result.W1 = nn.get_W1();
            result.W2 = nn.get_W2();
            result.b1 = nn.get_b1();
            result.b2 = nn.get_b2();
            result.converged = converged;
            result.epoch_converged = epoch_converged;
            
            results.push_back(result);
            
            std::cout << "Alpha: " << alpha << ", Seed: " << seed 
                      << ", Loss final: " << loss_history.back() 
                      << ", Converged: " << (converged ? "yes" : "no")
                      << ", Epoch converged: " << epoch_converged << std::endl;
        }
    }
    
    return results;
}