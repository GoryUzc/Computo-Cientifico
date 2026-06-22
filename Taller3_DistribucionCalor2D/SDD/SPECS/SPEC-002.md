# SPEC-002: Implementacion Python de Visualizaciones para Poisson 2D

> **Documento de Especificacion Tecnica**  
> **Proyecto:** Taller 3 - Simulacion de Distribucion de Calor 2D  

---

## 1. Proposito

Especificar la implementacion en Python de las visualizaciones requeridas para el Taller 3. El script debe leer los archivos CSV generados por el solver C++ y producir 4 graficas que cumplan con los entregables del taller:

1. **Patron de bandas** de la matriz dispersa A (spy plot)
2. **Heatmap con isotermas** de la distribucion de temperatura
3. **Comparacion con/sin fuente** de calor (side-by-side)
4. **Perfil transversal** u(x, 0.5) mostrando el efecto de la fuente

---

## 2. Datos de Entrada (CSV desde C++)

### 2.1 `..resultados/temperatura.csv`

Contiene solo los nodos internos (400 filas para n=20).

```csv
x,y,temperatura
0.047619,0.047619,25.123456
0.095238,0.047619,26.456789
...
```

### 2.2 `..resultados/temperatura_completa.csv`

Incluye nodos internos + fronteras (484 filas para n=20).

```csv
x,y,temperatura,tipo
0.000000,0.000000,50.000000,frontera
0.047619,0.000000,0.000000,frontera
...
0.047619,0.047619,25.123456,interno
...
```

### 2.3 `..resultados/temperatura_sin_fuente.csv`

Solucion sin fuente de calor (para comparacion).

### 2.4 `..resultados/matriz_A.csv`

Elementos no nulos de la matriz A (aproximadamente 1960 filas).

```csv
fila,columna,valor
0,0,4.0
0,1,-1.0
0,20,-1.0
...
```

---

## 3. Modulos a Implementar

### 3.1 Script Principal: `visualizar_calor.py`

