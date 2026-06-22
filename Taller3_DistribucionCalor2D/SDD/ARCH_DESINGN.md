# ARQUITECTURA Y DISEÑO: Proyecto — Simulación de Distribución de Calor 2D

> Documento de contexto para el Taller 3 de Cálculo Científico.  
> **Metodología:** SDD (Specification-Driven Development)  
> **Problema:** Ecuación de Poisson mediante Diferencias Finitas

---

## 1. Stack

| Capa | Tecnología | Versión | Propósito |
|------|-----------|---------|-----------|
| **Cálculo numérico** | C++17 | ISO/IEC 14882:2017 | Núcleo: construcción de matriz A, vector b, solver |
| **Álgebra lineal** | Eigen 3.4 | 3.4.0+ | Matrices dispersas (SparseMatrix), solver iterativo |
| **Build** | CMake | 3.16+ | Generación de targets |
| **Exportación** | `std::ofstream` (STL) | C++11+ | Escritura de resultados CSV |
| **Visualización** | Python 3.10+ | 3.10+ | Heatmaps, isotermas, perfiles |
| **Gráficos** | matplotlib | 3.7+ | Renderizado de figuras |
| **Análisis datos** | numpy, pandas | 2.0+ | Carga y procesamiento de CSV |

**Compilador:** Visual Studio 2022 (MSVC) / GCC 11+ / Clang 14+

---

## 2. Objetivo

Resolver la **Ecuación de Poisson** en 2D para modelar la distribución de temperatura en estado estacionario en una placa cuadrada, utilizando el **Método de Diferencias Finitas** y condiciones de frontera de Dirichlet.

**Ecuación gobernante:**

$$\nabla^2 u(x,y) = -f(x,y)$$

**Discretización:**

$$4u_{i,j} - (u_{i+1,j} + u_{i-1,j} + u_{i,j+1} + u_{i,j-1}) = h^2 f_{i,j}$$

**Flujo:**

```
Configuración (n, bordes, fuente)  →  Construcción Matriz A (Sparse)  →  Construcción Vector b  →  Solver (BiCGSTAB)  →  Solución u  →  CSV  →  Python  →  Visualizaciones
```

---

## 3. Conceptos Fundamentales

### 3.1 Dominio y Discretización

| Propiedad | Valor |
|-----------|-------|
| **Dominio físico** | $[0, 1] \times [0, 1]$ |
| **Nodos internos por lado** | $n = 20$ |
| **Total de incógnitas** | $N = n^2 = 400$ |
| **Espaciado de malla** | $h = \frac{1}{n+1} = \frac{1}{21} \approx 0.0476$ |
| **Nodos totales (incluyendo bordes)** | $(n+2) \times (n+2) = 22 \times 22 = 484$ |

**Mapeo de índices:**

- Nodo interno $(i, j)$ con $i, j \in [1, n]$
- Índice lineal: $k = (i-1) \cdot n + (j-1)$ donde $k \in [0, N-1]$

### 3.2 Condiciones de Frontera (Dirichlet)

| Borde | Condición | Valor |
|-------|-----------|-------|
| **Superior** ($y = 1$) | $u(x, 1) = T_{sup}$ | $100\text{°C}$ |
| **Inferior** ($y = 0$) | $u(x, 0) = T_{inf}$ | $0\text{°C}$ |
| **Izquierdo** ($x = 0$) | $u(0, y) = T_{izq}$ | $50\text{°C}$ |
| **Derecho** ($x = 1$) | $u(1, y) = T_{der}$ | $50\text{°C}$ |

### 3.3 Fuente de Calor Puntual

```
f(x, y) = Q * δ(x - 0.5) * δ(y - 0.5)
```

**Implementación discreta:**

- Nodo central: $(i_c, j_c) = (\frac{n+1}{2}, \frac{n+1}{2}) = (10, 10)$ para $n=20$
- Intensidad: $Q = 1000$ (ajustable para visualizar perturbación)
- Solo el nodo central tiene $f_{i_c, j_c} = Q$, el resto $f_{i,j} = 0$

### 3.4 Construcción del Sistema $Au = b$

**Matriz A (Sparse, $N \times N$):**

- Diagonal principal: $A_{k,k} = 4$ para todo $k$
- Vecinos horizontales: $A_{k, k-1} = -1$ y $A_{k, k+1} = -1$ (si existen)
- Vecinos verticales: $A_{k, k-n} = -1$ y $A_{k, k+n} = -1$ (si existen)
- **Patrón:** Matriz de 5 diagonales (pentadiagonal)

**Vector b ($N \times 1$):**

- Contribución de fuente: $b_k = h^2 \cdot f_{i,j}$
- Contribución de bordes: Si un vecino es frontera, sumar su valor a $b_k$

**Ejemplo para nodo $(i,j)$ con vecinos en bordes:**

