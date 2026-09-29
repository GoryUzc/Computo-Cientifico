# python/visualizar_seir.py
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
import sys

class VisualizadorSEIR:
    """Clase para generar visualizaciones del modelo SEIR."""
    
    def __init__(self, resultados_dir: str):
        self.resultados_dir = Path(resultados_dir)
        self.graficos_dir = self.resultados_dir / 'graficos'
        self.graficos_dir.mkdir(parents=True, exist_ok=True)
        
        # Cargar datos
        self.df_A = self._cargar_csv('escenario_A.csv')
        self.df_B = self._cargar_csv('escenario_B.csv')
        self.df_cons_A = self._cargar_csv('conservacion_A.csv')
        self.df_cons_B = self._cargar_csv('conservacion_B.csv')
        self.df_resumen = self._cargar_csv('resumen.csv')
    
    def _cargar_csv(self, filename: str) -> pd.DataFrame:
        """Carga un archivo CSV desde el directorio de resultados"""
        filepath = self.resultados_dir / filename
        if not filepath.exists():
            print(f"[ERROR] No se encontró: {filepath}")
            sys.exit(1)
        return pd.read_csv(filepath)
    
    # ============================================
    # GRÁFICA 1: Evolución temporal Escenario A
    # ============================================
    def graficar_escenario_A(self):
        """GRÁFICA 1: Evolución S, E, I, R en Escenario A"""
        fig, ax = plt.subplots(figsize=(12, 7))
        
        # Escalar a millones para mejor visualización
        factor = 1e-6
        
        ax.plot(self.df_A['tiempo'], self.df_A['S'] * factor, 
               'b-', linewidth=2.5, label='Susceptibles (S)')
        ax.plot(self.df_A['tiempo'], self.df_A['E'] * factor, 
               'orange', linewidth=2.5, label='Expuestos (E)')
        ax.plot(self.df_A['tiempo'], self.df_A['I'] * factor, 
               'r-', linewidth=2.5, label='Infectados (I)')
        ax.plot(self.df_A['tiempo'], self.df_A['R'] * factor, 
               'g-', linewidth=2.5, label='Recuperados (R)')
        
        ax.set_xlabel('Tiempo (días)', fontsize=13)
        ax.set_ylabel('Población (millones)', fontsize=13)
        ax.set_title('Escenario A: Epidemia Libre (Sin Intervención)\n'
                    'Evolución Temporal del Modelo SEIR\n'
                    r'$\beta = 0.6$, $R_0 = 6.0$',
                    fontsize=14, fontweight='bold')
        ax.legend(fontsize=11, loc='center right')
        ax.grid(True, alpha=0.3)
        ax.set_xlim(0, 200)
        
        # Anotar pico de infectados
        pico_idx = self.df_A['I'].idxmax()
        pico_dia = self.df_A.loc[pico_idx, 'tiempo']
        pico_valor = self.df_A.loc[pico_idx, 'I'] * factor
        ax.annotate(f'Pico: {pico_valor:.2f}M\nDía {pico_dia:.1f}',
                   xy=(pico_dia, pico_valor),
                   xytext=(pico_dia + 20, pico_valor + 2),
                   arrowprops=dict(arrowstyle='->', color='red', lw=2),
                   fontsize=10, color='red', fontweight='bold')
        
        plt.tight_layout()
        plt.savefig(self.graficos_dir / '01_escenario_A.png', dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Gráfica 1 guardada: 01_escenario_A.png")
    
    # ============================================
    # GRÁFICA 2: Evolución temporal Escenario B
    # ============================================
    def graficar_escenario_B(self):
        """GRÁFICA 2: Evolución S, E, I, R en Escenario B"""
        fig, ax = plt.subplots(figsize=(12, 7))
        
        factor = 1e-6
        
        ax.plot(self.df_B['tiempo'], self.df_B['S'] * factor, 
               'b-', linewidth=2.5, label='Susceptibles (S)')
        ax.plot(self.df_B['tiempo'], self.df_B['E'] * factor, 
               'orange', linewidth=2.5, label='Expuestos (E)')
        ax.plot(self.df_B['tiempo'], self.df_B['I'] * factor, 
               'r-', linewidth=2.5, label='Infectados (I)')
        ax.plot(self.df_B['tiempo'], self.df_B['R'] * factor, 
               'g-', linewidth=2.5, label='Recuperados (R)')
        
        # Línea vertical en t=30 (inicio de intervención)
        ax.axvline(x=30, color='purple', linestyle='--', linewidth=2.5, 
                   label='Inicio de intervención (t=30)\nβ: 0.6 → 0.25')
        
        ax.set_xlabel('Tiempo (días)', fontsize=13)
        ax.set_ylabel('Población (millones)', fontsize=13)
        ax.set_title('Escenario B: Con Intervención (Confinamiento)\n'
                    'Evolución Temporal del Modelo SEIR\n'
                    r'$R_0$: 6.0 → 2.5',
                    fontsize=14, fontweight='bold')
        ax.legend(fontsize=11, loc='center right')
        ax.grid(True, alpha=0.3)
        ax.set_xlim(0, 200)
        
        # Anotar pico de infectados
        pico_idx = self.df_B['I'].idxmax()
        pico_dia = self.df_B.loc[pico_idx, 'tiempo']
        pico_valor = self.df_B.loc[pico_idx, 'I'] * factor
        ax.annotate(f'Pico: {pico_valor:.2f}M\nDía {pico_dia:.1f}',
                   xy=(pico_dia, pico_valor),
                   xytext=(pico_dia + 20, pico_valor + 2),
                   arrowprops=dict(arrowstyle='->', color='red', lw=2),
                   fontsize=10, color='red', fontweight='bold')
        
        plt.tight_layout()
        plt.savefig(self.graficos_dir / '02_escenario_B.png', dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Gráfica 2 guardada: 02_escenario_B.png")
    
    # ============================================
    # GRÁFICA 3: Comparación de infectados
    # ============================================
    def graficar_comparacion_infectados(self):
        """GRÁFICA 3: Comparación de I(t) entre escenarios"""
        fig, ax = plt.subplots(figsize=(12, 7))
        
        factor = 1e-6
        
        ax.plot(self.df_A['tiempo'], self.df_A['I'] * factor, 
               'r-', linewidth=2.5, label='Escenario A (Sin intervención)')
        ax.plot(self.df_B['tiempo'], self.df_B['I'] * factor, 
               'b-', linewidth=2.5, label='Escenario B (Con intervención)')
        
        # Línea vertical en t=30
        ax.axvline(x=30, color='purple', linestyle='--', linewidth=2, 
                   label='Inicio de intervención')
        
        ax.set_xlabel('Tiempo (días)', fontsize=13)
        ax.set_ylabel('Infectados (millones)', fontsize=13)
        ax.set_title('Comparación de Infectados I(t)\n'
                    'Efecto del Confinamiento en el Pico Epidémico',
                    fontsize=14, fontweight='bold')
        ax.legend(fontsize=11)
        ax.grid(True, alpha=0.3)
        ax.set_xlim(0, 200)
        
        # Calcular métricas
        pico_A = self.df_A['I'].max() * factor
        pico_B = self.df_B['I'].max() * factor
        reduccion = (1 - pico_B / pico_A) * 100
        
        dia_pico_A = self.df_A.loc[self.df_A['I'].idxmax(), 'tiempo']
        dia_pico_B = self.df_B.loc[self.df_B['I'].idxmax(), 'tiempo']
        retraso = dia_pico_B - dia_pico_A
        
        # Anotar reducción del pico
        ax.text(0.02, 0.98, 
               f'Reducción del pico: {reduccion:.1f}%\n'
               f'Retraso del pico: {retraso:.1f} días',
               transform=ax.transAxes,
               fontsize=12,
               verticalalignment='top',
               bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        
        plt.tight_layout()
        plt.savefig(self.graficos_dir / '03_comparacion_infectados.png', dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Gráfica 3 guardada: 03_comparacion_infectados.png")
    
    # ============================================
    # GRÁFICA 4: Verificación de conservación
    # ============================================
    def graficar_conservacion(self):
        """GRÁFICA 4: Verificación de conservación N(t) = S+E+I+R"""
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))
        
        # Panel izquierdo: Escenario A
        ax1.plot(self.df_cons_A['tiempo'], self.df_cons_A['error_absoluto'],
                'b-', linewidth=1.5)
        ax1.set_xlabel('Tiempo (días)', fontsize=12)
        ax1.set_ylabel('Error de conservación |N(t) - N₀|', fontsize=12)
        ax1.set_title('Escenario A: Error de Conservación\n'
                     'Epidemia Libre',
                     fontsize=13, fontweight='bold')
        ax1.grid(True, alpha=0.3)
        ax1.set_yscale('log')
        
        # Anotar error máximo
        error_max_A = self.df_cons_A['error_absoluto'].max()
        ax1.text(0.98, 0.98, f'Error máx: {error_max_A:.2e}',
                transform=ax1.transAxes,
                fontsize=10,
                horizontalalignment='right',
                verticalalignment='top',
                bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.8))
        
        # Panel derecho: Escenario B
        ax2.plot(self.df_cons_B['tiempo'], self.df_cons_B['error_absoluto'],
                'r-', linewidth=1.5)
        ax2.set_xlabel('Tiempo (días)', fontsize=12)
        ax2.set_ylabel('Error de conservación |N(t) - N₀|', fontsize=12)
        ax2.set_title('Escenario B: Error de Conservación\n'
                     'Con Intervención',
                     fontsize=13, fontweight='bold')
        ax2.grid(True, alpha=0.3)
        ax2.set_yscale('log')
        
        # Anotar error máximo
        error_max_B = self.df_cons_B['error_absoluto'].max()
        ax2.text(0.98, 0.98, f'Error máx: {error_max_B:.2e}',
                transform=ax2.transAxes,
                fontsize=10,
                horizontalalignment='right',
                verticalalignment='top',
                bbox=dict(boxstyle='round', facecolor='lightcoral', alpha=0.8))
        
        fig.suptitle('Verificación de Conservación de la Población Total\n'
                    'Error introducido por el método RK4 (escala logarítmica)',
                    fontsize=14, fontweight='bold', y=1.02)
        
        plt.tight_layout()
        plt.savefig(self.graficos_dir / '04_conservacion.png', dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Gráfica 4 guardada: 04_conservacion.png")
    
    # ============================================
    # GRÁFICA 5: Comparación de todos los compartimentos
    # ============================================
    def graficar_comparacion_completa(self):
        """GRÁFICA 5: Comparación S, E, I, R entre escenarios"""
        fig, axes = plt.subplots(2, 2, figsize=(14, 10))
        
        factor = 1e-6
        compartimentos = ['S', 'E', 'I', 'R']
        titulos = ['Susceptibles', 'Expuestos', 'Infectados', 'Recuperados']
        colores_A = ['blue', 'orange', 'red', 'green']
        colores_B = ['darkblue', 'darkorange', 'darkred', 'darkgreen']
        
        for idx, (comp, titulo) in enumerate(zip(compartimentos, titulos)):
            ax = axes[idx // 2, idx % 2]
            
            ax.plot(self.df_A['tiempo'], self.df_A[comp] * factor,
                   color=colores_A[idx], linewidth=2, linestyle='-',
                   label='Escenario A')
            ax.plot(self.df_B['tiempo'], self.df_B[comp] * factor,
                   color=colores_B[idx], linewidth=2, linestyle='--',
                   label='Escenario B')
            
            # Línea vertical en t=30
            ax.axvline(x=30, color='purple', linestyle=':', linewidth=1.5, alpha=0.7)
            
            ax.set_xlabel('Tiempo (días)', fontsize=11)
            ax.set_ylabel(f'{titulo} (millones)', fontsize=11)
            ax.set_title(titulo, fontsize=12, fontweight='bold')
            ax.legend(fontsize=9)
            ax.grid(True, alpha=0.3)
            ax.set_xlim(0, 200)
        
        fig.suptitle('Comparación Completa de Compartimentos\n'
                    'Escenario A (línea continua) vs Escenario B (línea punteada)',
                    fontsize=14, fontweight='bold', y=0.995)
        
        plt.tight_layout()
        plt.savefig(self.graficos_dir / '05_comparacion_completa.png', dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Gráfica 5 guardada: 05_comparacion_completa.png")
    
    # ============================================
    # REPORTE ESTADÍSTICO
    # ============================================
    def generar_reporte_estadistico(self):
        """Genera un reporte estadístico completo en consola"""
        print("\n" + "="*80)
        print("REPORTE ESTADÍSTICO - MODELO EPIDEMIOLÓGICO SEIR")
        print("="*80)
        
        # Escenario A
        print("\n▶ ESCENARIO A (Epidemia Libre, β = 0.6):")
        pico_A = self.df_A['I'].max()
        dia_pico_A = self.df_A.loc[self.df_A['I'].idxmax(), 'tiempo']
        recuperados_A = self.df_A.iloc[-1]['R']
        porcentaje_A = (recuperados_A / 3e7) * 100
        
        print(f"  Pico de infectados: {pico_A/1e6:.2f} millones ({pico_A/3e7*100:.2f}% de la población)")
        print(f"  Día del pico: {dia_pico_A:.1f}")
        print(f"  Recuperados finales: {recuperados_A/1e6:.2f} millones ({porcentaje_A:.2f}% de la población)")
        
        # Escenario B
        print("\n▶ ESCENARIO B (Con Intervención, β: 0.6 → 0.25):")
        pico_B = self.df_B['I'].max()
        dia_pico_B = self.df_B.loc[self.df_B['I'].idxmax(), 'tiempo']
        recuperados_B = self.df_B.iloc[-1]['R']
        porcentaje_B = (recuperados_B / 3e7) * 100
        
        print(f"  Pico de infectados: {pico_B/1e6:.2f} millones ({pico_B/3e7*100:.2f}% de la población)")
        print(f"  Día del pico: {dia_pico_B:.1f}")
        print(f"  Recuperados finales: {recuperados_B/1e6:.2f} millones ({porcentaje_B:.2f}% de la población)")
        
        # Comparación
        print("\n▶ COMPARACIÓN:")
        reduccion_pico = (1 - pico_B / pico_A) * 100
        retraso_pico = dia_pico_B - dia_pico_A
        reduccion_recuperados = (1 - recuperados_B / recuperados_A) * 100
        
        print(f"  Reducción del pico: {reduccion_pico:.1f}%")
        print(f"  Retraso del pico: {retraso_pico:.1f} días")
        print(f"  Reducción de infectados totales: {reduccion_recuperados:.1f}%")
        
        # Conservación
        print("\n▶ VERIFICACIÓN DE CONSERVACIÓN:")
        error_max_A = self.df_cons_A['error_absoluto'].max()
        error_max_B = self.df_cons_B['error_absoluto'].max()
        
        print(f"  Error máximo Escenario A: {error_max_A:.2e}")
        print(f"  Error máximo Escenario B: {error_max_B:.2e}")
        print(f"  Tolerancia: 1e-6")
        print(f"  Estado: {'✓ PASA' if error_max_A < 1e-6 and error_max_B < 1e-6 else '✗ FALLA'}")
        
        print("\n" + "="*80)
    
    # ============================================
    # EJECUCIÓN COMPLETA
    # ============================================
    def generar_todas(self):
        """Ejecuta todas las visualizaciones"""
        print("="*80)
        print("VISUALIZACIONES - MODELO EPIDEMIOLÓGICO SEIR")
        print("="*80)
        
        self.graficar_escenario_A()
        self.graficar_escenario_B()
        self.graficar_comparacion_infectados()
        self.graficar_conservacion()
        self.graficar_comparacion_completa()
        self.generar_reporte_estadistico()
        
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
    
    vis = VisualizadorSEIR(str(resultados_dir))
    vis.generar_todas()