```python
# python/visualizar_calor.py
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path
import sys

class VisualizadorPoisson:
    # Clase para generar las 4 visualizaciones requeridas del Taller 3.

    def __init__(self, resultados_dir):
        self.resultados_dir = Path(resultados_dir)
        self.graficos_dir = self.resultados_dir / 'graficos'
        self.graficos_dir.mkdir(parents=True, exist_ok=True)

        # Cargar datos
        self.df_temp = self._cargar_csv('temperatura.csv')
        self.df_temp_completa = self._cargar_csv('temperatura_completa.csv')
        self.df_temp_sin_fuente = self._cargar_csv('temperatura_sin_fuente.csv')
        self.df_matriz = self._cargar_csv('matriz_A.csv')

        # Obtener dimensiones de la malla
        self.n = int(np.sqrt(len(self.df_temp)))
        self.x_unique = np.sort(self.df_temp['x'].unique())
        self.y_unique = np.sort(self.df_temp['y'].unique())

    def _cargar_csv(self, filename):
        filepath = self.resultados_dir / filename
        if not filepath.exists():
            print(f"[ERROR] No se encontro: {filepath}")
            sys.exit(1)
        return pd.read_csv(filepath)

    def _crear_grid(self, df):
        # Convierte DataFrame a matriz 2D para visualizacion.
        x = df['x'].values
        y = df['y'].values
        z = df['temperatura'].values

        n_total = int(np.sqrt(len(df)))
        X = x.reshape(n_total, n_total)
        Y = y.reshape(n_total, n_total)
        Z = z.reshape(n_total, n_total)

        return X, Y, Z

    def graficar_patron_matriz(self):
        # ENTREGABLE 1: Spy plot de la matriz A.
        fig, ax = plt.subplots(figsize=(10, 8))

        filas = self.df_matriz['fila'].values
        columnas = self.df_matriz['columna'].values
        valores = self.df_matriz['valor'].values

        scatter = ax.scatter(columnas, filas, c=valores, cmap='RdBu_r', 
                            s=10, marker='s', edgecolors='none')

        ax.set_xlabel('Columna', fontsize=12)
        ax.set_ylabel('Fila', fontsize=12)
        ax.set_title(f'Patron de Bandas de la Matriz A (n={self.n})
'
                    f'{len(filas)} elementos no nulos de {self.n**2}x{self.n**2}',
                    fontsize=14, fontweight='bold')

        plt.colorbar(scatter, ax=ax, label='Valor')
        ax.set_aspect('equal')
        ax.invert_yaxis()

        plt.tight_layout()
        plt.savefig(self.graficos_dir / '01_patron_matriz_A.png', dpi=300)
        plt.close()
        print(f"[OK] Patron de matriz guardado.")

    def graficar_heatmap_isotermas(self):
        # ENTREGABLE 2: Heatmap con curvas de nivel.
        fig, ax = plt.subplots(figsize=(10, 8))

        X, Y, Z = self._crear_grid(self.df_temp)

        im = ax.pcolormesh(X, Y, Z, cmap='hot', shading='auto')

        niveles = np.linspace(Z.min(), Z.max(), 15)
        contour = ax.contour(X, Y, Z, levels=niveles, colors='cyan', 
                            linewidths=0.8, alpha=0.8)
        ax.clabel(contour, inline=True, fontsize=8, fmt='%.1fC')

        ax.set_xlabel('x [m]', fontsize=12)
        ax.set_ylabel('y [m]', fontsize=12)
        ax.set_title('Distribucion de Temperatura en Estado Estacionario
'
                    'con Fuente de Calor Puntual en (0.5, 0.5)',
                    fontsize=14, fontweight='bold')
        ax.set_aspect('equal')

        plt.colorbar(im, ax=ax, label='Temperatura [C]')

        plt.tight_layout()
        plt.savefig(self.graficos_dir / '02_heatmap_isotermas.png', dpi=300)
        plt.close()
        print(f"[OK] Heatmap con isotermas guardado.")

    def graficar_comparacion_fuentes(self):
        # ENTREGABLE 3: Comparacion visual con y sin fuente de calor.
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 7))

        X1, Y1, Z1 = self._crear_grid(self.df_temp)
        im1 = ax1.pcolormesh(X1, Y1, Z1, cmap='hot', shading='auto')
        ax1.contour(X1, Y1, Z1, levels=10, colors='cyan', linewidths=0.6, alpha=0.7)
        ax1.set_title('CON Fuente de Calor (Q = 1000)', fontsize=13, fontweight='bold')
        ax1.set_xlabel('x [m]')
        ax1.set_ylabel('y [m]')
        ax1.set_aspect('equal')
        plt.colorbar(im1, ax=ax1, label='Temperatura [C]')

        X2, Y2, Z2 = self._crear_grid(self.df_temp_sin_fuente)
        im2 = ax2.pcolormesh(X2, Y2, Z2, cmap='hot', shading='auto',
                             vmin=Z1.min(), vmax=Z1.max())
        ax2.contour(X2, Y2, Z2, levels=10, colors='cyan', linewidths=0.6, alpha=0.7)
        ax2.set_title('SIN Fuente de Calor', fontsize=13, fontweight='bold')
        ax2.set_xlabel('x [m]')
        ax2.set_ylabel('y [m]')
        ax2.set_aspect('equal')
        plt.colorbar(im2, ax=ax2, label='Temperatura [C]')

        fig.suptitle('Analisis Fisico: Efecto de la Fuente de Calor Puntual',
                    fontsize=15, fontweight='bold', y=1.02)

        plt.tight_layout()
        plt.savefig(self.graficos_dir / '03_comparacion_fuentes.png', dpi=300, 
                   bbox_inches='tight')
        plt.close()
        print(f"[OK] Comparacion guardada.")

        centro_idx = self.n // 2
        T_con = Z1[centro_idx, centro_idx]
        T_sin = Z2[centro_idx, centro_idx]
        print(f"[INFO] Temperatura en centro (0.5, 0.5):")
        print(f"       Con fuente:    {T_con:.4f}C")
        print(f"       Sin fuente:    {T_sin:.4f}C")
        print(f"       Diferencia:    {T_con - T_sin:.4f}C")

    def graficar_perfil_transversal(self):
        # ENTREGABLE 4: Perfil u(x, 0.5) mostrando el pico de la fuente.
        fig, ax = plt.subplots(figsize=(12, 6))

        y_objetivo = 0.5
        tolerancia = 0.01

        mask_con = np.abs(self.df_temp['y'] - y_objetivo) < tolerancia
        perfil_con = self.df_temp[mask_con].sort_values('x')

        mask_sin = np.abs(self.df_temp_sin_fuente['y'] - y_objetivo) < tolerancia
        perfil_sin = self.df_temp_sin_fuente[mask_sin].sort_values('x')

        x_completo_con = np.concatenate([[0.0], perfil_con['x'].values, [1.0]])
        T_completo_con = np.concatenate([[50.0], perfil_con['temperatura'].values, [50.0]])

        x_completo_sin = np.concatenate([[0.0], perfil_sin['x'].values, [1.0]])
        T_completo_sin = np.concatenate([[50.0], perfil_sin['temperatura'].values, [50.0]])

        ax.plot(x_completo_con, T_completo_con, 'r-o', linewidth=2, 
               markersize=4, label='Con fuente (Q=1000)')
        ax.plot(x_completo_sin, T_completo_sin, 'b--s', linewidth=2, 
               markersize=4, label='Sin fuente')

        idx_pico = np.argmax(T_completo_con)
        ax.plot(x_completo_con[idx_pico], T_completo_con[idx_pico], 
               'g*', markersize=15, label=f'Pico: {T_completo_con[idx_pico]:.2f}C')

        ax.set_xlabel('x [m]', fontsize=12)
        ax.set_ylabel('Temperatura u(x, 0.5) [C]', fontsize=12)
        ax.set_title('Perfil Transversal de Temperatura en y = 0.5
'
                    'Efecto de la Fuente de Calor Puntual',
                    fontsize=14, fontweight='bold')
        ax.legend(fontsize=11)
        ax.grid(True, alpha=0.3)
        ax.set_xlim(0, 1)

        plt.tight_layout()
        plt.savefig(self.graficos_dir / '04_perfil_transversal.png', dpi=300)
        plt.close()
        print(f"[OK] Perfil transversal guardado.")

    def generar_todas(self):
        # Ejecuta todas las visualizaciones.
        print("="*60)
        print("VISUALIZACIONES - TALLER 3: POISSON 2D")
        print("="*60)
        print(f"[INFO] Malla: {self.n}x{self.n} nodos internos")
        print(f"[INFO] Total incognitas: {self.n**2}")
        print()

        self.graficar_patron_matriz()
        self.graficar_heatmap_isotermas()
        self.graficar_comparacion_fuentes()
        self.graficar_perfil_transversal()

        print()
        print("="*60)
        print("TODAS LAS VISUALIZACIONES GENERADAS CORRECTAMENTE")
        print("="*60)


if __name__ == '__main__':
    resultados_dir = Path(__file__).parent.parent / 'resultados'

    if not resultados_dir.exists():
        print(f"[ERROR] Directorio no encontrado: {resultados_dir}")
        print("[INFO] Ejecuta primero el solver C++ para generar los CSV")
        sys.exit(1)

    vis = VisualizadorPoisson(str(resultados_dir))
    vis.generar_todas()
```

