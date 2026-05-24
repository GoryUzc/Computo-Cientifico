# Resultados del análisis — Compresión de Imágenes mediante SVD

**Algoritmo SVD:** Eigen::BDCSVD (ComputeFullU | ComputeFullV)  
**Valores de k evaluados:** 5, 20, 50, 100, 200  
**Fecha de generación:** ejecución automática vía `svd_compresion.exe`

---

## Resumen de métricas por imagen

| Imagen | Archivo | Dimensiones | Rango (r) | k=5 Error | k=5 Energía | k=5 Tasa | k=20 Error | k=20 Energía | k=20 Tasa | k=50 Error | k=50 Energía | k=50 Tasa | k=100 Error | k=100 Energía | k=100 Tasa | k=200 Error | k=200 Energía | k=200 Tasa |
|--------|---------|-------------|----------:|----------:|------------:|---------:|-----------:|------------:|----------:|-----------:|------------:|----------:|-----------:|-------------:|-----------:|-----------:|-------------:|-----------|
| ID 1 | edificio_gris_1.png | 275×183 | 183 | 0.2387 | 94.30% | 21.9× | 0.1651 | 97.27% | 5.5× | 0.1031 | 98.94% | 2.2× | 0.0429 | 99.82% | 1.1× | 0.0000 | 100.00% | 0.6× |
| ID 2 | geometria_gris_2.png | 360×360 | 360 | 0.6976 | 51.34% | 36.0× | 0.5849 | 65.79% | 9.0× | 0.4633 | 78.54% | 3.6× | 0.3125 | 90.24% | 1.8× | 0.1090 | 98.81% | 0.9× |
| ID 3 | lena_gris_3.png | 512×512 | 512 | 0.2007 | 95.97% | 51.2× | 0.1029 | 98.94% | 12.8× | 0.0572 | 99.67% | 5.1× | 0.0276 | 99.92% | 2.6× | 0.0068 | 100.00% | 1.3× |
| ID 4 | mandril_gris_4.png | 209×242 | 209 | 0.2585 | 93.32% | 22.4× | 0.1058 | 98.88% | 5.6× | 0.0513 | 99.74% | 2.2× | 0.0152 | 99.98% | 1.1× | 0.0000 | 100.00% | 0.6× |
| ID 5 | edificio_rgb_5.png | 348×461 | 348 | 0.2908 | 91.55% | 39.6× | 0.1927 | 96.29% | 9.9× | 0.1256 | 98.42% | 4.0× | 0.0734 | 99.46% | 2.0× | 0.0264 | 99.93% | 1.0× |
| ID 6 | lena_rgb_6.png | 512×512 | 512 | 0.1992 | 96.03% | 51.2× | 0.1057 | 98.88% | 12.8× | 0.0689 | 99.53% | 5.1× | 0.0421 | 99.82% | 2.6× | 0.0189 | 99.96% | 1.3× |
| ID 7 | mandril_rgb_7.png | 236×325 | 236 | 0.1515 | 97.71% | 27.3× | 0.0709 | 99.50% | 6.8× | 0.0422 | 99.82% | 2.7× | 0.0210 | 99.96% | 1.4× | 0.0029 | 100.00% | 0.7× |
| ID 8 | pimiento_rgb_8.png | 225×225 | 225 | 0.0401 | 99.84% | 22.5× | 0.0153 | 99.98% | 5.6× | 0.0064 | 100.00% | 2.2× | 0.0009 | 100.00% | 1.1× | 0.0000 | 100.00% | 0.6× |

---

## Métricas detalladas por imagen

### Imagen 1 — edificio_gris_1.png (275×183, gris)

| k | Error relativo | Energía capturada | Tasa de compresión |
|---|----------------|-------------------|-------------------|
| 5 | 0.2387 | 94.30% | 21.9× |
| 20 | 0.1651 | 97.27% | 5.5× |
| 50 | 0.1031 | 98.94% | 2.2× |
| 100 | 0.0429 | 99.82% | 1.1× |
| 183 | 0.0000 | 100.00% | 0.6× |

### Imagen 2 — geometria_gris_2.png (360×360, gris)

