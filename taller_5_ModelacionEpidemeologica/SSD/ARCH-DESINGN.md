# ARCH-DESIGN.md: Taller 5 — Modelación Epidemiológica SEIR con RK4

> Documento de arquitectura y diseño para implementación del modelo SEIR   
> **Problema:** Sistema de EDOs no lineales resuelto numéricamente con Runge-Kutta 4to orden

---

## 1. Stack Tecnológico

| Capa | Tecnología | Versión | Propósito |
|------|-----------|---------|-----------|
| **Cálculo numérico** | C++17 | ISO/IEC 14882:2017 | Núcleo: implementación manual de RK4 |
| **Álgebra lineal** | Eigen 3.4 | 3.4.0+ | Operaciones vectoriales (Vector4d) |
| **Build** | CMake | 3.16+ | Generación de targets |
| **Exportación** | `std::ofstream` (STL) | C++11+ | Escritura de resultados CSV |
| **Visualización** | Python 3.10+ | 3.10+ | Gráficas de evolución temporal |
| **Gráficos** | matplotlib | 3.7+ | Renderizado de figuras |
| **Análisis datos** | numpy, pandas | 2.0+ | Carga y procesamiento de CSV |
| **Informe** | LaTeX | - | Documento técnico |

---

## 2. Objetivo

Implementar un solver numérico en **C++17 con Eigen** para resolver el sistema de ecuaciones diferenciales ordinarias (EDO) del **modelo compartimental SEIR** utilizando el **Método de Runge-Kutta de 4to orden (RK4)** implementado manualmente.

El proyecto debe:

1. Resolver el sistema SEIR para **dos escenarios epidemiológicos**
2. Calcular el **Número Básico de Reproducción** $R_0 = \beta/\gamma$
3. **Demostrar numéricamente** la conservación de la población total $S + E + I + R = N$
4. Exportar resultados a CSV para visualización en Python


---

## 3. Fundamento Matemático

### 3.1 Modelo Compartimental SEIR

El sistema de EDOs no lineales que rige la dinámica epidemiológica es:

$$
\begin{align*}
\frac{dS}{dt} &= -\frac{\beta S I}{N} \\
\frac{dE}{dt} &= \frac{\beta S I}{N} - \sigma E \\
\frac{dI}{dt} &= \sigma E - \gamma I \\
\frac{dR}{dt} &= \gamma I
\end{align*}
$$

**Parámetros biológicos:**

| Parámetro | Significado | Valor |
|-----------|-------------|-------|
| $\beta$ | Tasa de transmisión | Escenario A: 0.6, Escenario B: variable |
| $\sigma$ | Inverso del periodo de incubación | $0.2$ ($D_{incub} = 5$ días) |
| $\gamma$ | Inverso del periodo de infección | $0.1$ ($D_{infec} = 10$ días) |
| $N$ | Población total | $3 \times 10^7$ |

**Condiciones iniciales:**
$$S(0) = N - I_0, \quad E(0) = 0, \quad I(0) = 100, \quad R(0) = 0$$

### 3.2 Número Básico de Reproducción ($R_0$)

$$R_0 = \frac{\beta}{\gamma}$$

**Interpretación epidemiológica:**
- $R_0 > 1$: La enfermedad se propaga (brote epidémico)
- $R_0 < 1$: La enfermedad se extingue
- $R_0 = 1$: Estado crítico (endémico)

### 3.3 Método de Runge-Kutta 4to Orden (RK4)

Para un sistema $\frac{d\mathbf{y}}{dt} = \mathbf{f}(t, \mathbf{y})$, el paso de $t_n$ a $t_{n+1} = t_n + h$ es:

$$
\begin{align*}
\mathbf{k}_1 &= \mathbf{f}(t_n, \mathbf{y}_n) \\
\mathbf{k}_2 &= \mathbf{f}\left(t_n + \frac{h}{2}, \mathbf{y}_n + \frac{h}{2}\mathbf{k}_1\right) \\
\mathbf{k}_3 &= \mathbf{f}\left(t_n + \frac{h}{2}, \mathbf{y}_n + \frac{h}{2}\mathbf{k}_2\right) \\
\mathbf{k}_4 &= \mathbf{f}(t_n + h, \mathbf{y}_n + h\mathbf{k}_3) \\
\mathbf{y}_{n+1} &= \mathbf{y}_n + \frac{h}{6}(\mathbf{k}_1 + 2\mathbf{k}_2 + 2\mathbf{k}_3 + \mathbf{k}_4)
\end{align*}
$$

