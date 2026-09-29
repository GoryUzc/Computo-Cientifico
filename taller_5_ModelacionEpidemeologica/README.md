# Taller 5 — Modelación Epidemiológica SEIR con RK4

## Descripción
Implementación de un modelo epidemiológico SEIR (Susceptibles-Expuestos-Infectados-Recuperados) utilizando el método numérico de Runge-Kutta de 4to orden (RK4) para resolver el sistema de ecuaciones diferenciales ordinarias (EDO). El proyecto incluye implementación en C++ para el solver numérico y visualización en Python.

## Estructura del Proyecto
```
Taller5_ModelacionEpidemologica/
├── CMakeLists.txt
├── SSD/
│   ├── ARCH-DESINGN.md
│   ├── SPEC-001.md
│   └── SPEC-002.md
├── resultados/
│   ├── escenario_A.csv
│   ├── escenario_B.csv
│   ├── conservacion_A.csv
│   ├── conservacion_B.csv
│   └── resumen.csv
├── src/
│   ├── main.cpp
│   ├── modelo_seir.hpp
│   ├── modelo_seir.cpp
│   ├── solver_rk4.hpp
│   ├── solver_rk4.cpp
│   ├── exportador.hpp
│   └── exportador.cpp
├── python/
│   ├── visualizar_seir.py
│   └── requirements.txt
└── Docs/
    └── informe_taller5.tex
```

## Especificaciones Técnicas

### C++ Implementation (SPEC-001.md)
- **Lenguaje**: C++17
- **Dependencias**: Eigen 3.4+
- **Build System**: CMake 3.16+
- **Compilación**:
  ```bash
  mkdir build && cd build
  cmake ..
  make
  ./seir_rk4
  ```

### Parámetros del Modelo
- Población total (N): 3 × 10⁷
- Tasa de progresión E→I (σ): 0.2 (1/5 días)
- Tasa de recuperación I→R (γ): 0.1 (1/10 días)
- Paso temporal (h): 0.1 días
- Tiempo final (T): 200 días
- Condiciones iniciales: S₀ = N-100, E₀ = 0, I₀ = 100, R₀ = 0

### Escenarios Epidemiológicos
1. **Escenario A (Epidemia Libre)**:
   - β = 0.6 constante
   - R₀ = β/γ = 6.0

2. **Escenario B (Con Intervención)**:
   - β(t) = { 0.6 si t < 30, 0.25 si t ≥ 30 }
   - R₀ inicial = 6.0, R₀ intervención = 2.5

### Salida del Programa C++
- `resultados/escenario_A.csv`: Evolución temporal escenario A
- `resultados/escenario_B.csv`: Evolución temporal escenario B
- `resultados/conservacion_A.csv`: Error de conservación A
- `resultados/conservacion_B.csv`: Error de conservación B
- `resultados/resumen.csv`: Métricas comparativas

### Python Visualization (SPEC-002.md)
- **Lenguaje**: Python 3.10+
- **Dependencias**: numpy>=1.24.0, pandas>=2.0.0, matplotlib>=3.7.0
- **Ejecución**:
  ```bash
  cd python
  pip install -r requirements.txt
  python visualizar_seir.py
  ```

### Visualizaciones Generadas
1. `01_escenario_A.png`: Evolución S,E,I,R (Escenario A)
2. `02_escenario_B.png`: Evolución S,E,I,R (Escenario B)
3. `03_comparacion_infectados.png`: Comparación I(t) entre escenarios
4. `04_conservacion.png`: Verificación de conservación N(t)
5. `05_comparacion_completa.png`: Comparación 2x2 de todos los compartimentos

## Resultados Esperados
- Conservación de población: |S+E+I+R - N| < 1e-6
- Reducción del pico infectados con intervención
- Retraso del pico epidémico debido a confinamiento
- Gráficas de alta resolución (300 DPI) para informe LaTeX

## Referencias
1. Kermack & McKendrick (1927) - Teoría matemática de epidemias
2. Brauer (2017) - Modelos epidemiológicos
3. Butcher (2008) - Métodos Numéricos para EDO
4. Harko et al. (2014) - Soluciones aproximadas SEIR
5. Eigen Documentation - Manipulación de matrices densas