| k | Error relativo | Energía capturada | Tasa de compresión |
|---|----------------|-------------------|-------------------|
| 5 | 0.6976 | 51.34% | 36.0× |
| 20 | 0.5849 | 65.79% | 9.0× |
| 50 | 0.4633 | 78.54% | 3.6× |
| 100 | 0.3125 | 90.24% | 1.8× |
| 200 | 0.1090 | 98.81% | 0.9× |

### Imagen 3 — lena_gris_3.png (512×512, gris)

| k | Error relativo | Energía capturada | Tasa de compresión |
|---|----------------|-------------------|-------------------|
| 5 | 0.2007 | 95.97% | 51.2× |
| 20 | 0.1029 | 98.94% | 12.8× |
| 50 | 0.0572 | 99.67% | 5.1× |
| 100 | 0.0276 | 99.92% | 2.6× |
| 200 | 0.0068 | 100.00% | 1.3× |

### Imagen 4 — mandril_gris_4.png (209×242, gris)

| k | Error relativo | Energía capturada | Tasa de compresión |
|---|----------------|-------------------|-------------------|
| 5 | 0.2585 | 93.32% | 22.4× |
| 20 | 0.1058 | 98.88% | 5.6× |
| 50 | 0.0513 | 99.74% | 2.2× |
| 100 | 0.0152 | 99.98% | 1.1× |
| 200 | 0.0000 | 100.00% | 0.6× |

### Imagen 5 — edificio_rgb_5.png (348×461, color)

| k | Error relativo | Energía capturada | Tasa de compresión |
|---|----------------|-------------------|-------------------|
| 5 | 0.2908 | 91.55% | 39.6× |
| 20 | 0.1927 | 96.29% | 9.9× |
| 50 | 0.1256 | 98.42% | 4.0× |
| 100 | 0.0734 | 99.46% | 2.0× |
| 200 | 0.0264 | 99.93% | 1.0× |

### Imagen 6 — lena_rgb_6.png (512×512, color)

| k | Error relativo | Energía capturada | Tasa de compresión |
|---|----------------|-------------------|-------------------|
| 5 | 0.1992 | 96.03% | 51.2× |
| 20 | 0.1057 | 98.88% | 12.8× |
| 50 | 0.0689 | 99.53% | 5.1× |
| 100 | 0.0421 | 99.82% | 2.6× |
| 200 | 0.0189 | 99.96% | 1.3× |

### Imagen 7 — mandril_rgb_7.png (236×325, color)

| k | Error relativo | Energía capturada | Tasa de compresión |
|---|----------------|-------------------|-------------------|
| 5 | 0.1515 | 97.71% | 27.3× |
| 20 | 0.0709 | 99.50% | 6.8× |
| 50 | 0.0422 | 99.82% | 2.7× |
| 100 | 0.0210 | 99.96% | 1.4× |
| 200 | 0.0029 | 100.00% | 0.7× |

### Imagen 8 — pimiento_rgb_8.png (225×225, color)

| k | Error relativo | Energía capturada | Tasa de compresión |
|---|----------------|-------------------|-------------------|
| 5 | 0.0401 | 99.84% | 22.5× |
| 20 | 0.0153 | 99.98% | 5.6× |
| 50 | 0.0064 | 100.00% | 2.2× |
| 100 | 0.0009 | 100.00% | 1.1× |
| 200 | 0.0000 | 100.00% | 0.6× |

---

## Análisis comparativo

### Mejor rendimiento de compresión
- **Imagen 8 (pimiento_rgb_8.png)**: Con k=20 logra 99.98% de energía con solo error 0.0153
- **Imagen 7 (mandril_rgb_7.png)**: Excelente equilibrio, con k=50 alcanza 99.82% de energía

### Mayor dificultad de compresión
- **Imagen 2 (geometria_gris_2.png)**: Requiere k=100 para alcanzar solo 90% de energía, indicando alta variabilidad espectral

### Imágenes en escala de grises vs color
- Las imágenes en gris (ID 1,3,4) muestran patrones similares de compresión
- Las imágenes color (ID 5,6,7,8) tienen valores de error y energía comparables, indicando que el algoritmo procesa cada canal por separado eficientemente

---