```
b_k = h² * f_{i,j} + T_superior + T_inferior + T_izquierda + T_derecha
      (solo sumar los que apliquen según posición del nodo)
```

---

## 4. Algoritmo General

```
ENTRADA:  n (nodos por lado), T_bordes, Q (intensidad fuente)
SALIDA:   resultados/temperatura.csv, resultados/matriz_A.csv
          graficos/heatmap.png, graficos/patron_A.png, graficos/perfil.png

1. INICIALIZACIÓN
   a. Calcular h = 1 / (n + 1)
   b. Calcular N = n * n
   c. Inicializar matriz dispersa A (N × N)
   d. Inicializar vector b (N × 1)

2. CONSTRUCCIÓN DEL SISTEMA
   Para cada nodo interno (i, j) con i,j ∈ [1, n]:
      a. Calcular índice lineal k = (i-1)*n + (j-1)
      b. A[k, k] = 4  (diagonal)

      c. Vecino izquierdo (i-1, j):
         Si i > 1:  A[k, k-1] = -1
         Si i == 1: b[k] += T_izquierda

      d. Vecino derecho (i+1, j):
         Si i < n:  A[k, k+1] = -1
         Si i == n: b[k] += T_derecha

      e. Vecino inferior (i, j-1):
         Si j > 1:  A[k, k-n] = -1
         Si j == 1: b[k] += T_inferior

      f. Vecino superior (i, j+1):
         Si j < n:  A[k, k+n] = -1
         Si j == n: b[k] += T_superior

      g. Fuente de calor:
         Si (i, j) == (centro_x, centro_y):
            b[k] += h² * Q

3. RESOLUCIÓN DEL SISTEMA
   a. Usar solver iterativo: Eigen::BiCGSTAB
   b. Resolver: u = A⁻¹ * b
   c. Verificar convergencia (error < tolerancia)

4. EXPORTAR RESULTADOS
   a. Guardar temperatura.csv con columnas: x, y, temperatura
   b. Guardar matriz_A.csv con elementos no nulos: fila, columna, valor

5. VISUALIZAR (Python)
   a. Heatmap con isotermas
   b. Patrón de dispersión de matriz A
   c. Perfil transversal u(x, 0.5)
   d. Comparación con/sin fuente
```

---

## 5. Estructura de Carpetas

```
Taller3_Calor2D_DiferenciasFinitas/
├── CMakeLists.txt                       # Build system (CMake 3.16+)
├── README.md                            # Instrucciones de uso y compilación
├── Docs/                                # Informe de resultados
├── SDD/                                 # Specification-Driven Development
│   ├── SPECS/
│   │   ├── spec-001.md              # Implementación C++: módulos, API
│   │   └── spec-002.md              # Script Python de visualización
│   ├── PLANS/
│   │   └── plan_poisson.md          # Plan de desarrollo paso a paso
│   └── ARCH_DESIGN.md               # Arquitectura, diseño y teoría (este archivo)
├── src/
│   ├── main.cpp                     # Orquestador del flujo
│   ├── malla.hpp/cpp                # Gestión de nodos y mapeo de índices
│   ├── ensamblador.hpp/cpp          # Construcción de A y b
│   ├── solver.hpp/cpp               # Wrapper de Eigen BiCGSTAB
│   ├── exportador.hpp/cpp           # Escritura de CSV
│   └── utils.hpp                    # Utilidades (constantes, tipos)
├── resultados/
│   ├── temperatura.csv              # Solución: x, y, T
│   ├── temperatura_sin_fuente.csv   # Comparación
│   └── matriz_A.csv                 # Elementos no nulos de A
├── python/
│   ├── visualizar_calor.py          # Script principal de visualización
│   └── requirements.txt             # Dependencias: numpy, matplotlib, pandas
└── build/                           # Directorio de compilación (generado)
```

---

## 6. Métricas y Criterios de Aceptación

| Criterio | Cómo se verifica | Estado |
|----------|-----------------|--------|
| **Dimensiones correctas** | Matriz A es $400 \times 400$, vector b es $400 \times 1$ | ⬜ Pendiente |
| **Simetría de A** | $A = A^T$ (matriz simétrica definida positiva) | ⬜ Pendiente |
| **Condiciones de frontera** | Bordes mantienen temperatura constante | ⬜ Pendiente |
| **Convergencia del solver** | Residuo $\|Au - b\| < 10^{-10}$ | ⬜ Pendiente |
| **Conservación de energía** | Balance global de calor (entrada = salida) | ⬜ Pendiente |
| **Efecto de fuente** | Pico de temperatura visible en $(0.5, 0.5)$ | ⬜ Pendiente |
| **Visualizaciones generadas** | 4 gráficos PNG (heatmap, isotermas, patrón A, perfil) | ⬜ Pendiente |
| **Comparación con/sin fuente** | Diferencia cuantificable en el centro | ⬜ Pendiente |

