#include "neural_network.hpp"
#include <random>

NeuralNetwork::NeuralNetwork(int hidden_size, double alpha)
    : H_(hidden_size), alpha_(alpha),
      W1_(Eigen::VectorXd::Zero(hidden_size)),
      b1_(Eigen::VectorXd::Zero(hidden_size)),
      W2_(Eigen::VectorXd::Zero(hidden_size)),
      b2_(0.0) {}

void NeuralNetwork::initialize_weights(unsigned int seed) {
    std::mt19937 gen(seed);
    double he_std = std::sqrt(2.0 / static_cast<double>(H_));
    std::normal_distribution<double> dist(0.0, he_std);
    
    for (int i = 0; i < H_; ++i) {
        W1_(i) = dist(gen);
        b1_(i) = dist(gen);
        W2_(i) = dist(gen);
    }
    b2_ = 0.0; // Bias de salida se inicializa a 0
}

Eigen::VectorXd NeuralNetwork::forward(const Eigen::VectorXd& X) {
    int n = X.size();
    
    // Z1 = X * W1^T + b1  -> (n x H)
    Z1_ = X.replicate(1, H_) * W1_.asDiagonal()
        + b1_.transpose().replicate(n, 1);
    A1_ = Z1_.cwiseMax(0.0);                    // ReLU
    Y_hat_ = A1_ * W2_ + Eigen::VectorXd::Constant(n, b2_);
    
    return Y_hat_;
}

double NeuralNetwork::train_step(const Eigen::VectorXd& X, const Eigen::VectorXd& Y) {
    int n = X.size();
    
    // 1. FORWARD PASS
    // Z1 = X * W1^T + b1  -> (n x H)
    Z1_ = X.replicate(1, H_) * W1_.asDiagonal()
        + b1_.transpose().replicate(n, 1);
    A1_ = Z1_.cwiseMax(0.0);                    // ReLU
    Y_hat_ = A1_ * W2_ + Eigen::VectorXd::Constant(n, b2_);
    
    X_ = X;
    Y_ = Y;
    
    // Costo J = (1/2n) * ||Y_hat - Y||^2
    Eigen::VectorXd error = Y_hat_ - Y;
    double J = error.squaredNorm() / (2.0 * n);
    
    // 2. BACKWARD PASS
    // delta_out = (1/n)(Y_hat - Y)        (n)
    Eigen::VectorXd delta_out = error / static_cast<double>(n);
    
    // Gradientes capa de salida
    Eigen::VectorXd dW2 = A1_.transpose() * delta_out;   // (H)
    double db2 = delta_out.sum();
    
    // delta_hidden = (delta_out * W2^T) ⊙ (Z1 > 0)      (n x H)
    Eigen::MatrixXd delta_hidden =
        (delta_out * W2_.transpose()).array()
        * (Z1_.array() > 0.0).cast<double>();
    
    // Gradientes capa oculta
    Eigen::VectorXd dW1 = delta_hidden.transpose() * X_; // (H)
    Eigen::VectorXd db1 = delta_hidden.colwise().sum();  // (H)
    
    // 3. UPDATE (Descenso de Gradiente)
    W1_ -= alpha_ * dW1;
    b1_ -= alpha_ * db1;
    W2_ -= alpha_ * dW2;
    b2_ -= alpha_ * db2;
    
    return J;
}