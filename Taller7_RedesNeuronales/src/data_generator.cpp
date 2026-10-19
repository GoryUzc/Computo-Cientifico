#include "data_generator.hpp"
#include <random>
#include <cmath>

Dataset DataGenerator::generate(int n_samples, double noise_sigma, unsigned int seed) {
    Eigen::VectorXd X(n_samples);
    Eigen::VectorXd Y(n_samples);
    
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dist_x(-2.0, 2.0);
    std::normal_distribution<double> dist_noise(0.0, noise_sigma);
    
    for (int i = 0; i < n_samples; ++i) {
        double x = dist_x(gen);
        X(i) = x;
        double y = x * x + std::sin(5.0 * x) + dist_noise(gen);
        Y(i) = y;
    }
    
    return {X, Y};
}