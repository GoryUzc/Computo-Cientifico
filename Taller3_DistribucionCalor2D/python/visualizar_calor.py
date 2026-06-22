# python/visualizar_calor.py
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path
import sys

class VisualizadorPoisson:

    def __init__(self, resultados_dir):
        self.resultados_dir = Path(resultados_dir)
        self._ensure_graficos_dir()
        self._cargar_datos()

    def _ensure_graficos_dir(self):
        self.graficos_dir = self.resultados_dir / 'graficos'
        self.graficos_dir.mkdir(parents=True, exist_ok=True)

    def _cargar_datos(self):
        self.df_temp = self._cargar_csv('temperatura.csv')
        self.df_temp_completa = self._cargar_csv('temperatura_completa.csv')
        self.df_temp_sin_fuente = self._cargar_csv('temperatura_sin_fuente.csv')
        self.df_matriz = self._cargar_csv('matriz_A.csv')

        self.n = int(np.sqrt(len(self.df_temp)))
        self.x_unique = np.sort(self.df_temp['x'].unique())
        self.y_unique = np.sort(self.df_temp['y'].unique())

    def _cargar_csv(self, filename):
        filepath = self.resultados_dir / filename
        if not filepath.exists():
            print(f"[ERROR] No se encontró: {filepath}")
            sys.exit(1)
        return pd.read_csv(filepath)

    def _crear_grid(self, df):
        x = df['x'].values
        y = df['y'].values
        z = df['temperatura'].values

        n_total = int(np.sqrt(len(df)))
        X = x.reshape(n_total, n_total)
        Y = y.reshape(n_total, n_total)
        Z = z.reshape(n_total, n_total)

        return X, Y, Z

    def _validar_simetria(self, Z):
        Z_simetrica = np.fliplr(Z)
        error_simetria = np.max(np.abs(Z - Z_simetrica))
        print(f"[VAL] Error de simetría: {error_simetria:.2e}")
        return error_simetria

    def _validar_gradiente_vertical(self, Z):
        promedio_gradiente = np.mean(np.diff(Z, axis=0))
        print(f"[VAL] Promedio gradiente vertical: {promedio_gradiente:.2e}")
        return promedio_gradiente

    def graficar_patron_matriz(self):
        fig, ax = plt.subplots(figsize=(10, 8))

        filas = self.df_matriz['fila'].values
        columnas = self.df_matriz['columna'].values
        valores = self.df_matriz['valor'].values

        scatter = ax.scatter(columnas, filas, c=valores, cmap='RdBu_r',
                           s=10, marker='s', edgecolors='none')

        ax.set_xlabel('Columna', fontsize=12)
        ax.set_ylabel('Fila', fontsize=12)
        ax.set_title(f'Patrón de Bandas de la Matriz A (n={self.n})\n'
                     f'{len(filas)} elementos no nulos de {self.n**2}x{self.n**2}',
                     fontsize=14, fontweight='bold')

        plt.colorbar(scatter, ax=ax, label='Valor')
        ax.set_aspect('equal')
        ax.invert_yaxis()

        plt.tight_layout()
        plt.savefig(self.graficos_dir / '01_patron_matriz_A.png', dpi=300)
        plt.close()
        print("[OK] Patrón de matriz guardado.")

    def graficar_heatmap_isotermas(self):
        fig, ax = plt.subplots(figsize=(10, 8))

        X, Y, Z = self._crear_grid(self.df_temp)

        im = ax.pcolormesh(X, Y, Z, cmap='hot', shading='auto')

        niveles = np.linspace(Z.min(), Z.max(), 15)
        contour = ax.contour(X, Y, Z, levels=niveles, colors='cyan',
                           linewidths=0.8, alpha=0.8)
        ax.clabel(contour, inline=True, fontsize=8, fmt='%.1f')

        ax.set_xlabel('x [m]', fontsize=12)
        ax.set_ylabel('y [m]', fontsize=12)
        ax.set_title('Distribución de Temperatura en Estado Estacionario\n'
                     'con Fuente de Calor Puntual en (0.5, 0.5)',
                     fontsize=14, fontweight='bold')
        ax.set_aspect('equal')

        plt.colorbar(im, ax=ax, label='Temperatura [C]')

        plt.tight_layout()
        plt.savefig(self.graficos_dir / '02_heatmap_isotermas.png', dpi=300)
        plt.close()
        print("[OK] Heatmap con isotermas guardado.")

    def graficar_comparacion_fuentes(self):
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

        fig.suptitle('Análisis Físico: Efecto de la Fuente de Calor Puntual',
                     fontsize=15, fontweight='bold', y=1.02)

        plt.tight_layout()
        plt.savefig(self.graficos_dir / '03_comparacion_fuentes.png', dpi=300,
                   bbox_inches='tight')
        plt.close()
        print("[OK] Comparación guardada.")

        centro_idx = self.n // 2
        T_con = Z1[centro_idx, centro_idx]
        T_sin = Z2[centro_idx, centro_idx]
        print(f"[INFO] Temperatura en centro (0.5, 0.5):")
        print(f"       Con fuente:    {T_con:.4f}C")
        print(f"       Sin fuente:    {T_sin:.4f}C")
        print(f"       Diferencia:    {T_con - T_sin:.4f}C")

    def graficar_perfil_transversal(self):
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
        ax.set_title('Perfil Transversal de Temperatura en y = 0.5\n'
                     'Efecto de la Fuente de Calor Puntual',
                     fontsize=14, fontweight='bold')
        ax.legend(fontsize=11)
        ax.grid(True, alpha=0.3)
        ax.set_xlim(0, 1)

        plt.tight_layout()
        plt.savefig(self.graficos_dir / '04_perfil_transversal.png', dpi=300)
        plt.close()
        print("[OK] Perfil transversal guardado.")

    def generar_todas(self):
        print("=" * 60)
        print("VISUALIZACIONES - TALLER 3: POISSON 2D")
        print("=" * 60)
        print(f"[INFO] Malla: {self.n}x{self.n} nodos internos")
        print(f"[INFO] Total incógnitas: {self.n ** 2}")
        print()

        self.graficar_patron_matriz()
        self.graficar_heatmap_isotermas()
        self.graficar_comparacion_fuentes()
        self.graficar_perfil_transversal()

        print()
        print("=" * 60)
        print("TODAS LAS VISUALIZACIONES GENERADAS CORRECTAMENTE")
        print("=" * 60)


if __name__ == '__main__':
    resultados_dir = Path(__file__).parent.parent / 'resultados'

    if not resultados_dir.exists():
        print(f"[ERROR] Directorio no encontrado: {resultados_dir}")
        print("[INFO] Ejecuta primero el solver C++ para generar los CSV")
        sys.exit(1)

    vis = VisualizadorPoisson(str(resultados_dir))
    vis.generar_todas()