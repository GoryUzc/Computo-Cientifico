# ARCH_DESINGH.md: Taller 7 — Red Neuronal con Backpropagation

> Documento de arquitectura y diseño para implementación de red neuronal con 1 capa oculta   
> **Problema:** Aproximación de $f(x) = x^2 + \sin(5x)$ mediante red neuronal entrenada con backpropagation manual

---

## 1. Stack Tecnológico

| Capa | Tecnología | Versión | Propósito |
|------|-----------|---------|-----------|
| **Cálculo numérico** | C++17 | ISO/IEC 14882:2017 | Núcleo: forward/backward pass |
| **Álgebra lineal** | Eigen 3.4 | 3.4.0+ | Operaciones matriciales vectorizadas |
| **Generación aleatoria** | `<random>` (STL) | C++11+ | Inicialización de pesos y ruido |
| **Build** | CMake | 3.10+ | Generación de targets |
| **Exportación** | `std::ofstream` (STL) | C++11+ | Escritura de resultados CSV |
| **Visualización** | Python 3 | Python 3 | Gráficas de convergencia y ajuste |
| **Gráficos** | matplotlib | 3.7+ | Curvas de pérdida y ajuste |
| **Análisis datos** | numpy, pandas | numpy 1.24+, pandas 2.0+ | Carga y procesamiento de CSV |
| **Informe** | LaTeX | - | Documento técnico profesional |

---

## 2. Objetivo

Implementar una red neuronal de **1 capa oculta con $H$ neuronas** en C++17 con Eigen para aproximar la función:

$$f(x) = x^2 + \sin(5x)$$

El proyecto debe:

1. **Generar $N=100$ puntos** de entrenamiento con ruido gaussiano $\sigma = 0.5$
2. **Inicializar pesos** con **5 semillas aleatorias distintas** (para análisis de mínimos locales)
3. **Implementar forward pass** con activación ReLU
4. **Implementar backward pass** (backpropagation manual) con pérdida cuadrática media escalada por 1/2
5. **Entrenar** con 3 valores de $\alpha$ (0.001, 0.01, 0.5)
6. **Analizar mínimos locales** (5 ejecuciones × 3 $\alpha$ = 15 experimentos totales)
7. **Exportar resultados** a CSV para visualización en Python

---

## 3. Fundamento Matemático

### 3.1 Arquitectura de la Red

```
Entrada x (escalar)
    │
    ▼
┌─────────────────────────────────────┐
│  Capa Oculta (H neuronas con ReLU)  │
│  z₁ = W₁·x + b₁                     │
│  a₁ = max(0, z₁)                    │
└─────────────────────────────────────┘
    │
    ▼
┌─────────────────────────────────────┐
│  Capa de Salida (lineal)            │
│  ŷ = W₂·a₁ + b₂                     │
└─────────────────────────────────────┘
    │
    ▼
  Salida ŷ (escalar)
```

### 3.2 Dimensiones de las Variables

| Variable | Dimensión | Descripción |
|----------|-----------|-------------|
| $x$ | $(n, 1)$ | Entradas (batch de $n$ muestras) |
| $W_1$ | $(1, H)$ | Pesos capa oculta |
| $b_1$ | $(1, H)$ | Sesgos capa oculta |
| $W_2$ | $(H, 1)$ | Pesos capa salida |
| $b_2$ | escalar | Sesgo capa salida |
| $z_1$ | $(n, H)$ | Pre-activaciones capa oculta |
| $a_1$ | $(n, H)$ | Activaciones (post-ReLU) |
| $\hat{y}$ | $(n, 1)$ | Predicciones |

### 3.3 Forward Pass

$$
\begin{align*}
Z_1 &= X \cdot W_1 + \mathbf{1} \cdot b_1 \quad \in \mathbb{R}^{n \times H} \\
A_1 &= \max(0, Z_1) \quad \text{(ReLU elemento a elemento)} \\
\hat{Y} &= A_1 \cdot W_2 + b_2 \quad \in \mathbb{R}^{n \times 1}
\end{align*}
$$

### 3.4 Función de Costo (MSE)

$$J = \frac{1}{2n} \sum_{i=1}^{n} (\hat{y}_i - y_i)^2$$

