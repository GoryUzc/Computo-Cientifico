# SPEC-006: Implementación Python de Visualización para RBF

> **Documento de Especificación Técnica**  
> **Proyecto:** Taller 6 — Interpolación mediante Redes de Base Radial  
> **Módulo:** `visualizar_rbf.py`  
> **Metodología:** SDD (Specification-Driven Development)  
> **Fecha:** 9 de octubre de 2026

---

## 1. Propósito

Especificar la implementación en Python de un script que genere visualizaciones profesionales del modelo RBF, con énfasis en **legibilidad** y **ausencia de superposición de leyendas**. El script debe:

1. Visualizar los **100 puntos de entrenamiento** sobre el dominio
2. Comparar las **3 superficies predichas** (ε = 0.5, 1.5, 5.0) con la función real
3. Mostrar el **efecto del parámetro ε** mediante cortes transversales
4. Analizar el **error relativo** de cada aproximación
5. Generar una vista **3D destacada** del mejor modelo (ε óptimo)

**Enfoque visual:** Se priorizarán gráficos de **contorno (contourf)** en lugar de superficies 3D para la comparación, ya que son más legibles y no sufren de oclusión.

---

## 2. Datos de Entrada (CSV desde C++)

### 2.1 `resultados/datos_entrenamiento.csv`

```csv
x,y,z
-1.234567,0.567890,-0.823456
...
```
**Filas:** 100 puntos con ruido gaussiano

### 2.2 `resultados/superficie_eps_0.50.csv`
### 2.3 `resultados/superficie_eps_1.50.csv`
### 2.4 `resultados/superficie_eps_5.00.csv`

```csv
x,y,z_predicho
-2.000000,-2.000000,0.123456
...
```
**Filas:** 2500 puntos (malla 50×50)

---

## 3. Módulo a Implementar

### 3.1 Script Principal: `visualizar_rbf.py`

