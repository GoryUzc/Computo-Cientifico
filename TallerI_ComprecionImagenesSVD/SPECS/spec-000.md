## SPEC-000: Contexto del Proyecto — Compresión de Imágenes mediante SVD


## 1. Stack

Lenguaje: C++17  
Build: CMake 3.16+ con Visual Studio 2022 / GCC / Clang  
Dependencias: Eigen 3.4 (header-only, álgebra lineal), stb_image (header-only, carga/guardado de imágenes)  
Sistema operativo: Windows 10/11 (x64), Linux, macOS  
Gestor de paquetes: vcpkg (Eigen), headers stb_image incluidos manualmente  

Scripts de análisis: Python 3.10+ (matplotlib, numpy, pillow)  
Informe: LaTeX 

**Objetivo:** Implementar un sistema de compresión de imágenes usando la Descomposición en Valores Singulares (SVD). El proyecto debe cargar una imagen en escala de grises, calcular su SVD, generar aproximaciones de rango bajo para distintos valores de k, y producir métricas cuantitativas de calidad y compresión.

**Restricción:** El núcleo computacional (SVD, reconstrucción, métricas) se implementa en C++ con Eigen. Las visualizaciones y gráficos de análisis se generan con Python.


## 2. Estructura de Carpetas a Crear

```
TALLERI_COMPRENCIONIMAGENESSVD/
├── CMakeLists.txt                    # Build system con targets C++
├── README.md                         # Instrucciones de compilación y uso
├── SPECS/
│   ├── SPEC-000.md                   # Este documento: contexto del proyecto
│   ├── SPEC-001.md                   # Implementación C++ con Eigen
│   ├── SPEC-002.md                   # Scripts Python de visualización
└── informe/
│       └── main.tex                  # Informe LaTeX en Overleaf   
├── src/
│   ├── main.cpp                      # Punto de entrada: orquesta flujo completo
│   ├── cargador_imagen.hpp           # Declaración: carga imagen → matriz Eigen
│   ├── cargador_imagen.cpp           # Implementación con stb_image
│   ├── svd_procesador.hpp           # Declaración: cálculo SVD y aproximaciones
│   ├── svd_procesador.cpp           # Implementación con Eigen BDCSVD
│   ├── metricas.hpp                  # Declaración: error, energía, compresión
│   ├── metricas.cpp                  # Implementación de métricas cuantitativas
│   └── exportador.hpp / .cpp         # Guarda matrices resultado como PNG/CSV
├── include/
│   └── stb/                          # stb_image.h, stb_image_write.h
├── assets/
│   ├── test_gris.png                 # Imagen de prueba en escala de grises
│   └── test_color.png                # Imagen de prueba a color (bonus)
├── resultados/
│   ├── reconstrucciones/             # Imágenes comprimidas para cada k
│   ├── graficos/                     # Gráficos generados por Python
│   └── datos/                        # CSV con métricas numéricas
├── python/
│   ├── visualizar.py                 # Script: gráficos de reconstrucción
│   ├── analisis_cuantitativo.py      # Script: error, energía, compresión vs k
│   ├── comparar_color.py           # Script: bonus imagen a color
│   └── requirements.txt              # Dependencias Python
```


## 3. Conceptos Fundamentales


### a. Descomposición SVD

Sea $A \in \mathbb{R}^{m \times n}$ una matriz que representa una imagen en escala de grises (cada entrada = intensidad de un píxel, 0–255).

Su descomposición SVD viene dada por:

```
A = U Σ V^T
```

Donde:
- $U \in \mathbb{R}^{m \times m}$: matriz ortogonal (vectores singulares izquierdos).
- $\Sigma \in \mathbb{R}^{m \times n}$: matriz diagonal con valores singulares $\sigma_1 \geq \sigma_2 \geq \dots \geq \sigma_r > 0$.
- $V^T \in \mathbb{R}^{n \times n}$: matriz ortogonal transpuesta (vectores singulares derechos).
- $r = \text{rango}(A) \leq \min(m, n)$.