**Propiedades del RK4:**
- Error local de truncamiento: $\mathcal{O}(h^5)$
- Error global: $\mathcal{O}(h^4)$
- Requiere 4 evaluaciones de $\mathbf{f}$ por paso
- Estabilidad excelente para problemas no lineales

### 3.4 Conservación de la Población

Sumando las 4 ecuaciones del SEIR:

$$\frac{dS}{dt} + \frac{dE}{dt} + \frac{dI}{dt} + \frac{dR}{dt} = 0$$

$$\implies \frac{dN}{dt} = 0 \implies N(t) = S(t) + E(t) + I(t) + R(t) = \text{constante}$$

**Verificación numérica:** En cada paso $t$, debe cumplirse:
$$|S(t) + E(t) + I(t) + R(t) - N_0| < 10^{-6}$$

---

## 4. Especificaciones de los Escenarios

### 4.1 Escenario A: Epidemia Libre (Sin Intervención)

| Parámetro | Valor |
|-----------|-------|
| $\beta$ | $0.6$ (constante) |
| $R_0$ | $0.6 / 0.1 = 6.0$ |
| Interpretación | Epidemia severa sin medidas de control |

### 4.2 Escenario B: Con Intervención (Confinamiento)

| Parámetro | Valor |
|-----------|-------|
| $\beta(t)$ | $\begin{cases} 0.6 & \text{si } t < 30 \\ 0.25 & \text{si } t \geq 30 \end{cases}$ |
| $R_0$ inicial | $6.0$ |
| $R_0$ intervención | $0.25 / 0.1 = 2.5$ |
| Interpretación | Medidas de confinamiento reducen transmisión en 58% |

### 4.3 Parámetros de Simulación

| Parámetro | Valor | Justificación |
|-----------|-------|---------------|
| Paso temporal $h$ | $0.1$ días | 10 pasos por día, equilibrio precisión/costo |
| Tiempo final $T$ | $200$ días | Cubre ciclo epidémico completo |
| Total de pasos | $2000$ | $T/h = 200/0.1$ |
| Tolerancia conservación | $10^{-6}$ | Verificación de integridad numérica |

---

## 5. Estructura del Proyecto

```
Taller5_ModelacionEpidemologica/
├── CMakeLists.txt
├── SDD/
│   ├── SPECS/
│   │   └── spec-001.md
|   |   └── spec-002.md
│   └── ARCH_DESIGN.md
├── src/
│   ├── main.cpp
│   ├── modelo_seir.hpp/cpp
│   ├── solver_rk4.hpp/cpp
│   ├── escenarios.hpp/cpp
│   └── exportador.hpp/cpp
├── resultados/
│   ├── escenario_A.csv
│   ├── escenario_B.csv
│   ├── conservacion_A.csv
│   ├── conservacion_B.csv
│   └── resumen.csv
├── python/
│   ├── visualizar_seir.py
│   └── requirements.txt
└── Docs/
    └── informe_taller5.tex
```
## 6. Impementacion 

**spec001.m**  Desarrollo en C++
**spec002.m**  Desarrollo en python 



## 7. Referencias

1. **Kermack, W. O., & McKendrick, A. G.** (1927). *A contribution to the mathematical theory of epidemics.* Proceedings of the Royal Society of London, 115(772), 700-721.

2. **Brauer, F.** (2017). *Epidemic models.* In Handbook of Mathematical Methods in Infectious Disease Modeling (pp. 3-26). CRC Press.

3. **Butcher, J. C.** (2008). *Numerical Methods for Ordinary Differential Equations* (2nd ed.). Wiley.

4. **Harko, T., Lobo, F. S., & Mak, M. K.** (2014). *Approximate analytical solutions of the SEIR epidemic model.* Applied Mathematics and Computation, 245, 337-348.

5. **Eigen Documentation.** *Dense matrix and array manipulation.* https://eigen.tuxfamily.org/dox/

---

**Fin del documento ARCH_DESIGN.md**