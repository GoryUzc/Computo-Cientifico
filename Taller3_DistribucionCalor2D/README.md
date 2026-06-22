# Taller 3: Distribución de Calor 2D - Ecuación de Poisson

## Descripción

Simulación numérica de la distribución de temperatura en estado estacionario en una placa cuadrada usando el método de diferencias finitas para resolver la ecuación de Poisson 2D.

## Ecuación Gobernante

$$\nabla^2 u(x,y) = -f(x,y)$$

donde $f(x,y) = Q \cdot \delta(x-0.5) \cdot \delta(y-0.5)$ representa una fuente de calor puntual en el centro.

## Requisitos

- **CMake** 3.16+
- **Compilador C++17** (MSVC 2022, GCC 11+, o Clang 14+)
- **Eigen3** 3.4+
- **Python** 3.10+ con numpy, pandas, matplotlib

## Compilación y Ejecución

### Windows (MSVC)

```powershell
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
.\Release\calor_2d.exe
```

### Linux/macOS

```bash
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)
./calor_2d
```

## Uso

```bash
# 1. Ejecutar el solver C++ (genera resultados)
./calor_2d.exe

# 2. Ejecutar visualizaciones (genera gráficos)
cd python
pip install -r requirements.txt
python visualizar_calor.py
```

## Salida Esperada

El programa genera:
- `resultados/temperatura.csv` - Solución con fuente (nodos internos)
- `resultados/temperatura_completa.csv` - Solución con fuente (incluye fronteras)
- `resultados/temperatura_sin_fuente.csv` - Solución sin fuente
- `resultados/matriz_A.csv` - Patrón de elementos no nulos
- `resultados/graficos/*.png` - 4 visualizaciones

## Parámetros de Configuración

| Parámetro | Valor | Descripción |
|-----------|-------|-------------|
| n | 20 | Nodos internos por lado |
| L | 1.0 | Longitud del dominio [0,1]×[0,1] |
| h | 1/21 ≈ 0.0476 | Espaciado de malla |
| T_superior | 100°C | Condición de frontera arriba |
| T_inferior | 0°C | Condición de frontera abajo |
| T_izquierda | 50°C | Condición de frontera izquierda |
| T_derecha | 50°C | Condición de frontera derecha |
| Q | 1000 | Intensidad de fuente de calor |

## Arquitectura

```
src/
├── main.cpp         # Orquestador del flujo
├── malla.hpp/cpp    # Gestión de nodos y coordenadas
├── ensamblador.hpp/cpp # Construcción de sistema Au=b
├── solver.hpp/cpp   # Wrapper de Eigen BiCGSTAB
└── exportador.hpp/cpp # Escritura de resultados CSV
```

## Documentación

- `SDD/ARCH_DESIGN.md` - Arquitectura y diseño detallado
- `SDD/SPECS/SPEC-001.md` - Especificación del solver C++
- `SDD/SPECS/SPEC-002.md` - Especificación de visualizaciones Python
