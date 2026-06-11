# SPEC-002 — Script Python: Visualización del PCA Genético

> Documento derivado de SPEC-000 y SPEC-001. Especificación del script Python
> encargado de generar la visualización del análisis de componentes principales.
>
> **Estado:** Especificación para implementación.

---

## 1. Propósito

Este documento especifica el script Python `visualizar_pca.py` encargado de:

1. Leer las coordenadas PCA exportadas por el programa C++ (`resultado_pca.csv`).
2. Generar un **scatter plot** de PC1 vs PC2 donde cada punto representa un individuo.
3. Identificar visualmente clusters poblacionales y posibles individuos de mezcla (admixture).
4. Exportar la figura como PNG de alta calidad para el informe.

---

## 2. requirements.txt

```
numpy>=1.24
matplotlib>=3.7
pandas>=2.0
```

## 3. Ubicación y estructura

- **Archivo:** `python/visualizar_pca.py`
- **Tipo:** Script Python 3.x
- **Líneas estimadas:** ~80
- **Dependencias:** `matplotlib`, `numpy`, `pandas` 

---

## 4. Flujo de trabajo

```
1. Lectura de resultado_pca.csv desde resultados/
2. Extracción de columnas PC1 y PC2
3. Cálculo de varianza explicada (lectura de varianza.csv)
4. Generación de scatter plot con estilo profesional
5. Anotación de ejes con porcentajes de varianza
6. Guardado de figura en resultados/grafico_pca.png
7. Mensaje de confirmación
```

---

## 5. Argumentos de línea de comandos

| Argumento | Descripción | Valor por defecto |
|-----------|-------------|-------------------|
| `--input` | Ruta al CSV de coordenadas PCA | `resultados/resultado_pca.csv` |
| `--varianza` | Ruta al CSV de varianza explicada | `resultados/varianza.csv` |
| `--output` | Ruta de salida del PNG | `resultados/grafico_pca.png` |
| `--titulo` | Título del gráfico | `"Análisis de Componentes Principales - Estructura Poblacional"` |

Uso:
```bash
# Desde la raíz del proyecto
python python/visualizar_pca.py

# Con rutas relativas personalizadas
python python/visualizar_pca.py --input resultados/resultado_pca.csv --output resultados/grafico.png
```

---

## 6. Funciones principales implementadas

### 6.1 `leer_pca(ruta) -> pd.DataFrame`

- **Entrada:** ruta al archivo `resultado_pca.csv`
- **Salida:** DataFrame con columnas `Individuo`, `PC1`, `PC2`

### 6.2 `leer_varianza(ruta) -> pd.DataFrame`

- **Entrada:** ruta al archivo `varianza.csv`
- **Salida:** DataFrame con columnas `Componente`, `VarianzaExplicada`

### 6.3 `generar_scatter_plot(pca_data, varianza_data, titulo, ruta_salida)`

**Características implementadas:**
- Backend `Agg` para entornos sin display
- Tamaño: 10 × 8 pulgadas, DPI 150
- Color: colormap `viridis` basado en PC1
- Tamaño puntos: 80, borde negro, alpha 0.7
- Líneas de referencia en x=0 y y=0 (gris punteado)
- Grid: sí, linestyle `--`, alpha 0.3
- Numeración de individuos cercas de cada punto
- Etiquetas incluyen porcentaje de varianza explicada
- Fondo blanco (facecolor='white')

---

## 7. Formato de salida

### Archivo generado
- `resultados/grafico_pca.png` — Scatter plot PC1 vs PC2 (10×8 pulgadas, 150 DPI)

### Configuración
- Backend: `Agg` (para entornos sin display)
- Formato: PNG con fondo blanco
- Bbox: `tight` para márgenes optimizados

---

## 8. Integración con el sistema C++

El script puede invocarse desde C++ principal con:

```cpp
// En main.cpp (opcional - actualmente se ejecuta manualmente)
// system("python python/visualizar_pca.py");
```

---

## 9. Ejemplo de uso

```bash
# Desde la raíz del proyecto
python python/visualizar_pca.py

# Con parámetros personalizados (desde cualquier directorio)
python python/visualizar_pca.py --input ../resultados/resultado_pca.csv --output ../resultados/mi_grafico.png
```

---

## 10. Notas de implementación

- Se usa matplotlib backend `"Agg"` para entornos sin display
- Los colores usan colormap `viridis` para visualización científica estándar
- Numeración automática de individuos en el gráfico
- Creación automática del directorio de salida si no existe
- Manejo de errores básico para archivos no encontrados

---

## 11. Criterios de aceptación del gráfico

| Criterio | Cómo se verifica |
|----------|-----------------|
| **Archivo generado** | `grafico_pca.png` existe y tiene tamaño > 0 bytes |
| **Dimensiones** | 10 × 8 pulgadas, 150 DPI |
| **Puntos visibles** | 30 puntos dispersos en el plano |
| **Ejes etiquetados** | PC1 y PC2 con porcentajes de varianza |
| **Título presente** | Título descriptivo centrado |
| **Grid activo** | Líneas de referencia visibles |
| **Interpretación posible** | Clusters o gradientes visibles a simple vista |

---