En forma expandida:

```
A = Σ_{i=1}^{r} σ_i · u_i · v_i^T
```

Cada término $\sigma_i \cdot u_i \cdot v_i^T$ es una "capa" o "modo" de la imagen. Los primeros modos capturan la estructura global; los últimos capturan detalles finos y ruido.


### b. Aproximación de Rango Bajo (Teorema de Eckart-Young-Mirsky)

La mejor aproximación de rango $k$ (con $k < r$) en norma de Frobenius se obtiene truncando la suma:

```
A_k = Σ_{i=1}^{k} σ_i · u_i · v_i^T
    = U_{:,1:k} · diag(σ_{1:k}) · V^T_{1:k,:}
```

**Propiedad clave:** $A_k$ es la matriz de rango $k$ más cercana a $A$ en norma de Frobenius. No existe otra matriz de rango $k$ que aproxime mejor a $A$.


### c. Compresión: ¿Qué se guarda?

Para reconstruir $A_k$ no necesitamos toda la matriz $A$ ($m \times n$ datos). Solo necesitamos:

```
Datos almacenados = k columnas de U  +  k valores de Σ  +  k filas de V^T
                  = k·m  +  k  +  k·n
                  = k·(m + n + 1)
```

**Tasa de compresión:**

```
Tasa = (m · n) / [k · (m + n + 1)]
```

Ejemplo: para una imagen 512×512 con $k=50$:
- Original: 262,144 datos
- Comprimido: $50 \times (512 + 512 + 1) = 51,250$ datos
- Tasa: **~5.1x**


### d. Métricas de Calidad

**Error relativo (norma de Frobenius):**

```
Error(k) = ||A - A_k||_F / ||A||_F
```

Mide qué proporción de la "magnitud" total de la imagen se pierde al comprimir. Valor 0 = perfecto; valor 1 = todo perdido.

**Energía capturada (varianza acumulada):**

```
E(k) = (Σ_{i=1}^{k} σ_i^2) / (Σ_{i=1}^{r} σ_i^2) × 100%
```

Mide qué porcentaje de la "información" (varianza) de la imagen se conserva. Valores singulares grandes → más información.


### e. Imagen a Color (Bonus)

Una imagen RGB tiene 3 canales independientes. Se aplica SVD a cada canal por separado:

```
R = U_R · Σ_R · V_R^T   →   R_k
G = U_G · Σ_G · V_G^T   →   G_k
B = U_B · Σ_B · V_B^T   →   B_k

Imagen comprimida = merge(R_k, G_k, B_k)
```

Datos totales: $3 \times k \times (m + n + 1)$.


## 4. Algoritmo General del Programa

```
ENTRADA:  Imagen PNG/JPG desde assets/
SALIDA:   Imágenes comprimidas, métricas CSV, gráficos PNG

1. Cargar imagen con stb_image y convertir a matriz Eigen::MatrixXd (0–255)
2. Calcular SVD completa usando Eigen::BDCSVD (divide-and-conquer, rápido)
3. Para cada k ∈ {5, 20, 50, 100, 200}:
   a. Construir A_k = U[:,:k] * diag(σ[:k]) * Vt[:k,:]
   b. Guardar A_k como imagen PNG en resultados/reconstrucciones/
   c. Calcular métricas: error relativo, energía capturada, tasa compresión
   d. Escribir métricas a resultados/datos/metricas.csv
4. Exportar valores singulares a CSV para análisis en Python
5. Ejecutar scripts Python para generar gráficos comparativos
6. (Bonus) Repetir pasos 1–5 para imagen a color, canal por canal
```


## 5. Librerías y Dependencias

