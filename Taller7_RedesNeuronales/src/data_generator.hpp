#pragma once

#include <Eigen/Dense>

struct Dataset {
    Eigen::VectorXd X;
    Eigen::VectorXd Y;
};

class DataGenerator {
public:
    static Dataset generate(int n_samples, double noise_sigma, unsigned int seed);
};