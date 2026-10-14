# Taller 6 — Interpolación mediante Redes de Base Radial (RBF)

> Proyecto académico de interpolación numérica en C++17 con Eigen3, siguiendo metodología SDD (Specification-Driven Development).

---

## 1. Resumen del Proyecto

Implementación de un interpolador de **Redes de Base Radial (RBF)** para aproximar la función objetivo:

$$f(x, y) = \sin(x)\cos(y), \quad (x, y) \in [-2, 2]^2$$

El pipeline genera 100 puntos de entrenamiento con ruido gaussiano, construye la matriz de Gram $\Phi$ mediante un kernel Gaussiano, aplica regularización de Tikhonov ($\lambda = 10^{-6}$), resuelve el sistema lineal para obtener los pesos $w$, y evalúa la superficie en una malla regular de $50 \times 50 = 2500$ puntos. Se estudian 3 valores del parámetro de forma $\epsilon \in \{0.5, 1.5, 5.0\}$.

---

## 2. Stack Tecnológico

| Capa | Tecnología | Versión / Config | Propósito |
|------|-----------|------------------|-----------|
| **Lenguaje** | C++17 | ISO/IEC 14882:2017 | Núcleo numérico |
| **Álgebra lineal** | Eigen3 | 3.4.0+ | Matrices densas, LDLT, valores propios |
| **Generación aleatoria** | `<random>` (STL) | C++11+ | `mt19937`, uniform/normal |
| **Build** | CMake | 3.16+ | Generación de targets |
| **Optimización** | GCC/Clang | `-O3 -ffast-math -march=native` | Cálculo numérico agresivo |
| **Visualización** | Python 3.10+ | 3.10+ | `matplotlib`, `mpl_toolkits.mplot3d` |
| **Datos / Gráficos** | numpy, pandas | 2.0+, 2.0+ | Carga CSV, reshape, contourf |
| **Informe** | LaTeX | — | Documento técnico profesional |

---

## 3. Estructura del Proyecto

```
.
├── CMakeLists.txt              # Build: 5 módulos C++, Eigen3, C++17
├── README.md                  
├── SSD/
│   ├── ARCH_DESING.md          # Arquitectura y contratos
│   └── SPECS/
│       ├── spec-001.md         # Generador de datos (generador_datos)
│       ├── spec-002.md         # Núcleo RBF (rbf_core)
│       ├── spec-003.md         # Evaluador de superficie (evaluador)
│       ├── spec-004.md         # Exportador CSV (exportador)
│       ├── spec-005.md         # Orquestador principal (main)
│       └── spec-py.md          # Visualización Python (visualizar_rbf)
├── src/
│   ├── main.cpp                # Orquestador: 4 fases + reporte final
│   ├── generador_datos.hpp/cpp # N=100 puntos, ruido σ=0.1, semilla 42
│   ├── rbf_core.hpp/cpp        # Φ, Tikhonov, LDLT/QR fallback, condición
│   ├── evaluador.hpp/cpp       # Malla 50×50, evaluación O(M·N), RMSE
│   └── exportador.hpp/cpp      # CSV: datos, pesos, superficies
├── python/
│   ├── visualizar_rbf.py       # 6 gráficos profesionales + reporte
│   └── requirements.txt        # numpy, pandas, matplotlib
└── resultados/                 # Salida (generado por el ejecutable)
    ├── datos_entrenamiento.csv
    ├── funcion_real.csv
    ├── pesos_eps_0.50.csv
    ├── pesos_eps_1.50.csv
    ├── pesos_eps_5.00.csv
    ├── superficie_eps_0.50.csv
    ├── superficie_eps_1.50.csv
    ├── superficie_eps_5.00.csv
    └── graficos/
        ├── 01_puntos_entrenamiento.png
        ├── 02_comparacion_contornos.png
        ├── 03_corte_transversal.png
        ├── 04_error_relativo.png
        ├── 05_vista_3d.png
        └── 06_analisis_epsilon.png
```

---

## 4. Especificaciones Técnicas por Módulo