---

## 7. Procedimiento de Compilación y Ejecución

### 7.1 Prerrequisitos

| Herramienta | Versión mínima | Comentario |
|-------------|----------------|------------|
| **CMake** | 3.16+ | Build system |
| **Compilador C++** | GCC 11+, Clang 14+, o MSVC 2022 | C++17 requerido |
| **Eigen3** | 3.4+ | Álgebra lineal dispersa |
| **Python** | 3.10+ | Visualización |

### 7.2 Compilación (Windows - MSVC)

```powershell
# 1. Crear directorio de compilación
mkdir build
cd build

# 2. Configurar con CMake
cmake .. -G "Visual Studio 17 2022"

# 3. Compilar en Release
cmake --build . --config Release

# 4. Ejecutar
.\Release\calor_2d.exe
```

### 7.3 Compilación (Linux/macOS - GCC/Clang)

```bash
# 1. Crear directorio de compilación
mkdir build && cd build

# 2. Configurar con CMake
cmake ..

# 3. Compilar
cmake --build . -j$(nproc)

# 4. Ejecutar
./calor_2d
```

### 7.4 Ejecución del script Python

```bash
# Instalar dependencias
pip install -r python/requirements.txt

# Ejecutar visualización
cd python
python visualizar_calor.py
```

### 7.5 Salida esperada

```
=== SIMULACIÓN DE DISTRIBUCIÓN DE CALOR 2D ===
Dominio: [0, 1] × [0, 1]
Nodos internos: 20 × 20 = 400
Espaciado h: 0.047619

Construyendo sistema...
  Matriz A: 400 × 400 (elementos no nulos: ~1960)
  Vector b: 400 × 1

Resolviendo con BiCGSTAB...
  Iteraciones: 45
  Error final: 2.3e-12

Exportando resultados...
  temperatura.csv: 400 filas
  matriz_A.csv: 1960 elementos

Simulación completada con éxito.
```

---

## 8. Entregables del Taller

### 8.1 Visualización del Patrón de Matriz A

**Tipo:** Gráfico de dispersión (spy plot)  
**Propósito:** Mostrar la estructura de bandas de la matriz dispersa  
**Herramienta Python:** `matplotlib.pyplot.spy()`

### 8.2 Mapa de Calor con Isotermas

**Tipo:** Heatmap + contour plot  
**Propósito:** Visualizar la distribución de temperatura y gradientes  
**Herramienta Python:** `plt.imshow()` + `plt.contour()`

### 8.3 Análisis Físico: Con y Sin Fuente

**Tipo:** Comparación lado a lado  
**Propósito:** Cuantificar el efecto de la fuente puntual  
**Métrica:** Diferencia de temperatura $\Delta T = T_{con} - T_{sin}$ en el centro

### 8.4 Perfil Transversal

**Tipo:** Gráfico de línea  
**Propósito:** Mostrar el pico de temperatura en $y = 0.5$  
**Ecuación:** $u(x, 0.5)$ para $x \in [0, 1]$

---

## 9. Consideraciones Numéricas

### 9.1 Estabilidad del Método

- El MDF para la ecuación de Poisson es **incondicionalmente estable**
- La matriz A es **simétrica definida positiva** (SPD)
- Garantiza existencia y unicidad de la solución

### 9.2 Selección del Solver

| Solver | Ventajas | Desventajas | Recomendación |
|--------|----------|-------------|---------------|
| **BiCGSTAB** | Rápido para matrices dispersas | Puede no converger si A es mal condicionada | ✅ **Recomendado** |
| **Conjugate Gradient** | Óptimo para SPD | Solo funciona si A es SPD | ✅ Alternativa |
| **LU Directo** | Exacto | Memoria $O(N^2)$, lento para N grande | ❌ No usar |

### 9.3 Precondicionamiento

Para acelerar la convergencia:

```cpp
// Precondicionador incompleto LU
Eigen::IncompleteLUT<double> precondicionador;
solver.preconditioner(precondicionador);
```

---

## 10. Referencias

1. **Strikwerda, J. C.** (2004). *Finite Difference Schemes and Partial Differential Equations.* SIAM. → Fundamentos teóricos del MDF.

2. **LeVeque, R. J.** (2007). *Finite Difference Methods for Ordinary and Partial Differential Equations.* SIAM. → Estabilidad y convergencia.

3. **Saad, Y.** (2003). *Iterative Methods for Sparse Linear Systems.* SIAM. → Solvers iterativos (BiCGSTAB, CG).

4. **Eigen Documentation.** *Sparse Linear Algebra.* https://eigen.tuxfamily.org/dox/group__Sparse__chapter.html → Manipulación de matrices dispersas.

---

**Fin del documento ARCH_DESIGN.md**
