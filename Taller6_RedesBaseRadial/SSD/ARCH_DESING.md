# ARCH_DESIGN.md: Taller 6 — Interpolación mediante Redes de Base Radial (RBF)

> Documento de arquitectura y diseño para implementación de interpolación RBF  
> **Metodología:** SDD (Specification-Driven Development)  
> **Problema:** Aproximación de f(x,y) = sin(x)cos(y) mediante kernel Gaussiano con regularización de Tikhonov

---

## 1. Stack Tecnológico

| Capa | Tecnología | Versión | Propósito |
|------|-----------|---------|-----------|
| **Cálculo numérico** | C++17 | ISO/IEC 14882:2017 | Núcleo: construcción de Φ, solver lineal |
| **Álgebra lineal** | Eigen 3.4 | 3.4.0+ | Matrices densas, descomposición LDLT/QR |
| **Generación aleatoria** | `<random>` (STL) | C++11+ | Distribución uniforme y normal |
| **Build** | CMake | 3.16+ | Generación de targets |
| **Exportación** | `std::ofstream` (STL) | C++11+ | Escritura de resultados CSV |
| **Visualización** | Python 3.10+ | 3.10+ | Superficies 3D, scatter plots |
| **Gráficos** | matplotlib + mpl_toolkits | 3.7+ | `plot_surface`, `contourf` |
| **Análisis datos** | numpy, pandas | 2.0+ | Carga y procesamiento de CSV |
| **Informe** | LaTeX | - | Documento técnico profesional |

---

## 2. Objetivo

Implementar un interpolador de **Redes de Base Radial (RBF)** en C++17 con Eigen para aproximar la función objetivo:

$$f(x, y) = \sin(x)\cos(y), \quad (x, y) \in [-2, 2]^2$$

El proyecto debe:

1. **Generar N=100 puntos de entrenamiento** con ruido gaussiano σ=0.1
2. **Construir la matriz de Gram** Φ ∈ ℝ^(N×N) usando kernel Gaussiano
3. **Aplicar regularización de Tikhonov** con λ=10⁻⁶ para estabilizar la inversión
4. **Resolver el sistema lineal** (Φ + λI)w = y para obtener los pesos w
5. **Evaluar la superficie** en una malla regular de 50×50 puntos
6. **Estudiar el efecto del parámetro de forma ε** con 3 valores: pequeño, óptimo y grande
7. **Exportar resultados a CSV** para visualización en Python

---

## 3. Fundamento Matemático

### 3.1 Función de Aproximación RBF

Dado un conjunto de N centros $\{\mathbf{x}_j\}_{j=1}^N \subset \mathbb{R}^2$, la superficie interpolante es:

$$s(\mathbf{x}) = \sum_{j=1}^{N} w_j \phi(\|\mathbf{x} - \mathbf{x}_j\|)$$

donde:
- $\mathbf{x} = (x, y) \in \mathbb{R}^2$ es el punto a evaluar
- $\mathbf{x}_j$ es el j-ésimo centro (punto de entrenamiento)
- $w_j$ es el peso asociado al centro j (incógnita)
- $\phi(r)$ es la función de base radial

### 3.2 Kernel Gaussiano

$$\phi(r) = \exp\left(-(\epsilon r)^2\right)$$

Donde:
- $r = \|\mathbf{x} - \mathbf{x}_j\| = \sqrt{(x - x_j)^2 + (y - y_j)^2}$ es la distancia euclidiana
- $\epsilon > 0$ es el **parámetro de forma** (shape parameter)

**Propiedades:**
- $\phi(0) = 1$ (máximo en el centro)
- $\phi(r) \to 0$ cuando $r \to \infty$ (decaimiento exponencial)
- $\epsilon$ pequeño → campana ancha (influencia global)
- $\epsilon$ grande → campana estrecha (influencia local)

### 3.3 Sistema Lineal de Interpolación

Imponiendo la condición de interpolación exacta $s(\mathbf{x}_i) = y_i$ para cada punto de entrenamiento:

$$\sum_{j=1}^{N} w_j \phi(\|\mathbf{x}_i - \mathbf{x}_j\|) = y_i, \quad i = 1, \dots, N$$

En forma matricial:

$$\Phi \mathbf{w} = \mathbf{y}$$

Donde:
- $\Phi \in \mathbb{R}^{N \times N}$ es la **matriz de Gram** (o matriz de interpolación)
- $\Phi_{ij} = \phi(\|\mathbf{x}_i - \mathbf{x}_j\|) = \exp\left(-\epsilon^2 \|\mathbf{x}_i - \mathbf{x}_j\|^2\right)$
- $\mathbf{w} = [w_1, \dots, w_N]^T$ es el vector de pesos
- $\mathbf{y} = [y_1, \dots, y_N]^T$ es el vector de valores objetivo

**Propiedades de Φ:**
- **Simétrica:** $\Phi_{ij} = \Phi_{ji}$
- **Definida positiva** (para centros distintos y ε > 0)
- **Diagonal unitaria:** $\Phi_{ii} = \phi(0) = 1$

### 3.4 Regularización de Tikhonov

**Problema:** La matriz Φ puede estar **mal condicionada** cuando:
- ε es muy pequeño (campanas muy anchas → filas casi idénticas)
- Puntos de entrenamiento están muy cercanos

**Solución:** Añadir un término de regularización a la diagonal:

$$(\Phi + \lambda I) \mathbf{w} = \mathbf{y}$$

Donde:
- $I$ es la matriz identidad N×N
- $\lambda > 0$ es el **parámetro de regularización** (típicamente $\lambda = 10^{-6}$)

