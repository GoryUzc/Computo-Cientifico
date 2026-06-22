// ensamblador.cpp
#include "ensamblador.hpp"
#include <fstream>
#include <vector>

Ensamblador::Ensamblador(const Malla& malla, 
                         const CondicionesFrontera& fronteras,
                         const FuenteCalor& fuente)
    : malla_(malla), fronteras_(fronteras), fuente_(fuente) {}

Eigen::SparseMatrix<double> Ensamblador::construir_matriz_A() {
    int n = malla_.get_n();
    int N = malla_.get_N();
    Eigen::SparseMatrix<double> A(N, N);
    
    // Usar Triplets para evitar assertion failures
    std::vector<Eigen::Triplet<double>> triplets;
    triplets.reserve(5 * N);
    
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            int k = malla_.indice_2d_a_1d(i, j);  // Global index [0, N-1]
            
            // Diagonal principal
            triplets.emplace_back(k, k, 4.0);
            
            // Vecino izquierdo (i, j-1) -> k-1
            if (j > 1) {
                triplets.emplace_back(k, k - 1, -1.0);
            }
            
            // Vecino derecho (i, j+1) -> k+1
            if (j < n) {
                triplets.emplace_back(k, k + 1, -1.0);
            }
            
            // Vecino superior (i-1, j) -> k-n
            if (i > 1) {
                triplets.emplace_back(k, k - n, -1.0);
            }
            
            // Vecino inferior (i+1, j) -> k+n
            if (i < n) {
                triplets.emplace_back(k, k + n, -1.0);
            }
        }
    }
    
    A.setFromTriplets(triplets.begin(), triplets.end());
    A.makeCompressed();  // Finalizar estructura
    
    return A;
}

Eigen::VectorXd Ensamblador::construir_vector_b() {
    int n = malla_.get_n();
    int N = malla_.get_N();
    Eigen::VectorXd b(N);
    b.setZero();
    
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            int k = malla_.indice_2d_a_1d(i, j);
            
            // Fronteras
            if (i == 1) b[k] += fronteras_.Tleft;
            if (i == n) b[k] += fronteras_.Tright;
            if (j == 1) b[k] += fronteras_.Tinf;
            if (j == n) b[k] += fronteras_.Tsupper;
            
            // Fuente
            if (fuente_.activa && i == fuente_.nodo_i && j == fuente_.nodo_j) {
                b[k] += (malla_.get_h() * malla_.get_h()) * fuente_.intensidad;
            }
        }
    }
    
    return b;
}

void Ensamblador::exportar_patron_matriz(const std::string& archivo, 
                                         const Eigen::SparseMatrix<double>& A) {
    std::ofstream ofs(archivo);
    if (!ofs.is_open()) {
        throw std::runtime_error("No se pudo abrir archivo");
    }
    
    for (int row = 0; row < A.rows(); ++row) {
        for (Eigen::SparseMatrix<double>::InnerIterator it(A, row); it; ++it) {
            ofs << row << "," << it.col() << "," << it.value() << "\n";
        }
    }
}