| Librería | Versión | Propósito | Tipo |
|----------|---------|-----------|------|
| **Eigen** | 3.4+ | SVD (BDCSVD), álgebra matricial | Header-only |
| **stb_image** | Latest | Cargar PNG/JPG → array de píxeles | Header-only |
| **stb_image_write** | Latest | Guardar array de píxeles → PNG | Header-only |
| **matplotlib** (Py) | 3.7+ | Gráficos de análisis cuantitativo | pip |
| **numpy** (Py) | 1.24+ | Operaciones matriciales, CSV | pip |
| **Pillow** (Py) | 10.0+ | Manipulación de imágenes a color | pip |

**Eigen BDCSVD vs JacobiSVD:**
- `BDCSVD`: Divide-and-conquer + bidiagonalización. **Recomendado para matrices grandes** (imágenes > 100×100). Es el estándar para este proyecto.citeweb_search:21#0web_search:21#2
- `JacobiSVD`: Iteraciones Jacobi bilateral. Muy preciso pero lento para matrices grandes. Solo para matrices pequeñas (< 16×16).


## 6. Variantes de Configuración

| Parámetro | Valores soportados | Efecto |
|-----------|-------------------|--------|
| `valores_k` | `{5, 20, 50, 100, 200}` (configurable) | Niveles de compresión a evaluar |
| `algoritmo_svd` | `BDCSVD` (default), `JacobiSVD` | Algoritmo de descomposición SVD en Eigen |
| `modo` | `gris` (default), `color` (bonus) | Tipo de imagen a procesar |
| `formato_salida` | `png`, `csv` | Formatos de exportación de resultados |


## 7. Ejemplo de Ejecución

```bash
# Compilar (desde raíz del proyecto)
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release

# Ejecutar con imagen por defecto
./Release/svd_compresion.exe

# Ejecutar con imagen personalizada y valores k custom
./Release/svd_compresion.exe assets/mi_foto.png --k 10,30,60 --modo gris

# Ejecutar bonus color
./Release/svd_compresion.exe assets/mi_foto.png --k 30 --modo color
```

**Salida esperada en consola:**

```
[1/6] Cargando imagen: assets/test_gris.png (512x512)
[2/6] Calculando SVD con BDCSVD...
[3/6] Rango completo: r = 512
[4/6] Generando aproximaciones de rango bajo...
        k=5:   Error=0.1581 | Energía=97.5% | Compresión=51.2x
        k=20:  Error=0.1014 | Energía=99.0% | Compresión=12.8x
        k=50:  Error=0.0023 | Energía=100.0% | Compresión=5.1x
        k=100: Error=0.0019 | Energía=100.0% | Compresión=2.6x
        k=200: Error=0.0012 | Energía=100.0% | Compresión=1.3x
[5/6] Exportando métricas a resultados/datos/metricas.csv
[6/6] Ejecutando scripts Python para gráficos...
✅ Completado. Revisa resultados/graficos/
```


## 8. Aspectos a Evaluar

| Aspecto | Cómo se demuestra |
|---------|-------------------|
| **SVD correcta** | Reconstrucción exacta A = U·Σ·V^T con error numérico ~1e-15 |
| **Compresión real** | Archivos PNG de salida más pequeños que el original |
| **Métricas consistentes** | Error relativo decrece monotónicamente con k; energía crece monotónicamente |
| **Teorema E-Y-M** | Para cada k, no existe otra matriz de rango k con menor error Frobenius |
| **Código documentado** | Comentarios Doxygen en headers `.hpp` |
| **Reproducibilidad** | `cmake --build .` + `./svd_compresion.exe` genera mismos resultados |


## 9. Evidencias Solicitadas

| Evidencia | Archivo/Formato |
|-----------|-----------------|
| Código fuente C++ | `src/*.cpp`, `src/*.hpp` |
| CMakeLists.txt | `CMakeLists.txt` con targets Eigen |
| Imagen original | `assets/test_gris.png` |
| Imágenes comprimidas | `resultados/reconstrucciones/ak_*.png` |
| Métricas numéricas | `resultados/datos/metricas.csv` |
| Gráficos de análisis | `resultados/graficos/*.png` (generados por Python) |
| Informe LaTeX | `docs/informe/main.tex` (compilado en Overleaf) |


