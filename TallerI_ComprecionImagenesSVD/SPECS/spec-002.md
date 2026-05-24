# SPEC-002 — Script Python: Visualización y Análisis Cuantitativo

## 1. Propósito
Este documento especifica el script Python `analisis_cuantitativo.py` encargado de generar visualizaciones y análisis cuantitativo de los resultados de la compresión SVD de imágenes. El script lee los archivos CSV generados por el programa C++ y crea gráficos PNG para facilitar la interpretación de los resultados.

## 2. Ubicación y estructura
- **Archivo:** `python/analisis_cuantitativo.py`
- **Tipo:** Script Python 3.x
- **Líneas:** ~105
- **Dependencias:** numpy, matplotlib, Pillow (ver requirements.txt)

## 3. Flujo de trabajo
El script sigue este flujo:
1. Lectura del ID de imagen desde argumentos de línea de comandos
2. Localización de los archivos de datos en `resultados/datos/`
3. Lectura de métricas y valores singulares desde CSV
4. Generación de 4 gráficos PNG en `resultados/graficos/`
5. Mensaje de confirmación de guardado

## 4. Argumentos de línea de comandos
| Argumento | Descripción | Valor por defecto |
|-----------|-------------|-------------------|
| `id_imagen` | ID único de la imagen procesada | `"1"` |

Uso: `python analisis_cuantitativo.py [id_imagen]`

## 5. Funciones principales

### 5.1 `leer_csv(ruta)`
**Contrato:**
- Entrada: ruta — ruta al archivo CSV a leer
- Proceso: Lee CSV usando csv.DictReader, extrae columnas numéricas
- Salida: Tupla (ks, vals, fieldnames) donde:
  - ks: lista de valores k (enteros)
  - vals: lista de listas con valores numéricos asociados a cada k
  - fieldnames: nombres de las columnas del CSV

### 5.2 Generación de gráficos
El script genera cuatro tipos de gráficos:

#### Gráfico 1: Error relativo vs k
- **Propiedad:** Muestra el error de reconstrucción (norma Frobenius relativa) en función del rango k
- **Escala:** Eje y en escala logarítmica para mejor visualización
- **Estilo:** Línea con marcadores circulares, color rojo (#e74c3c)

#### Gráfico 2: Energía capturada vs k
- **Propiedad:** Muestra el porcentaje de energía capturada en función del rango k
- **Formato:** Eje y formateado como porcentaje con un decimal
- **Estilo:** Línea con marcadores cuadrados, color verde (#27ae60)

#### Gráfico 3: Tasa de compresión vs k
- **Propiedad:** Muestra la tasa de compresión (en veces) en función del rango k
- **Característica especial:** Eje x invertido para que mayor k (menos compresión) esté a la derecha
- **Estilo:** Línea con marcadores triangulares, color azul (#2980b9)

#### Gráfico 4: Panel combinado
- **Propiedad:** Combina los tres gráficos anteriores en una sola figura horizontal
- **Título:** "Análisis cuantitativo SVD — Compresión de imagen"
- **Dimensiones:** 16x5 pulgadas (tres subgráficos de igual tamaño)

## 6. Formato de salida
Todos los gráficos se guardan en el directorio `resultados/graficos/` con los siguientes nombres:
- `error_vs_k_<id>.png`
- `energia_vs_k_<id>.png`
- `tasa_compresion_vs_k_<id>.png`
- `panel_combinado_<id>.png`

Donde `<id>` es el ID de la imagen procesada.

Configuración común:
- DPI: 150
- Tamaño base: 7x4.5 pulgadas (excepto panel combinado: 16x5)
- Rejilla: activada con estilo discontinuado y transparencia
- Títulos y etiquetas descriptivos

## 7. Integración con el sistema C++
El script es invocado automáticamente desde el programa C++ principal mediante:
```cpp
system("python analisis_cuantitativo.py <id> 2>NUL || "
       "python3 analisis_cuantitativo.py <id> 2>NUL || "
       "echo \"[WARN] Python no disponible; omite graficos.\"");
```
Esto asegura compatibilidad tanto con Windows (python) como sistemas Unix-like (python3).

## 8. Requisitos de entorno
Ver archivo `python/requirements.txt`:
```
numpy>=1.24
matplotlib>=3.7
Pillow>=10.0
```

## 9. Ejemplo de uso
```bash
# Desde la raíz del proyecto:
cd python
pip install -r requirements.txt
python analisis_cuantitativo.py 1  # Procesa imagen con ID=1
```

## 10. Notas de implementación
- Se usa matplotlib backend "Agg" para entornos sin display
- Todos los gráficos incluyen rejilla para facilitar lectura
- Los colores fueron seleccionados para buena distinguishabilidad
- El panel combinado mantiene proporciones adecuadas para visualización horizontal
- El script crea automáticamente el directorio de salida si no existe