```python
# python/visualizar_rbf.py
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from mpl_toolkits.mplot3d import Axes3D
from pathlib import Path
import sys

class VisualizadorRBF:
    """Clase para generar visualizaciones profesionales del modelo RBF."""
    
    def __init__(self, resultados_dir: str):
        self.resultados_dir = Path(resultados_dir)
        self.graficos_dir = self.resultados_dir / 'graficos'
        self.graficos_dir.mkdir(parents=True, exist_ok=True)
        
        # Configuración global de matplotlib para legibilidad
        plt.rcParams.update({
            'font.size': 11,
            'axes.titlesize': 13,
            'axes.labelsize': 12,
            'legend.fontsize': 10,
            'figure.titlesize': 15,
            'figure.dpi': 100,
            'savefig.dpi': 300,
            'savefig.bbox': 'tight'
        })
        
        # Cargar datos
        self.df_train = self._cargar_csv('datos_entrenamiento.csv')
        self.df_eps = {
            0.5: self._cargar_csv('superficie_eps_0.50.csv'),
            1.5: self._cargar_csv('superficie_eps_1.50.csv'),
            5.0: self._cargar_csv('superficie_eps_5.00.csv')
        }
        
        # Crear malla para contourf
        self._preparar_mallas()
    
    def _cargar_csv(self, filename: str) -> pd.DataFrame:
        filepath = self.resultados_dir / filename
        if not filepath.exists():
            print(f"[ERROR] No se encontró: {filepath}")
            sys.exit(1)
        return pd.read_csv(filepath)
    
    def _preparar_mallas(self):
        """Convierte los CSV planos en matrices 2D para contourf."""
        df_ref = self.df_eps[1.5]
        self.x_grid = np.sort(df_ref['x'].unique())
        self.y_grid = np.sort(df_ref['y'].unique())
        self.X, self.Y = np.meshgrid(self.x_grid, self.y_grid)
        
        # Función real (sin ruido)
        self.Z_real = np.sin(self.X) * np.cos(self.Y)
        
        # Superficies predichas
        self.Z_pred = {}
        for eps, df in self.df_eps.items():
            Z = df['z_predicho'].values.reshape(len(self.y_grid), len(self.x_grid))
            self.Z_pred[eps] = Z
    
    # GRÁFICA 1: Puntos de Entrenamiento
    def graficar_puntos_entrenamiento(self):
        """Scatter 2D de los 100 puntos de entrenamiento."""
        fig, ax = plt.subplots(figsize=(9, 7))
        
        scatter = ax.scatter(
            self.df_train['x'], self.df_train['y'],
            c=self.df_train['z'], cmap='viridis',
            s=60, edgecolors='black', linewidth=0.5,
            alpha=0.9
        )
        
        cbar = plt.colorbar(scatter, ax=ax, label='Valor z (con ruido)')
        cbar.ax.tick_params(labelsize=10)
        
        ax.set_xlabel('x', fontsize=12, fontweight='bold')
        ax.set_ylabel('y', fontsize=12, fontweight='bold')
        ax.set_title('Puntos de Entrenamiento (N=100)\n'
                    r'$z = \sin(x)\cos(y) + \mathcal{N}(0, 0.1^2)$',
                    fontsize=14, fontweight='bold', pad=15)
        ax.set_xlim(-2.1, 2.1)
        ax.set_ylim(-2.1, 2.1)
        ax.grid(True, alpha=0.3, linestyle='--')
        ax.set_aspect('equal')
        
        stats_text = (f"N = {len(self.df_train)}\n"
                     f"σ = 0.1\n"
                     f"Dominio: [-2, 2]²")
        ax.text(0.02, 0.98, stats_text,
               transform=ax.transAxes,
               fontsize=10,
               verticalalignment='top',
               bbox=dict(boxstyle='round,pad=0.5', 
                        facecolor='wheat', alpha=0.8,
                        edgecolor='black', linewidth=0.5))
        
        plt.tight_layout()
        plt.savefig(self.graficos_dir / '01_puntos_entrenamiento.png')
        plt.close()
        print(f"[OK] 01_puntos_entrenamiento.png")
    
    # GRÁFICA 2: Comparación de Contornos (2x2)
    def graficar_comparacion_contornos(self):
        """Contourf comparando función real vs 3 aproximaciones RBF."""
        fig, axes = plt.subplots(2, 2, figsize=(14, 12))
        
        vmin, vmax = -1.0, 1.0
        levels = np.linspace(vmin, vmax, 25)
        
        titulos = [
            ('Función Real', self.Z_real, 'Greens'),
            (r'RBF ($\epsilon = 0.5$)', self.Z_pred[0.5], 'Reds'),
            (r'RBF ($\epsilon = 1.5$)', self.Z_pred[1.5], 'Blues'),
            (r'RBF ($\epsilon = 5.0$)', self.Z_pred[5.0], 'Oranges')
        ]
        
        for idx, (titulo, Z, cmap_name) in enumerate(titulos):
            ax = axes[idx // 2, idx % 2]
            
            cf = ax.contourf(self.X, self.Y, Z, levels=levels, 
                           cmap=cmap_name, extend='both')
            
            cs = ax.contour(self.X, self.Y, Z, levels=levels[::4],
                          colors='black', linewidths=0.5, alpha=0.4)
            
            ax.scatter(self.df_train['x'], self.df_train['y'],
                      c='white', s=15, edgecolors='black',
                      linewidth=0.5, alpha=0.7, zorder=5)
            
            ax.set_title(titulo, fontsize=13, fontweight='bold', pad=10)
            ax.set_xlabel('x')
            ax.set_ylabel('y')
            ax.set_xlim(-2, 2)
            ax.set_ylim(-2, 2)
            ax.set_aspect('equal')
            
            cbar = plt.colorbar(cf, ax=ax, fraction=0.046, pad=0.04)
            cbar.ax.tick_params(labelsize=9)
        
        fig.suptitle('Comparación de Superficies RBF con Diferentes Valores de $\\epsilon$\n'
                    'Puntos blancos: datos de entrenamiento',
                    fontsize=15, fontweight='bold', y=0.995)
        
        plt.tight_layout(rect=[0, 0, 1, 0.96])
        plt.savefig(self.graficos_dir / '02_comparacion_contornos.png')
        plt.close()
        print(f"[OK] 02_comparacion_contornos.png")
    
    # GRÁFICA 3: Corte Transversal (y=0)
    def graficar_corte_transversal(self):
        """Corte en y=0 para ver el perfil de cada aproximación."""
        fig, ax = plt.subplots(figsize=(11, 7))
        
        idx_y0 = np.argmin(np.abs(self.y_grid))
        
        x_slice = self.X[idx_y0, :]
        z_real_slice = self.Z_real[idx_y0, :]
        
        estilos = {
            0.5: {'color': '#d62728', 'ls': '--', 'label': r'$\epsilon = 0.5$ (oscilaciones)'},
            1.5: {'color': '#1f77b4', 'ls': '-',  'label': r'$\epsilon = 1.5$ (óptimo)'},
            5.0: {'color': '#ff7f0e', 'ls': '-.', 'label': r'$\epsilon = 5.0$ (picos locales)'}
        }
        
        ax.plot(x_slice, z_real_slice, 'k-', linewidth=3, 
               label='Función real $\\sin(x)\\cos(0)$', zorder=10)
        
        for eps, estilo in estilos.items():
            z_slice = self.Z_pred[eps][idx_y0, :]
            ax.plot(x_slice, z_slice, 
                   color=estilo['color'], 
                   linestyle=estilo['ls'],
                   linewidth=2.2, 
                   label=estilo['label'],
                   alpha=0.9)
        
        mask = np.abs(self.df_train['y']) < 0.3
        if mask.any():
            ax.scatter(self.df_train.loc[mask, 'x'], 
                      self.df_train.loc[mask, 'z'],
                      c='purple', s=50, marker='o',
                      edgecolors='black', linewidth=0.8,
                      label='Datos de entrenamiento (|y|<0.3)',
                      zorder=15)
        
        ax.set_xlabel('x', fontsize=13, fontweight='bold')
        ax.set_ylabel('z', fontsize=13, fontweight='bold')
        ax.set_title('Corte Transversal en y = 0\n'
                    'Comparación de Aproximaciones RBF vs Función Real',
                    fontsize=14, fontweight='bold', pad=15)
        
        # Leyenda FUERA de la gráfica para evitar superposición
        ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.15),
                 ncol=2, frameon=True, fancybox=True,
                 shadow=True, fontsize=10)
        
        ax.grid(True, alpha=0.3, linestyle='--')
        ax.set_xlim(-2, 2)
        ax.set_ylim(-1.3, 1.3)
        
        plt.tight_layout()
        plt.savefig(self.graficos_dir / '03_corte_transversal.png')
        plt.close()
        print(f"[OK] 03_corte_transversal.png")
    
    # GRÁFICA 4: Mapa de Error Relativo
    def graficar_error_relativo(self):
        """Heatmap del error |z_pred - z_real| para cada ε."""
        fig, axes = plt.subplots(1, 3, figsize=(16, 5))
        
        epsilons = [0.5, 1.5, 5.0]
        etiquetas = [r'$\epsilon = 0.5$', r'$\epsilon = 1.5$', r'$\epsilon = 5.0$']
        
        errores = []
        for eps in epsilons:
            error = np.abs(self.Z_pred[eps] - self.Z_real)
            errores.append(error)
        
        vmax = max(e.max() for e in errores)
        
        for idx, (eps, etiqueta, error) in enumerate(zip(epsilons, etiquetas, errores)):
            ax = axes[idx]
            
            im = ax.contourf(self.X, self.Y, error, levels=20,
                           cmap='hot_r', vmin=0, vmax=vmax)
            
            ax.set_title(f'Error Absoluto\n{etiqueta}',
                        fontsize=12, fontweight='bold')
            ax.set_xlabel('x')
            ax.set_ylabel('y')
            ax.set_aspect('equal')
            
            cbar = plt.colorbar(im, ax=ax, fraction=0.046, pad=0.04)
            cbar.ax.tick_params(labelsize=9)
            cbar.set_label('|z_pred - z_real|', fontsize=10)
            
            stats_text = (f'Medio: {error.mean():.4f}\n'
                         f'Máx:   {error.max():.4f}')
            ax.text(0.02, 0.98, stats_text,
                   transform=ax.transAxes,
                   fontsize=9,
                   verticalalignment='top',
                   bbox=dict(boxstyle='round,pad=0.4',
                            facecolor='white', alpha=0.9,
                            edgecolor='black', linewidth=0.8))
        
        fig.suptitle('Mapa de Error Absoluto por Valor de $\\epsilon$\n'
                    '(Escala de color común para comparación justa)',
                    fontsize=14, fontweight='bold', y=1.02)
        
        plt.tight_layout()
        plt.savefig(self.graficos_dir / '04_error_relativo.png')
        plt.close()
        print(f"[OK] 04_error_relativo.png")
    
    # GRÁFICA 5: Vista 3D del Mejor Modelo
    def graficar_vista_3d(self):
        """Superficie 3D del mejor modelo (ε=1.5) con puntos de entrenamiento."""
        fig = plt.figure(figsize=(12, 9))
        ax = fig.add_subplot(111, projection='3d')
        
        surf = ax.plot_surface(self.X, self.Y, self.Z_pred[1.5],
                              cmap='viridis', alpha=0.85,
                              edgecolor='none', antialiased=True)
        
        ax.scatter(self.df_train['x'], self.df_train['y'], 
                  self.df_train['z'],
                  c='red', s=40, marker='o',
                  edgecolors='black', linewidth=0.5,
                  depthshade=True, label='Datos entrenamiento',
                  zorder=10)
        
        ax.set_xlabel('x', fontsize=12, fontweight='bold', labelpad=10)
        ax.set_ylabel('y', fontsize=12, fontweight='bold', labelpad=10)
        ax.set_zlabel('z', fontsize=12, fontweight='bold', labelpad=5)
        ax.set_title('Vista 3D del Mejor Modelo RBF ($\\epsilon = 1.5$)\n'
                    'Superficie interpolante + datos de entrenamiento',
                    fontsize=14, fontweight='bold', pad=20)
        
        cbar = fig.colorbar(surf, ax=ax, shrink=0.6, aspect=15, pad=0.1)
        cbar.set_label('z predicho', fontsize=11)
        cbar.ax.tick_params(labelsize=9)
        
        ax.view_init(elev=25, azim=135)
        ax.set_xlim(-2, 2)
        ax.set_ylim(-2, 2)
        ax.set_zlim(-1.2, 1.2)
        
        ax.legend(loc='upper left', fontsize=10,
                 frameon=True, fancybox=True, shadow=True)
        
        plt.tight_layout()
        plt.savefig(self.graficos_dir / '05_vista_3d.png')
        plt.close()
        print(f"[OK] 05_vista_3d.png")
    
    # GRÁFICA 6: Análisis del Parámetro ε
    def graficar_analisis_epsilon(self):
        """Estudio cuantitativo del efecto de ε."""
        fig, axes = plt.subplots(1, 2, figsize=(14, 5))
        
        epsilons = [0.5, 1.5, 5.0]
        etiquetas_eps = ['Pequeño\n(0.5)', 'Óptimo\n(1.5)', 'Grande\n(5.0)']
        
        errores_medios = []
        errores_maximos = []
        
        for eps in epsilons:
            error = np.abs(self.Z_pred[eps] - self.Z_real)
            errores_medios.append(error.mean())
            errores_maximos.append(error.max())
        
        ax = axes[0]
        x_pos = np.arange(len(epsilons))
        width = 0.35
        
        bars1 = ax.bar(x_pos - width/2, errores_medios, width,
                      label='Error medio', color='#1f77b4', alpha=0.8,
                      edgecolor='black', linewidth=0.8)
        bars2 = ax.bar(x_pos + width/2, errores_maximos, width,
                      label='Error máximo', color='#d62728', alpha=0.8,
                      edgecolor='black', linewidth=0.8)
        
        ax.set_xlabel('Valor de $\\epsilon$', fontsize=12, fontweight='bold')
        ax.set_ylabel('Error Absoluto', fontsize=12, fontweight='bold')
        ax.set_title('Análisis de Error por Valor de $\\epsilon$',
                    fontsize=13, fontweight='bold')
        ax.set_xticks(x_pos)
        ax.set_xticklabels(etiquetas_eps)
        ax.legend(loc='best', frameon=True, fancybox=True, shadow=True)
        ax.grid(True, alpha=0.3, axis='y', linestyle='--')
        
        for bars in [bars1, bars2]:
            for bar in bars:
                height = bar.get_height()
                ax.text(bar.get_x() + bar.get_width()/2., height,
                       f'{height:.3f}',
                       ha='center', va='bottom', fontsize=9)
        
        ax = axes[1]
        caracteristicas = [
            'Suavidad', 'Fidelidad\ndatos', 'Estabilidad\nnumérica',
            'Generalización'
        ]
        puntuaciones = {
            0.5: [3, 9, 2, 3],
            1.5: [9, 7, 8, 9],
            5.0: [2, 3, 9, 2]
        }
        
        x_pos = np.arange(len(caracteristicas))
        width = 0.25
        colores = ['#d62728', '#1f77b4', '#ff7f0e']
        
        for idx, eps in enumerate(epsilons):
            bars = ax.bar(x_pos + idx*width - width, 
                         puntuaciones[eps], width,
                         label=f'$\\epsilon = {eps}$',
                         color=colores[idx], alpha=0.8,
                         edgecolor='black', linewidth=0.8)
        
        ax.set_xlabel('Característica', fontsize=12, fontweight='bold')
        ax.set_ylabel('Puntuación (0-10)', fontsize=12, fontweight='bold')
        ax.set_title('Perfil Cualitativo de Cada $\\epsilon$',
                    fontsize=13, fontweight='bold')
        ax.set_xticks(x_pos)
        ax.set_xticklabels(caracteristicas)
        ax.set_ylim(0, 11)
        ax.legend(loc='best', frameon=True, fancybox=True, shadow=True)
        ax.grid(True, alpha=0.3, axis='y', linestyle='--')
        
        fig.suptitle('Análisis del Efecto del Parámetro de Forma $\\epsilon$',
                    fontsize=14, fontweight='bold', y=1.02)
        
        plt.tight_layout()
        plt.savefig(self.graficos_dir / '06_analisis_epsilon.png')
        plt.close()
        print(f"[OK] 06_analisis_epsilon.png")
    
    # REPORTE ESTADÍSTICO
    def generar_reporte(self):
        """Reporte estadístico en consola."""
        print("\n" + "="*80)
        print("REPORTE ESTADÍSTICO - MODELO RBF")
        print("="*80)
        
        print(f"\n▶ DATOS DE ENTRENAMIENTO:")
        print(f"  Número de puntos: {len(self.df_train)}")
        print(f"  Dominio: [{self.df_train['x'].min():.2f}, {self.df_train['x'].max():.2f}] × "
              f"[{self.df_train['y'].min():.2f}, {self.df_train['y'].max():.2f}]")
        print(f"  Rango z: [{self.df_train['z'].min():.4f}, {self.df_train['z'].max():.4f}]")
        
        print(f"\n▶ ANÁLISIS POR VALOR DE ε:")
        print(f"  {'ε':<8} {'Error Medio':<15} {'Error Máximo':<15}")
        print("  " + "-"*40)
        
        for eps in [0.5, 1.5, 5.0]:
            error = np.abs(self.Z_pred[eps] - self.Z_real)
            print(f"  {eps:<8.2f} {error.mean():<15.6f} {error.max():<15.6f}")
        
        print(f"\n▶ INTERPRETACIÓN:")
        print("  • ε = 0.5: Campanas anchas → oscilaciones (fenómeno de Runge)")
        print("  • ε = 1.5: Balance óptimo → superficie suave y fiel")
        print("  • ε = 5.0: Campanas estrechas → picos locales, poca generalización")
        
        print("\n" + "="*80)
    
    # EJECUCIÓN COMPLETA
    def generar_todas(self):
        """Genera todas las visualizaciones."""
        print("="*80)
        print("VISUALIZACIONES - INTERPOLACIÓN RBF")
        print("="*80)
        
        self.graficar_puntos_entrenamiento()
        self.graficar_comparacion_contornos()
        self.graficar_corte_transversal()
        self.graficar_error_relativo()
        self.graficar_vista_3d()
        self.graficar_analisis_epsilon()
        self.generar_reporte()
        
        print("\n" + "="*80)
        print("TODAS LAS VISUALIZACIONES GENERADAS CORRECTAMENTE")
        print(f"Ubicación: {self.graficos_dir}")
        print("="*80)


if __name__ == '__main__':
    resultados_dir = Path(__file__).parent.parent / 'resultados'
    
    if not resultados_dir.exists():
        print(f"[ERROR] Directorio no encontrado: {resultados_dir}")
        print("[INFO] Ejecuta primero el programa C++ para generar los CSV")
        sys.exit(1)
    
    vis = VisualizadorRBF(str(resultados_dir))
    vis.generar_todas()
```

