# SPEC-01: Implementación C++ de la red neuronal (Backpropagation)

> **Documento de Especificación Técnica**  
> **Proyecto:** Taller 7 — Entrenamiento de un Aproximador No Lineal mediante Backpropagation   
> **Stack:** C++17, Eigen 3.4, STL `<random>`, `<fstream>`

---

## 1. Propósito del Módulo C++

Implementar desde cero el ciclo de entrenamiento de una red neuronal con **1 capa oculta (función ReLU)** para aproximar la función:

$$f(x) = x^2 + \sin(5x) + \epsilon, \quad \epsilon \sim \mathcal{N}(0, \sigma^2)$$

El código debe calcular **manualmente los gradientes (Backpropagation)** y actualizar los pesos, permitiendo ejecutar **múltiples semillas y tasas de aprendizaje ($\alpha$)** para el análisis de mínimos locales (5 semillas × 3 $\alpha$ = **15 experimentos**).

---

## 2. Arquitectura de Datos (Eigen)

Para procesar lotes (batches) de $n$ muestras de manera vectorizada y eficiente:

- `X`: `Eigen::VectorXd` de tamaño $n$ (entradas).
- `Y`: `Eigen::VectorXd` de tamaño $n$ (salidas reales).
- `W1`: `Eigen::VectorXd` de tamaño $H$ (pesos capa oculta).
- `b1`: `Eigen::VectorXd` de tamaño $H$ (sesgos capa oculta).
- `Z1`, `A1`: `Eigen::MatrixXd` de tamaño $n \times H$ (pre-activación y activación ReLU).
- `W2`: `Eigen::VectorXd` de tamaño $H$ (pesos capa salida).
- `b2`: `double` (sesgo capa salida).
- `Y_hat`: `Eigen::VectorXd` de tamaño $n$ (predicciones).

---

## 3. Especificación de los Módulos C++

### 3.1 Módulo: `data_generator.hpp` / `.cpp`

**Responsabilidad:** Generar $n$ puntos en $x \in [-2, 2]$ y calcular $y = x^2 + \sin(5x) + \mathcal{N}(0, \sigma^2)$.

```cpp
struct Dataset {
    Eigen::VectorXd X;
    Eigen::VectorXd Y;
};

class DataGenerator {
public:
    static Dataset generate(int n_samples, double noise_sigma, unsigned int seed);
};
```

### 3.2 Módulo: `neural_network.hpp` / `.cpp`

**Responsabilidad:** Forward pass, Backward pass y actualización de pesos.

```cpp
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
```

#### 🧠 Implementación Clave: Backpropagation (C++ con Eigen)

```cpp
double NeuralNetwork::train_step(const Eigen::VectorXd& X, const Eigen::VectorXd& Y) {
    int n = X.size();

    // 1. FORWARD PASS
    // Z1 = X * W1^T + b1  -> (n x H)
    Z1_ = X.replicate(1, H_) * W1_.asDiagonal()
        + b1_.transpose().replicate(n, 1);
    A1_ = Z1_.cwiseMax(0.0);                    // ReLU
    Y_hat_ = A1_ * W2_ + Eigen::VectorXd::Constant(n, b2_);

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
```

### 3.3 Módulo: `trainer.hpp` / `.cpp`

**Responsabilidad:** Ejecutar el experimento completo (5 semillas × 3 tasas de aprendizaje) y guardar el historial de pérdidas.

```cpp
struct ExperimentResult {
    unsigned int seed;
    double alpha;
    std::vector<double> loss_history;   // epochs+1 elementos (incluye época 0)
    Eigen::VectorXd final_predictions;  // (n)
    Eigen::VectorXd W1, W2, b1;         // pesos finales (análisis de mínimos)
    double b2;
    bool converged;                     // alguna pérdida entrenada < 0.1 (no evalúa época 0)
    int epoch_converged;                // primera época entrenada < 0.1, -1 si nunca
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
```

