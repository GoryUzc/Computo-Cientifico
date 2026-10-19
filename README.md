# Cómputo Científico

Repositorio académico de proyectos, implementaciones y recursos para la materia Cómputo Científico. Cada carpeta contiene una herramienta o tema desarrollado durante el curso, con código fuente, documentación y especificaciones técnicas independientes.

## Proyectos

Cada proyecto es autónomo: posee su propio `CMakeLists.txt`, puede compilarse de forma independiente y no depende de los demás.

| # | Carpeta | Tema | Descripción | Estado |
|---|---------|------|-------------|--------|
| 1 | `TALLERI_COMPRENCIONIMAGENESSVD` | Compresión de Imágenes mediante SVD | Sistema completo de compresión basado en Descomposición en Valores Singulares (SVD). Implementación en C++17 con Eigen (BDCSVD), análisis cuantitativo con Python (matplotlib/numpy) e informe técnico en LaTeX. Incluye procesamiento de imágenes en escala de grises y color RGB. | ✅ Completado |
| 2 | `TALLER2_PCA_AncestreriaGenetica` | Análisis de Componentes Principales (PCA) e Identificación de Ancestría Genética | Implementación completa en C++17 (Eigen 3.4, fast-cpp-csv-parser) con visualización Python (matplotlib, pandas). Incluye normalización de Patterson, SVD y proyección PC1 vs PC2 para detectar estructura poblacional y admixture. | ✅ Completado |
| 3 | `Taller3_DistribucionCalor2D` | Distribución de Calor 2D - Ecuación de Poisson | Simulación numérica en placa cuadrada mediante diferencias finitas. Incluye solver C++17 (Eigen3, BiCGSTAB), exportación CSV, visualización Python (matplotlib, pandas) y documentación SDD con requisitos y arquitectura modular. | ✅ Completado |
| 4 | `Taller4_PageRank` | PageRank - Método de las Potencias | Implementación del algoritmo PageRank mediante el método de potencias en C++17 con Eigen. Incluye 5 casos de prueba con matrices de transición hardcodeadas, construcción de la matriz de Google G = d·M + (1-d)/n·J, y visualización en Python (pandas, matplotlib, tabulate) generando tablas CSV/LaTeX y gráficos PNG. | ✅ Completado |
| 5 | `taller_5_ModelacionEpidemeologica` | Modelación Epidemiológica - Simulación con EDOs | Implementación en **C++17** del modelo SIR+Epidemiológico con resolución numérica en tiempo real. Incluye visualización Python (matplotlib, pandas) con métricas clave de propagación de enfermedades. | ✅ Completado |
| 6 | `Taller6_RedesBaseRadial` | Interpolación mediante Redes de Base Radial (RBF) | Interpolador RBF Gaussiano en C++17 con Eigen3. Genera datos con ruido, aplica regularización de Tikhonov y compara la aproximación para distintos parámetros ε. Incluye exportación CSV y visualización en Python. | ✅ Completado |
| 7 | `Taller7_RedesNeuronales` | Red neuronal con Backpropagation | Implementación en C++17 de una red neuronal con una capa oculta para aproximar una función no lineal. Compara 15 experimentos con distintas tasas de aprendizaje e inicializaciones, y exporta historiales, predicciones y resultados para visualización en Python. | ✅ Completado |

## Cómo compilar cualquier proyecto

Cada proyecto se compila de forma independiente con CMake 3.16 o superior y requiere Eigen3. Instala Eigen3 mediante vcpkg o con el gestor de paquetes de tu sistema antes de configurar el proyecto.

En Bash, configura `VCPKG_ROOT` con la ruta a tu instalación de vcpkg y sustituye `Taller6_RedesBaseRadial` por la carpeta del proyecto que quieras compilar:

```bash
export VCPKG_ROOT="/c/vcpkg"
cmake -S Taller6_RedesBaseRadial -B Taller6_RedesBaseRadial/build \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build Taller6_RedesBaseRadial/build --config Release
```

Si Eigen3 ya está instalado y disponible para CMake, puedes omitir `-DCMAKE_TOOLCHAIN_FILE=...`.

## Informes Técnicos

- **Consulta** - https://goryuzc.github.io/Computo-Cientifico/

## Licencia

Uso académico. Libre para fines educativos.
