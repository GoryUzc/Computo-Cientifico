# Cómputo Científico

Repositorio académico de proyectos, implementaciones y recursos para la materia Cómputo Científico. Cada carpeta contiene una herramienta o tema desarrollado durante el curso, con código fuente, documentación y especificaciones técnicas independientes.

## Proyectos

Cada proyecto es autónomo: posee su propio `CMakeLists.txt`, puede compilarse de forma independiente y no depende de los demás.

| # | Carpeta | Tema | Descripción | Estado |
|---|---------|------|-------------|--------|
| 1.1 | `TALLERI_COMPRENCIONIMAGENESSVD` | Compresión de Imágenes mediante SVD | Sistema completo de compresión basado en Descomposición en Valores Singulares (SVD). Implementación en C++17 con Eigen (BDCSVD), análisis cuantitativo con Python (matplotlib/numpy) e informe técnico en LaTeX. Incluye procesamiento de imágenes en escala de grises y color RGB. | ✅ Completado |
| 1.2 | `TALLER2_PCA_AncestreriaGenetica` | Análisis de Componentes Principales (PCA) e Identificación de Ancestría Genética | Implementacion completo en C++17 (Eigen 3.4, fast-cpp-csv-parser) con visualización Python (matplotlib, pandas). Incluye normalización de Patterson, SVD y proyección PC1 vs PC2 para detectar estructura poblacional y admixture. | ✅ Completado |

## Cómo compilar cualquier proyecto

Todos los proyectos siguen la misma estructura de compilación con CMake y vcpkg.

### En Windows con Visual Studio 2022

Desde `x64 Native Tools Command Prompt for VS 2022`:

```bash
cd "TALLERI_COMPRENCIONIMAGENESSVD"
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE=C:/Users/User/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

### Con la extensión CMake Tools de VS Code

- Abre la carpeta del proyecto.
- Presiona `F7` para compilar.
- Presiona `Shift+F5` para ejecutar.

## Recursos

### Álgebra Lineal Computacional
- **Matrix Computations** — Golub, G. H. & Van Loan, C. F. (2013). 4th ed. Johns Hopkins. Cap. 2: SVD y Teorema de Eckart-Young-Mirsky.
- **Eigen Documentation** — *SVD Module.* https://eigen.tuxfamily.org/dox/group__SVD__Module.html

### Procesamiento de Imágenes
- **Digital Image Processing** — Gonzalez, R. C. & Woods, R. E.
- **Singular Value Decomposition: Applications to Image Processing** — Compton, A. (2020). Lagrange College Undergraduate Research.
- **SVD-Demo: Image Compression** — Baumann, T. https://timbaumann.info/svd-image-compression-demo/

### Implementación y Herramientas
- **Calculating SVD and PCA in C++** — DemoFox Blog (2022). https://blog.demofox.org/2022/07/12/calculating-svd-and-pca-in-c/
- **stb Libraries** — https://github.com/nothings/stb (stb_image, stb_image_write)

## Informes Tecnicos 

- **Consulta** - https://goryuzc.github.io/Computo-Cientifico/

## Licencia

Uso académico. Libre para fines educativos.