**Efecto:**
- Empuja los valores propios de Φ lejos de cero
- Garantiza que $(\Phi + \lambda I)$ sea definida positiva e invertible
- Intercambia interpolación exacta por aproximación suave (trade-off sesgo-varianza)
- Permite ignorar el ruido gaussiano en los datos

### 3.5 Evaluación en la Malla

Para cada punto $\mathbf{x}_k$ de la malla de evaluación (50×50 = 2500 puntos):

$$s(\mathbf{x}_k) = \sum_{j=1}^{N} w_j \exp\left(-\epsilon^2 \|\mathbf{x}_k - \mathbf{x}_j\|^2\right)$$

**Costo computacional:** O(M × N) = 2500 × 100 = 250,000 evaluaciones del kernel.

### 3.6 Efecto del Parámetro ε

| ε | Comportamiento | Condicionamiento | Resultado |
|---|----------------|------------------|-----------|
| **Pequeño** (0.5) | Campanas anchas | Mal condicionado | Oscilaciones (Runge) |
| **Óptimo** (1.5-2.0) | Balance | Bien condicionado | Superficie suave |
| **Grande** (5.0-10.0) | Campanas estrechas | Cercano a identidad | Picos aislados |

---

## 4. Parámetros de Configuración

| Parámetro | Valor | Justificación |
|-----------|-------|---------------|
| N (puntos de entrenamiento) | 100 | Balance precisión/costo |
| Dominio | $[-2, 2]^2$ | Rango estándar |
| σ (ruido) | 0.1 | 10% de la amplitud de f |
| λ (regularización) | 10⁻⁶ | Estabilidad sin sesgo excesivo |
| Resolución malla | 50×50 | 2500 puntos de evaluación |
| Semilla aleatoria | 42 | Reproducibilidad |
| ε₁ (pequeño) | 0.5 | Estudio de oscilaciones |
| ε₂ (óptimo) | 1.5 | Balance suavidad/precisión |
| ε₃ (grande) | 5.0 | Estudio de sobreajuste local |

---

## 5. Estructura del Proyecto

```
Taller6_RedesBaseRadial/
├── CMakeLists.txt
├── SDD/
│   ├── SPECS/
│   │   ├── spec-001.md              # Implementación C++
|   |   ├── spec-002.md
|   |   ├── spec-003.md
|   |   ├── spec-004.md
|   |   ├── spec-005.md
│   │   └── spec-py.md              # Visualización Python
│   └── ARCH_DESIGN.md               # Arquitectura y contratos del proyecto
├── src/
│   ├── main.cpp                     # Orquestador
│   ├── generador_datos.hpp/cpp      # Generación de puntos + ruido
│   ├── rbf_core.hpp/cpp             # Construcción de Φ, regularización, solver
│   ├── evaluador.hpp/cpp            # Evaluación en malla 50×50
│   └── exportador.hpp/cpp           # Escritura de CSV
├── resultados/
│   ├── datos_entrenamiento.csv      # 100 puntos (x, y, z)
│   ├── pesos_eps_0.50.csv           # Pesos w para ε=0.5
│   ├── pesos_eps_1.50.csv           # Pesos w para ε=1.5
│   ├── pesos_eps_5.00.csv           # Pesos w para ε=5.0
│   ├── superficie_eps_0.50.csv      # 2500 puntos evaluados
│   ├── superficie_eps_1.50.csv
│   └── superficie_eps_5.00.csvs
├── python/
│   ├── visualizar_rbf.py
│   └── requirements.txt
└── Docs/
    └── informe_taller6.tex
```

---

## 6. Implementación de Módulos (C++17)

Las especificaciones técnicas de desarrollo guiado se localizan en la ruta `SDD/SPECS/`:

*   **[spec-001.md](SDD/SPECS/spec-001.md)** — Módulo 1: `generador_datos.hpp/cpp` (Generación de puntos + ruido gaussiano).
*   **[spec-002.md](SDD/SPECS/spec-002.md)** — Módulo 2: `rbf_core.hpp/cpp` (Construcción de matriz Φ, Tikhonov y solver lineal Eigen).
*   **[spec-003.md](SDD/SPECS/spec-003.md)** — Módulo 3: `evaluador.hpp/cpp` (Evaluación de inferencias en malla de 50x50).
*   **[spec-004.md](SDD/SPECS/spec-004.md)** — Módulo 4: `exportador.hpp/cpp` (Persistencia y escritura de matrices y vectores a CSV).
*   **[spec-005.md](SDD/SPECS/spec-005.md)** — Módulo 5: `main.cpp` (Orquestador principal y flujo de ejecución de los 3 experimentos de \(\epsilon\)).

---

## 7. Visualización y Gráficas (Python)

*   **[spec-py.md](SDD/SPECS/spec-py.md)** — Módulo 6: `visualizar_rbf.py` (Script de renderizado para gráficos de contorno y superficies 3D).

---

## 8. Referencias

1. **Buhmann, M. D.** (2005). *Radial Basis Functions: Theory and Implementations*. Cambridge University Press.

2. **Fasshauer, G. E.** (2007). *Meshfree Approximation Methods with MATLAB*. World Scientific.

3. **Rippa, S.** (1999). *An algorithm for selecting a good value for the parameter c in radial basis function interpolation*. Advances in Computational Mathematics, 11(2-3), 193-210.

4. **Eigen Documentation.** *Linear Algebra and Decompositions.* https://eigen.tuxfamily.org/dox/

5. **Wright, G. B., & Fornberg, B.** (2006). *Scattered data approximation with the "stable" RBF method*. Proceedings of the 2006 ACM Symposium.

---

**Fin del documento ARCH_DESIGN.md**
s
---