### 4.1 Módulo 1 — Generador de Datos (`generador_datos`)
- **Entrada:** `N=100`, dominio $[-2,2]^2$, `σ=0.1`, semilla `42`
- **Salida:** `Eigen::MatrixXd` (N×2) puntos; `Eigen::VectorXd` (N) valores con/sin ruido
- **Exportación:** `datos_entrenamiento.csv` (header `x,y,z`; 6 decimales)
- **Características:** `std::uniform_real_distribution`, `std::normal_distribution`, `std::filesystem::create_directories`

### 4.2 Módulo 2 — Núcleo RBF (`rbf_core`)
- **Kernel Gaussiano:** $\phi(r) = \exp(-(\epsilon r)^2)$, implementado con `arg = epsilon_*r; exp(-(arg*arg))`
- **Matriz de Gram $\Phi$:** Simétrica, diagonal unitaria `1.0`, calculada solo en triángulo superior (`O(N^2/2)`)
- **Regularización:** `Phi_reg = Phi + λI` (`λ = 1e-6`)
- **Solver:** `Eigen::LDLT<Eigen::MatrixXd>` con fallback a `ColPivHouseholderQR`
- **Condición:** `SelfAdjointEigenSolver` para valores propios; alerta si $\lambda_{\min} \approx 0$
- **Exportación:** `pesos_eps_X.csv` (header `indice,peso`)

### 4.3 Módulo 3 — Evaluador (`evaluador`)
- **Malla:** `crear_malla()` generada por barrido por filas (`i`=y, `j`=x); paso `4/49 ≈ 0.0816`
- **Evaluación:** Fórmula $s(\mathbf{x}_k) = \sum_j w_j \exp(-\epsilon^2 \|\mathbf{x}_k - \mathbf{x}_j\|^2)$
- **Optimización de código:** Evita `sqrt()` y usa `r² = dx² + dy²`; cálculo directo `exp(-eps2 * r2)` (sin llamar `kernel()` en bucles internos para rendimiento)
- **Métricas:** `calcular_rmse()`; `funcion_real()` evaluada sobre malla
- **Exportación:** `superficie_eps_X.csv` (2500 filas), `funcion_real.csv`

### 4.4 Módulo 4 — Exportador (`exportador`)
- **Funcionalidad:** 3 métodos estáticos (`exportar_datos_entrenamiento`, `exportar_pesos`, `exportar_superficie`)
- **Verificación:** Comprobación de dimensiones (`puntos.rows() == valores.size()`, `malla.rows() == z_predicho.size()`)
- **Formato:** `std::fixed << std::setprecision(6)`, separador `,`, sin espacios
- **Nota:** El código actual usa mensajes de log concisos (no imprime `epsilon` ni rango de pesos en cada exportación como preveía el spec original); el archivo `resumen_comparativo.csv` es referenciado por `main.cpp` pero **no se genera actualmente**.

### 4.5 Módulo 5 — Orquestador (`main`)
- **Fases:**
  1. Generación de datos (cronometrado con `std::chrono::high_resolution_clock`)
  2. Creación de malla + exportación `funcion_real.csv`
  3. Bucle `ε ∈ {0.5, 1.5, 5.0}`: construir Φ → regularizar → resolver → calcular condición → exportar pesos → evaluar superficie → calcular RMSE → exportar superficie
  4. Reporte final en consola (tabla comparativa de errores, condición y tiempos)
- **Estructura:** `MetricasEscenario` almacena `epsilon`, `error_entrenamiento`, `error_malla`, `numero_condicion`, `tiempo_entrenamiento`, `tiempo_evaluacion`

### 4.6 Módulo 6 — Visualización Python (`visualizar_rbf`)
- **Clase:** `VisualizadorRBF` con 6 métodos de gráfico
- **Estrategias anti-superposición:** Leyendas fuera (`bbox_to_anchor=(0.5, -0.15)`), `loc='best'`, subplots separados, colores contrastantes por ε, cajas blancas (`bbox=dict(facecolor='white', alpha=0.9)`)
- **Carga:** `_cargar_csv()` con fallback para nombres con/without `000000`; `_preparar_mallas()` ordena por `['y','x']` y hace `reshape()`
- **Salida:** 6 PNGs en `resultados/graficos/` (DPI 300, fuente ≥ 10 pt)
- **Requisitos:** `numpy>=1.24.0`, `pandas>=2.0.0`, `matplotlib>=3.7.0`