### 3.4 Módulo: `exporter.hpp` / `.cpp`

**Responsabilidad:** Guardar los resultados en formatos que Python pueda leer fácilmente.

```cpp
class Exporter {
public:
    // entrenamiento_alfa_<código>_seed_<s>.csv : epoch,loss
    static void export_loss_history(const std::string& filename,
                                    const std::vector<double>& loss_history);

    // x,y_real,y_pred
    static void export_predictions(const std::string& filename,
                                   const Eigen::VectorXd& X,
                                   const Eigen::VectorXd& Y_real,
                                   const Eigen::VectorXd& Y_pred);

    // resumen_comparativo.csv :
    // alpha,seed,loss_final,convergio,epoca_convergencia
    static void export_summary(const std::string& filename,
                               const std::vector<ExperimentResult>& results);
};
```

---

## 4. Parámetros del Experimento 

| Parámetro | Valor | Justificación |
|-----------|-------|---------------|
| **Función objetivo** | $f(x) = x^2 + \sin(5x)$ | Función no lineal del taller |
| **Ruido ($\sigma$)** | **0.5** | Gaussiano, ~10% de la amplitud de $f$ |
| **Dominio** | $x \in [-2, 2]$ | Rango pedido por el taller |
| **Tamaño de muestra ($n$)** | 100 | Suficientes para capturar la función |
| **Neuronas ocultas ($H$)** | **20** | Suficiente para aproximar sin sobreajuste |
| **Épocas** | **10000** | Configurado en `src/main.cpp` |
| **Semillas** | `{42, 123, 456, 789, 1024}` | 5 ejecuciones (análisis de mínimos locales) |
| **Tasas de aprendizaje ($\alpha$)** | `{0.001, 0.01, 0.5}` | Lento, moderado, agresivo |
| **Inicialización** | **He**: $\mathcal{N}(0, \sqrt{2/H})$ para pesos y sesgos ocultos; $b_2=0$ | Correcta para activación ReLU |
| **Dataset de entrenamiento** | Generado **una sola vez** (seed fija = 7) | Garantiza comparación justa entre los 15 experimentos |
| **Umbral de convergencia** | loss < 0.1 | Se registra la primera época que alcanza el umbral; -1 si no ocurre |

**Total de experimentos:** 5 semillas × 3 $\alpha$ = **15 ejecuciones**

---

## 5. Estructura de Salida (carpeta `resultados/`)

```
resultados/
├── datos_entrenamiento.csv          # x, y_real, y_pred (100 filas con ruido)
├── funcion_real.csv                 # x, y_real, y_pred (500 puntos ordenados, sigma=0.001)
├── entrenamiento_alfa_1_seed_42.csv      # epoch, loss (10001 filas)
├── ... (15 archivos de historial de pérdida)
├── predicciones_alfa_1_seed_42.csv       # x, y_real, y_pred
├── ... (15 archivos de predicciones)
└── resumen_comparativo.csv          # alpha, seed, loss_final, convergio, epoca_convergencia
```

El `<código>` de la tasa de aprendizaje es `int(alpha * 1000)`: `1` para `0.001`, `10` para `0.01` y `500` para `0.5`.

El historial de pérdida de cada época se calcula durante el paso hacia delante, antes de actualizar los parámetros. `final_predictions` se conserva desde el último `train_step`, por lo que también corresponde al paso hacia delante anterior a la actualización final.

El programa C++ crea `resultados/` si no existe. Las rutas son relativas al directorio de trabajo desde el que se ejecuta el binario.

El exportador C++ escribe `resumen_comparativo.csv` con el encabezado `alpha,seed,loss_final,convergio,epoca_convergencia`. Al ejecutarse después el visualizador Python, este reemplaza el mismo archivo y lo ordena por pérdida final, usando las columnas `alpha,seed,loss_final,converges,epoch_converged`.
---

*Fin del documento SPEC-001.md*

---