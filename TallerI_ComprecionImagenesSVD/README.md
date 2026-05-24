# Compresión de Imágenes mediante SVD

## Especificaciones

| Documento | Contenido |
|-----------|-----------|
| [`SPECS/spec-000.md`](SPECS/spec-000.md) | Contexto del proyecto, marco teórico, algoritmo general y criterios de aceptación |
| [`SPECS/spec-001.md`](SPECS/spec-001.md) | Especificación formal de la implementación C++ (módulos, API, estructuras, build system) |
| [`SPECS/spec-002.md`](SPECS/spec-002.md) | Especificación del script Python para visualización y análisis cuantitativo |

## Requisitos
- CMake >= 3.16
- Visual Studio 2022 o GCC/Clang
- vcpkg con Eigen 3.4 instalado
- VCPKG_ROOT variable de entorno apuntando a `C:\Users\User\vcpkg`
- Python 3.x con matplotlib y numpy (opcional, para gráficos)

## Compilación
```cmd
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

## Ejecución

### Procesar una sola imagen
```cmd
# Escala de grises (por defecto)
.\Release\svd_compresion.exe --imagen assets/grises/edificio_gris_1.png --id 1

# Con valores k personalizados
.\Release\svd_compresion.exe --imagen assets/mi_foto.png --k 10,30,60 --modo gris --id 1

# Imagen a color
.\Release\svd_compresion.exe --imagen assets/rgb/lena_rgb_6.png --modo color --id 6
```

### Procesar todas las imágenes automáticamente
```cmd
.\procesar_todas.bat
```
Esto procesa las 8 imágenes del proyecto:
- 4 imágenes en escala de grises (ID: 1-4)
- 4 imágenes a color (ID: 5-8)

## Opciones de línea de comandos
| Opción | Descripción | Valor por defecto |
|--------|-------------|-------------------|
| `--imagen` | Ruta a la imagen | `assets/grises/edificio_gris_1.png` |
| `--modo` | `gris` o `color` | `gris` |
| `--k` | Lista de valores k separados por comas | `5,20,50,100,200` |
| `--id` | ID único para archivos de salida | `1` |

## Resultados

### Para cada imagen (ej. ID=1):
- `resultados/reconstrucciones/ak_1_*.png` — imágenes comprimidas para k=5,20,50,100,200
- `resultados/datos/metricas_1.csv` — métricas numéricas (error, energía, tasa)
- `resultados/datos/valores_singulares_1.csv` — valores singulares
- `resultados/graficos/error_vs_k_1.png` — gráfico de error vs k
- `resultados/graficos/energia_vs_k_1.png` — gráfico de energía vs k
- `resultados/graficos/tasa_compresion_vs_k_1.png` — gráfico de tasa vs k
- `resultados/graficos/panel_combinado_1.png` — panel de los 3 gráficos

## Análisis Python individual
```cmd
cd python
pip install -r requirements.txt
python analisis_cuantitativo.py 1  # ID de la imagen
```

## Tests unitarios
```cmd
cmake --build . --config Release --target test_reconstruccion_exacta
.\Release\test_reconstruccion_exacta.exe
```
Salida esperada:
```
[TEST] Reconstruccion exacta k=r: error = 4.xxxe-15
[TEST] Resultado: PASS
>>> El criterio de aceptacion SPEC-000 #3 se CUMPLE.
```

## Imágenes del dataset
| ID | Archivo | Dimensiones | Rango | Notas |
|----|---------|-------------|-------|-------|
| 1 | edificio_gris_1.png | 275x183 | 183 | Edificio en escala de grises |
| 2 | geometria_gris_2.png | 360x360 | 360 | Figuras geometricas en escala de grises |
| 3 | lena_gris_3.png | 512x512 | 512 | Clásica imagen Lena |
| 4 | mandril_gris_4.png | 209x242 | 209 | Mandril en escala de grises |
| 5 | edificio_rgb_5.png | 348x461 | 348 | Edificio a color |
| 6 | lena_rgb_6.png | 512x512 | 512 | Lena a color |
| 7 | mandril_rgb_7.png | 236x325 | 236 | Mandril a color |
| 8 | pimiento_rgb_8.png | 225x225 | 225 | Pimiento rojo |