---

## 5. Parámetros de Configuración

| Parámetro | Valor | Justificación |
|-----------|-------|---------------|
| N (entrenamiento) | 100 | Balance precisión / costo |
| Dominio | $[-2, 2]^2$ | Rango estándar para $\sin(x)\cos(y)$ |
| σ (ruido) | 0.1 | ~10% de amplitud de f |
| λ (Tikhonov) | $10^{-6}$ | Estabilidad sin sesgo excesivo |
| Resolución malla | 50 × 50 (= 2500) | Resolución visual adecuada |
| Semilla | 42 | Reproducibilidad |
| ε₁ (pequeño) | 0.5 | Estudio de oscilaciones (Runge) |
| ε₂ (óptimo) | 1.5 | Balance suavidad / precisión |
| ε₃ (grande) | 5.0 | Estudio de sobreajuste local |

---

## 6. Fundamento Matemático (Resumen)

- **Interpolación RBF:** $s(\mathbf{x}) = \sum_{j=1}^N w_j \phi(\|\mathbf{x} - \mathbf{x}_j\|)$
- **Kernel Gaussiano:** $\phi(r) = \exp(- (\epsilon r)^2)$; $\phi(0)=1$, decaimiento exponencial
- **Sistema lineal:** $\Phi \mathbf{w} = \mathbf{y}$; con regularización $ (\Phi + \lambda I) \mathbf{w} = \mathbf{y}$
- **Propiedades de $\Phi$:** Simétrica, definida positiva para $\epsilon > 0$ y centros distintos; diagonal unitaria
- **Costo evaluación:** $O(M \times N) = 2500 \times 100 = 250{,}000$ evaluaciones de kernel
- **Efecto de ε:** Pequeño → campana ancha → $\Phi$ mal condicionada → oscilaciones; Grande → campana estrecha → $\Phi \approx I$ → picos aislados

---

## 7. Cambios en el Código (Actualización respecto a Specs)

Los archivos `SSD/SPECS/` se han actualizado para reflejar las implementaciones reales:

- **spec-003.md:** `evaluar()` usa optimización sin `sqrt()` (`r²` directo) y `std::exp(-eps2 * r2)` en lugar de llamar `kernel()`; `crear_malla()` mantiene orden `['y','x']` consistente con Python.
- **spec-004.md:** `exportador.cpp` es más conciso (no imprime `epsilon` ni rango en cada llamada); se documenta que `resumen_comparativo.csv` no se genera desde C++ (solo se referencia en `main.cpp` para el listado de archivos).
- **spec-005.md:** Confirmado que `main.cpp` coincide con el flujo de 4 fases; se nota ausencia de generación real de `resumen_comparativo.csv`.
- **spec-002.md:** `rbf_core.cpp` usa `arg = epsilon_ * r; return std::exp(-(arg * arg))` (optimización algebraica equivalente).
- **spec-py.md:** `visualizar_rbf.py` incluye mecanismo de fallback (`000000`) para nombres de archivo generados por `std::to_string()`.

---

## 8. Cómo Compilar y Ejecutar

```bash
# Compilar (requiere Eigen3 instalado, vía vcpkg o sistema)
mkdir build && cd build
cmake ..
cmake --build . --config Release

# Ejecutar (genera resultados/)
./taller6_rbf

# Visualizar (requiere Python con dependencias)
cd ../python
python visualizar_rbf.py
```

---

## 9. Referencias

- Buhmann, M. D. (2005). *Radial Basis Functions: Theory and Implementations*. Cambridge University Press.
- Fasshauer, G. E. (2007). *Meshfree Approximation Methods with MATLAB*. World Scientific.
- Rippa, S. (1999). *An algorithm for selecting a good value for the parameter c in radial basis function interpolation*. Advances in Computational Mathematics, 11(2-3), 193-210.
- Eigen Documentation. *Linear Algebra and Decompositions.* https://eigen.tuxfamily.org/dox/

---

> **Estado:** Implementado y verificado. Todos los módulos compilan con CMake 3.16+, C++17, Eigen3. El pipeline genera los 9 archivos CSV previstos, calcula métricas y produce 6 visualizaciones profesionales.