---

## 4. Especificaciones Técnicas

### 4.1 Librerías Requeridas

| Librería | Versión | Propósito |
|----------|---------|-----------|
| `numpy` | 1.24+ | Operaciones matriciales |
| `pandas` | 2.0+ | Carga de CSV |
| `matplotlib` | 3.7+ | Visualización |

### 4.2 `requirements.txt`

```txt
numpy>=1.24.0
pandas>=2.0.0
matplotlib>=3.7.0
```

### 4.3 Estrategias Anti-Superposición de Leyendas

| Técnica | Aplicación |
|---------|-----------|
| **Leyendas fuera del área de datos** | Corte transversal usa `bbox_to_anchor=(0.5, -0.15)` |
| **`loc='best'`** | Matplotlib ubica automáticamente donde hay menos interferencia |
| **Subplots separados** | Cada ε tiene su propio panel en lugar de superponerse |
| **Cajas con fondo** | `bbox=dict(facecolor='white', alpha=0.9)` para legibilidad |
| **Colores contrastantes** | Paleta distintiva por ε (rojo, azul, naranja) |
| **Estilos de línea diferentes** | Sólido, punteado, discontinuo |

---

## 6. Criterios de Aceptación

| Criterio | Verificación | 
|----------|--------------|
| **6 gráficas generadas** | Archivos PNG en `graficos/` |
| **Sin superposición de leyendas** | Inspección visual |
| **Textos legibles** | Tamaño de fuente ≥ 10pt |
| **Colores contrastantes** | Paleta distintiva por ε |
| **Escala común en comparaciones** | Mismos `vmin`/`vmax` |
| **DPI 300** | Calidad para informe |
| **Reporte en consola** | Estadísticas impresas |

---

## 7. Checklist de Implementación

- [ ] Crear archivo `python/visualizar_rbf.py`
- [ ] Crear archivo `python/requirements.txt`

---

**Fin del documento SPEC-PY**