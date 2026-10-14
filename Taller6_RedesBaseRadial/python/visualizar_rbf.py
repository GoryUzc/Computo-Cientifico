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
            0.5: self._cargar_csv('superficie_eps_0.500000.csv'),
            1.5: self._cargar_csv('superficie_eps_1.500000.csv'),
            5.0: self._cargar_csv('superficie_eps_5.000000.csv')
        }
        
        # Crear malla para contourf
        self._preparar_mallas()
    
    def _cargar_csv(self, filename: str) -> pd.DataFrame:
        filepath = self.resultados_dir / filename
        if not filepath.exists():
            # Intentar sin ceros extras si falla
            alt_filename = filename.replace('000000', '')
            filepath_alt = self.resultados_dir / alt_filename
            if filepath_alt.exists():
                return pd.read_csv(filepath_alt)
            print(f"[ERROR] No se encontró: {filepath}")
            sys.exit(1)
        return pd.read_csv(filepath)
    
    def _preparar_mallas(self):
        """Convierte los CSV planos en matrices 2D para contourf."""
        # Usar epsilon=1.5 como referencia para la malla
        df_ref = self.df_eps[1.5]
        self.x_grid = np.sort(df_ref['x'].unique())
        self.y_grid = np.sort(df_ref['y'].unique())
        self.X, self.Y = np.meshgrid(self.x_grid, self.y_grid)
        
        # Función real (sin ruido)
        self.Z_real = np.sin(self.X) * np.cos(self.Y)
        
        # Superficies predichas
        self.Z_pred = {}
        for eps, df in self.df_eps.items():
            # Asegurar ordenamiento correcto para reshape
            df_sorted = df.sort_values(by=['y', 'x'])
            Z = df_sorted['z_predicho'].values.reshape(len(self.y_grid), len(self.x_grid))
            self.Z_pred[eps] = Z
    
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
            
            ax.contour(self.X, self.Y, Z, levels=levels[::4],
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
            
            plt.colorbar(cf, ax=ax, fraction=0.046, pad=0.04)
        
        fig.suptitle('Comparación de Superficies RBF con Diferentes Valores de $\\epsilon$\n'
                    'Puntos blancos: datos de entrenamiento',
                    fontsize=15, fontweight='bold', y=0.995)
        
        plt.tight_layout(rect=[0, 0, 1, 0.96])
        plt.savefig(self.graficos_dir / '02_comparacion_contornos.png')
        plt.close()
        print(f"[OK] 02_comparacion_contornos.png")
    
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
            ax.bar(x_pos + idx*width - width, 
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
        
        print("\n" + "="*80)
        print("TODAS LAS VISUALIZACIONES GENERADAS CORRECTAMENTE")
        print(f"Ubicación: {self.graficos_dir}")
        print("="*80)

if __name__ == '__main__':
    # Buscar el directorio resultados relativo a este script
    script_dir = Path(__file__).parent
    resultados_dir = script_dir.parent / 'resultados'
    
    if not resultados_dir.exists():
        resultados_dir = script_dir.parent.parent / 'resultados'
        
    if not resultados_dir.exists():
        resultados_dir = Path('resultados').resolve()
        
    if not resultados_dir.exists():
        print(f"[ERROR] Directorio no encontrado: {resultados_dir}")
        print("[INFO] Ejecuta primero el programa C++ para generar los CSV")
        sys.exit(1)
    
    vis = VisualizadorRBF(str(resultados_dir))
    vis.generar_todas()
