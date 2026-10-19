#pragma once

#include <Eigen/Dense>

class NeuralNetwork {
public:
    NeuralNetwork(int hidden_size, double alpha);
    
    // Inicialización He para ReLU: N(0, sqrt(2/H))
    void initialize_weights(unsigned int seed);
    
    // Forward Pass
    Eigen::VectorXd forward(const Eigen::VectorXd& X);
    
    // Backward Pass y Update (devuelve el costo J)
    double train_step(const Eigen::VectorXd& X, const Eigen::VectorXd& Y);
    
    // Getters
    Eigen::VectorXd get_predictions() const { return Y_hat_; }
    Eigen::VectorXd get_W1() const { return W1_; }
    Eigen::VectorXd get_W2() const { return W2_; }
    Eigen::VectorXd get_b1() const { return b1_; }
    double get_b2() const { return b2_; }
    
private:
    int H_;                  // Número de neuronas ocultas
    double alpha_;           // Tasa de aprendizaje
    
    // Parámetros
    Eigen::VectorXd W1_, b1_;
    Eigen::VectorXd W2_;
    double b2_;
    
    // Variables de estado para backprop
    Eigen::VectorXd X_, Y_, Y_hat_;
    Eigen::MatrixXd Z1_, A1_;
};