El factor $\frac{1}{2}$ se añade por conveniencia matemática: al derivar $(\hat{y}-y)^2$, el 2 baja y cancela el $\frac{1}{2}$, dejando una derivada limpia.

### 3.5 Backward Pass (Backpropagation)

Aplicando la regla de la cadena, de la salida hacia la entrada:

**Paso A: Gradiente en la salida**
$$\delta_{out} = \frac{\partial J}{\partial \hat{Y}} = \frac{1}{n}(\hat{Y} - Y) \in \mathbb{R}^{n \times 1}$$

**Paso B: Gradientes de la capa de salida**
$$\frac{\partial J}{\partial W_2} = A_1^T \cdot \delta_{out} \in \mathbb{R}^{H \times 1}$$
$$\frac{\partial J}{\partial b_2} = \sum_{i=1}^{n} \delta_{out}^{(i)} \in \mathbb{R}$$

**Paso C: Gradiente en la capa oculta**
$$\delta_{hidden} = (\delta_{out} \cdot W_2^T) \odot f'(Z_1) \in \mathbb{R}^{n \times H}$$

Donde:
- $\odot$ es la multiplicación de Hadamard (elemento a elemento)
- $f'(z) = \begin{cases} 1 & \text{si } z > 0 \\ 0 & \text{si } z \leq 0 \end{cases}$ (derivada de ReLU)

**Paso D: Gradientes de la capa oculta**
$$\frac{\partial J}{\partial W_1} = X^T \cdot \delta_{hidden} \in \mathbb{R}^{1 \times H}$$
$$\frac{\partial J}{\partial b_1} = \sum_{i=1}^{n} \delta_{hidden}^{(i)} \in \mathbb{R}^{1 \times H}$$

### 3.6 Actualización de Pesos (Descenso de Gradiente)

$$
\begin{align*}
W_1 &\leftarrow W_1 - \alpha \cdot \frac{\partial J}{\partial W_1} \\
b_1 &\leftarrow b_1 - \alpha \cdot \frac{\partial J}{\partial b_1} \\
W_2 &\leftarrow W_2 - \alpha \cdot \frac{\partial J}{\partial W_2} \\
b_2 &\leftarrow b_2 - \alpha \cdot \frac{\partial J}{\partial b_2}
\end{align*}
$$

### 3.7 Análisis de Mínimos Locales

La función de costo $J(W_1, b_1, W_2, b_2)$ es **no convexa** debido a:
1. La no linealidad de ReLU
2. El término $\sin(5x)$ en la función objetivo
3. La composición de transformaciones

**Consecuencia:** Diferentes inicializaciones aleatorias conducen a diferentes mínimos locales. Esto se estudiará ejecutando el entrenamiento **5 veces con semillas distintas**.

---

## 4. Parámetros de Configuración

| Parámetro | Valor | Justificación |
|-----------|-------|---------------|
| $N$ (puntos de entrenamiento) | 100 | Suficientes para capturar la función |
| Dominio $x$ | $[-2, 2]$ | Rango pedido por el taller |
| $\sigma$ (ruido) | 0.5 | 10% de la amplitud de $f$ |
| $H$ (neuronas ocultas) | 20 | Suficiente para aproximar sin sobreajuste |
| Épocas | 10000 | Parámetro configurado en `src/main.cpp` |
| $\alpha_1$ | 0.001 | Aprendizaje muy lento |
| $\alpha_2$ | 0.01 | Aprendizaje moderado |
| $\alpha_3$ | 0.5 | Aprendizaje agresivo  |
| **Semillas** | `{42, 123, 456, 789, 1024}` | Análisis de diferentes inicializaciones |
| Inicialización | $\mathcal{N}(0, \sqrt{2/H})$ para pesos y sesgos ocultos; $b_2=0$ | He para la activación ReLU |
| Datos de entrenamiento | 100 muestras, semilla 7 | El mismo conjunto se reutiliza en los 15 experimentos |
| Umbral de convergencia | pérdida menor que 0.1 | Registra la primera época entrenada que satisface el criterio; la época inicial 0 no se evalúa para convergencia en C++ |

### 4.1 Diseño del Experimento de Mínimos Locales