---

## 4. Especificaciones Tecnicas

### 4.1 Librerias Requeridas

| Libreria | Version | Proposito |
|----------|---------|-----------|
| `numpy` | 1.24+ | Manipulacion de arrays |
| `pandas` | 2.0+ | Carga de CSV |
| `matplotlib` | 3.7+ | Visualizacion |

### 4.2 `requirements.txt`

> INSTALADOS

```txt
numpy>=1.24.0
pandas>=2.0.0
matplotlib>=3.7.0
```

### 4.3 Configuracion de Graficas

| Parametro | Valor | Razon |
|-----------|-------|-------|
| `dpi` | 300 | Calidad para informe LaTeX |
| `figsize` | Variable | Ajustado por tipo de grafica |
| `cmap` (temperatura) | `'hot'` | Intuitivo para calor |
| `cmap` (matriz) | `'RdBu_r'` | Distingue +4 de -1 |
| `isotermas` | 15 niveles | Detalle adecuado |
| `bbox_inches` | `'tight'` | Evita recorte de etiquetas |

---

## 5. Estructura de Salida

```
resultados/
├── graficos/
│   ├── 01_patron_matriz_A.png        # Spy plot
│   ├── 02_heatmap_isotermas.png      # Heatmap + contour
│   ├── 03_comparacion_fuentes.png    # Side-by-side
│   └── 04_perfil_transversal.png     # Line plot
├── temperatura.csv
├── temperatura_completa.csv
├── temperatura_sin_fuente.csv
└── matriz_A.csv
```

---

## 6. Criterios de Aceptacion

| Criterio | Verificacion | Estado |
|----------|--------------|--------|
| **4 graficas generadas** | Existen los 4 archivos PNG en `graficos/` | ⬜ |
| **Patron de 5 diagonales** | Visualmente se distinguen las 5 bandas | ⬜ |
| **Isotermas visibles** | Las curvas cyan se superponen al heatmap | ⬜ |
| **Efecto de fuente visible** | El centro en "con fuente" es mas caliente | ⬜ |
| **Pico en perfil transversal** | Se observa un maximo local en x ~ 0.5 | ⬜ |
| **Escala consistente** | Comparacion usa el mismo rango de colores | ⬜ |
| **Fronteras correctas** | Perfil incluye T(0) = T(1) = 50C | ⬜ |
| **Resolucion adecuada** | Imagenes nitidas a 300 DPI | ⬜ |

---

## 7. Validaciones Fisicas Esperadas

### 7.1 Simetria Izquierda-Derecha

Dado que T_izquierda = T_derecha = 50C, la solucion debe ser simetrica:

$$u(x, y) = u(1-x, y)$$

**Verificacion en Python:**

```python
Z_simetrica = np.fliplr(Z)
error_simetria = np.max(np.abs(Z - Z_simetrica))
print(f"Error de simetria: {error_simetria:.2e}")
assert error_simetria < 1e-10, "La solucion no es simetrica"
```

### 7.2 Gradiente Vertical

La temperatura debe aumentar de y=0 (0C) a y=1 (100C):

$$\frac{\partial T}{\partial y} > 0 \quad \text{(en promedio)}$$

### 7.3 Efecto de la Fuente

La temperatura en el centro con fuente debe ser mayor que sin fuente:

$$T_{con}(0.5, 0.5) > T_{sin}(0.5, 0.5)$$

---

## 8. Checklist de Implementación

- [x] Crear archivo `python/visualizar_calor.py`
- [x] Crear archivo `python/requirements.txt`
- [x] Ejecutar solver C++ primero para generar CSV
- [x] Ejecutar script Python: `python python/visualizar_calor.py`
- [x] Verificar que se generen los 4 PNG

---

**Fin del documento SPEC-002.md**