## 10. Referencias

1. **Eigen Documentation.** *SVD Module.* https://eigen.tuxfamily.org/dox/group__SVD__Module.html  
   → `BDCSVD`, `JacobiSVD`, `ComputeFullU`, `ComputeFullV`.citeweb_search:21#2

2. **DemoFox Blog.** (2022). *Calculating SVD and PCA in C++.* https://blog.demofox.org/2022/07/12/calculating-svd-and-pca-in-c/  
   → Tutorial práctico de SVD con Eigen, truncamiento y compresión.citeweb_search:21#1

3. **Golub, G. H., & Van Loan, C. F.** (2013). *Matrix Computations* (4th ed.). Johns Hopkins.  
   → Cap. 2: Descomposición SVD, Teorema de Eckart-Young-Mirsky (p. 79).

4. **Compton, A.** (2020). *Singular Value Decomposition: Applications to Image Processing.* Lagrange College Undergraduate Research.  
   → Aplicación directa de SVD a compresión de imágenes en escala de grises y color.citeweb_search:21#5

5. **Tim Baumann.** *SVD-Demo: Image Compression.* https://timbaumann.info/svd-image-compression-demo/  
   → Demo interactiva con explicación de fórmula de compresión.citeweb_search:21#7

6. **stb Libraries.** https://github.com/nothings/stb  
   → `stb_image.h`, `stb_image_write.h`: carga/guardado de imágenes sin dependencias.


## 11. Notas de Implementación

- **Eigen header-only:** No requiere compilación ni linking. Solo agregar `-I/path/to/eigen` al include path.
- **BDCSVD requiere `ComputeFullU | ComputeFullV`:** Por defecto, Eigen no calcula U ni V para ahorrar tiempo. Para reconstrucciones de rango k necesitamos las matrices completas.
- **stb_image:** Devuelve píxeles en formato `unsigned char*` (0–255). Convertir a `double` para Eigen, luego clamp de vuelta a 0–255 al guardar.
- **Valores singulares ordenados:** Eigen garantiza $\sigma_1 \geq \sigma_2 \geq \dots$ automáticamente. No requiere ordenación manual.
- **Rango de matrices de imagen:** Para imágenes reales, el rango numérico suele ser completo ($r = \min(m,n)$) por ruido. El rango "efectivo" se determina por el codo de la curva de valores singulares.
- **Python como post-procesador:** Los scripts Python leen los CSV generados por C++ y producen los gráficos. No dependen del código C++ en tiempo de ejecución.


## 12. Criterios de Aceptación Preliminares

- [ ] Carga una imagen PNG/JPG y la convierte correctamente a `Eigen::MatrixXd`.
- [ ] Calcula SVD completa con `Eigen::BDCSVD` sin errores numéricos.
- [ ] Reconstrucción exacta ($k = r$) coincide con original (error < $10^{-10}$).
- [ ] Genera aproximaciones $A_k$ para $k \in \{5, 20, 50, 100, 200\}$.
- [ ] Calcula métricas: error relativo, energía capturada, tasa de compresión.
- [ ] Exporta métricas a CSV legible por scripts Python.
- [ ] Scripts Python generan gráficos de error, energía y compresión vs k.
- [ ] Compilación con CMake + Eigen sin errores ni warnings con `-Wall -Wextra`.
- [ ] Documentación Doxygen mínima en headers `.hpp` para funciones públicas.
- [ ] (Bonus) Procesa imagen a color aplicando SVD por canal RGB.


---

> **Nota para desarrollo:** Este SPEC-000 establece el contexto. Los siguientes specs detallarán:
> - `SPEC-001.md` → Implementación C++ con Eigen (clases, SVD, métricas, exportación)
> - `SPEC-002.md` → Scripts Python de visualización y análisis cuantitativo