```
Para cada tasa α ∈ {0.001, 0.01, 0.5}:
    Para cada semilla s ∈ {42, 123, 456, 789, 1024}:
        1. Inicializar la red con semilla s
        2. Registrar la pérdida inicial (época 0)
        3. Ejecutar 10000 pasos de entrenamiento por lotes
        4. Registrar la primera época con pérdida < 0.1; -1 si no ocurre
        5. Exportar historial y predicciones finales a CSV
```

**Total de experimentos:** 5 semillas × 3 $\alpha$ = **15 ejecuciones**

---

## 5. Estructura del Proyecto

```
Taller7_RedesNeuronales/
├── CMakeLists.txt
├── README.md
├── SSD/
│   ├── SPECS/
│   │   ├── spec-01.md
│   │   └── spec-02.md
│   └── ARCH_DESINGH.md
├── src/
│   ├── main.cpp
│   ├── data_generator.hpp/cpp
│   ├── neural_network.hpp/cpp
│   ├── trainer.hpp/cpp
│   └── exporter.hpp/cpp
├── resultados/
│   ├── datos_entrenamiento.csv
│   ├── funcion_real.csv
│   ├── entrenamiento_alfa_1_seed_42.csv
│   ├── predicciones_alfa_1_seed_42.csv
│   ├── resumen_comparativo.csv
│   └── graficos/
├── python/
│   ├── visualizar_red.py
│   └── requirements.txt
└── Docs/
```

---

## 6. Contratos de Datos CSV

### 6.1 `datos_entrenamiento.csv`
```csv
x,y_real,y_pred
-1.234567,-0.823456,-0.823456
0.987654,1.456789,1.456789
...
```
**Filas:** 100. `y_real` y `y_pred` coinciden aquí porque se exporta el valor generado con ruido como ambas columnas.

### 6.2 `funcion_real.csv`
```csv
x,y_real,y_pred
-2.000000,4.000000,4.000000
-1.990000,3.985...,3.985...
...
```
**Filas:** 500. Se generan con semilla 8 y desviación de ruido 0.001, y luego se ordenan por `x`; no es una malla exacta ni una evaluación completamente libre de ruido.

### 6.3 Historial `entrenamiento_alfa_<código>_seed_<semilla>.csv`
```csv
epoch,loss
0,12.345678
1,11.234567
...
10000,0.012345
```
El ejemplo muestra las columnas del CSV real; los valores de pérdida no incluyen una columna de predicción. **Filas:** 10001 (época 0 + 10000 épocas).

El código de la tasa se forma con `int(alpha * 1000)`: `1` para `0.001`, `10` para `0.01` y `500` para `0.5`.

La pérdida de cada época se mide en el paso hacia delante de `train_step`, antes de actualizar los parámetros. El vector `final_predictions` también se obtiene de la salida guardada durante el último `train_step`.

### 6.4 Predicciones `predicciones_alfa_<código>_seed_<semilla>.csv`
```csv
x,y_real,y_pred
-1.234567,-0.823456,-0.712345
0.987654,1.456789,1.234567
...
```
Las predicciones finales corresponden a las 100 muestras de entrenamiento.

### 6.5 `resumen_comparativo.csv`
El exportador C++ genera una fila por experimento con el encabezado:

```csv
alpha,seed,loss_final,convergio,epoca_convergencia
0.001000,42,0.358726,false,-1
...
```

El script Python de visualización vuelve a generar este archivo, ordenado por `loss_final`, con las columnas `alpha,seed,loss_final,converges,epoch_converged`. El contenido final depende de cuál proceso se ejecute en último lugar.

---

## 7. Referencias

1. **Goodfellow, I., Bengio, Y., & Courville, A.** (2016). *Deep Learning*. MIT Press. (Capítulo 6: Redes neuronales profundas)

2. **Bishop, C. M.** (2006). *Pattern Recognition and Machine Learning*. Springer. (Capítulo 5: Redes neuronales)

3. **Rumelhart, D. E., Hinton, G. E., & Williams, R. J.** (1986). *Learning representations by back-propagating errors*. Nature, 323(6088), 533-536.

4. **Eigen Documentation.** *Dense matrix and array manipulation.* https://eigen.tuxfamily.org/dox/

---

**Fin del documento ARCH_DESINGH.md**

---