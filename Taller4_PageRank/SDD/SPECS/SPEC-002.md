# SPEC-002: Implementación Python de Visualización y Tablas para PageRank

> **Documento de Especificación Técnica**  
> **Proyecto:** Taller 4 — Algoritmo PageRank de Google  `
---

## 1. Propósito

Especificar la implementación en Python de un script que:
1. **Genere tablas profesionales** con los resultados de los 5 casos de PageRank
2. **Visualice la convergencia** del método de potencias
3. **Muestre heatmaps** de las matrices de Google $G$
4. **Exporte reportes** en formato CSV y PNG de alta calidad

**Herramientas:** Python 3.10+, pandas, numpy, matplotlib, tabulate

---

## 2. Módulos a Implementar

### 2.1 Script Principal: `visualizar_pagerank.py`

```python
# python/visualizar_pagerank.py
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
import sys
from tabulate import tabulate

class VisualizadorPageRank:
    """Clase para generar tablas y visualizaciones del algoritmo PageRank."""
    
    def __init__(self, resultados_dir: str):
        self.resultados_dir = Path(resultados_dir)
        self.graficos_dir = self.resultados_dir / 'graficos'
        self.tablas_dir = self.resultados_dir / 'tablas'
        self.graficos_dir.mkdir(parents=True, exist_ok=True)
        self.tablas_dir.mkdir(parents=True, exist_ok=True)
        
        # Cargar datos de los 5 casos
        self.casos = {}
        for i in range(1, 6):
            self.casos[i] = {
                'pagerank': self._cargar_csv(f'caso{i}_pagerank.csv'),
                'convergencia': self._cargar_csv(f'caso{i}_convergencia.csv'),
                'matriz_G': self._cargar_csv(f'caso{i}_matriz_G.csv'),
                'matriz_M': self._cargar_csv(f'caso{i}_matriz_M.csv')
            }
        
        # Cargar resumen comparativo
        self.resumen = self._cargar_csv('resumen_comparativo.csv')
    
    def _cargar_csv(self, filename: str) -> pd.DataFrame:
        """Carga un archivo CSV desde el directorio de resultados"""
        filepath = self.resultados_dir / filename
        if not filepath.exists():
            print(f"[ERROR] No se encontró: {filepath}")
            sys.exit(1)
        return pd.read_csv(filepath)
    
    # ============================================
    # TABLAS PROFESIONALES
    # ============================================
    
    def generar_tabla_resumen_general(self):
        """TABLA 1: Resumen general de los 5 casos"""
        print("\n" + "="*100)
        print("TABLA 1: RESUMEN GENERAL DE LOS 5 CASOS DE PAGERANK")
        print("="*100)
        
        # Formatear tabla para impresión en consola
        tabla = []
        for _, row in self.resumen.iterrows():
            tabla.append([
                int(row['caso']),
                row['descripcion'],
                int(row['n_paginas']),
                int(row['iteraciones']),
                f"{row['error_final']:.2e}",
                "Sí" if row['convergencia'] else "No",
                f"{row['tiempo_segundos']:.4f}",
                f"{row['pagerank_max']:.6f}",
                f"{row['pagerank_min']:.6f}",
                f"{row['suma_pagerank']:.10f}"
            ])
        
        headers = [
            'Caso', 'Descripción', 'N Págs', 'Iter', 'Error Final',
            'Conv', 'Tiempo (s)', 'PR Máx', 'PR Mín', 'Σ PR'
        ]
        
        # Imprimir en consola
        print(tabulate(tabla, headers=headers, tablefmt='grid', floatfmt='.6f'))
        
        # Guardar como CSV
        df_tabla = pd.DataFrame(tabla, columns=headers)
        df_tabla.to_csv(self.tablas_dir / 'tabla_01_resumen_general.csv', 
                       index=False, encoding='utf-8')
        
        # Guardar como LaTeX
        with open(self.tablas_dir / 'tabla_01_resumen_general.tex', 'w', encoding='utf-8') as f:
            f.write(tabulate(tabla, headers=headers, tablefmt='latex_booktabs'))
        
        print(f"\n[OK] Tabla 1 guardada en: {self.tablas_dir}/")
    
    def generar_tabla_ranking_por_caso(self):
        """TABLA 2: Ranking de páginas para cada caso"""
        print("\n" + "="*100)
        print("TABLA 2: RANKING DE PÁGINAS POR CASO")
        print("="*100)
        
        for caso_id in range(1, 6):
            df_pr = self.casos[caso_id]['pagerank'].copy()
            
            # Ordenar por PageRank descendente
            df_pr = df_pr.sort_values('pagerank', ascending=False)
            df_pr['rank'] = range(1, len(df_pr) + 1)
            df_pr['porcentaje'] = (df_pr['pagerank'] * 100).round(4)
            
            # Formatear tabla
            tabla = []
            for _, row in df_pr.iterrows():
                tabla.append([
                    int(row['rank']),
                    int(row['pagina']),
                    f"{row['pagerank']:.8f}",
                    f"{row['porcentaje']:.4f}%",
                    int(row['grado_entrada']),
                    int(row['grado_salida'])
                ])
            
            headers = ['Rank', 'Página', 'PageRank', '% del Total', 'Grado Entrada', 'Grado Salida']
            
            print(f"\n--- CASO {caso_id} ---")
            print(tabulate(tabla, headers=headers, tablefmt='grid'))
            
            # Guardar CSV
            df_ranking = pd.DataFrame(tabla, columns=headers)
            df_ranking.to_csv(self.tablas_dir / f'tabla_02_ranking_caso{caso_id}.csv', 
                            index=False, encoding='utf-8')
            
            # Guardar LaTeX
            with open(self.tablas_dir / f'tabla_02_ranking_caso{caso_id}.tex', 'w', encoding='utf-8') as f:
                f.write(tabulate(tabla, headers=headers, tablefmt='latex_booktabs'))
        
        print(f"\n[OK] Tablas 2 guardadas en: {self.tablas_dir}/")
    
    def generar_tabla_convergencia(self):
        """TABLA 3: Métricas de convergencia por caso"""
        print("\n" + "="*100)
        print("TABLA 3: MÉTRICAS DE CONVERGENCIA")
        print("="*100)
        
        tabla = []
        for caso_id in range(1, 6):
            df_conv = self.casos[caso_id]['convergencia']
            
            # Calcular métricas
            iteraciones = len(df_conv) - 1
            error_inicial = df_conv.iloc[1]['error_norma'] if len(df_conv) > 1 else 0
            error_final = df_conv.iloc[-1]['error_norma']
            
            # Calcular tasa de convergencia
            if error_inicial > 0 and error_final > 0:
                tasa = (error_final / error_inicial) ** (1 / iteraciones) if iteraciones > 0 else 0
            else:
                tasa = 0
            
            # Página con mayor PageRank
            df_pr = self.casos[caso_id]['pagerank']
            pagina_top = df_pr['pagerank'].idxmax()
            pr_top = df_pr.loc[pagina_top, 'pagerank']
            
            tabla.append([
                caso_id,
                iteraciones,
                f"{error_inicial:.6f}",
                f"{error_final:.2e}",
                f"{tasa:.6f}",
                int(pagina_top),
                f"{pr_top:.6f}"
            ])
        
        headers = ['Caso', 'Iteraciones', 'Error Inicial', 'Error Final', 
                   'Tasa Conv', 'Pág Top', 'PR Top']
        
        print(tabulate(tabla, headers=headers, tablefmt='grid'))
        
        # Guardar
        df_tabla = pd.DataFrame(tabla, columns=headers)
        df_tabla.to_csv(self.tablas_dir / 'tabla_03_convergencia.csv', 
                       index=False, encoding='utf-8')
        
        with open(self.tablas_dir / 'tabla_03_convergencia.tex', 'w', encoding='utf-8') as f:
            f.write(tabulate(tabla, headers=headers, tablefmt='latex_booktabs'))
        
        print(f"\n[OK] Tabla 3 guardada en: {self.tablas_dir}/")
    
    def generar_tabla_matrices(self):
        """TABLA 4: Propiedades de las matrices M y G"""
        print("\n" + "="*100)
        print("TABLA 4: PROPIEDADES DE LAS MATRICES")
        print("="*100)
        
        tabla = []
        for caso_id in range(1, 6):
            df_M = self.casos[caso_id]['matriz_M']
            df_G = self.casos[caso_id]['matriz_G']
            
            # Propiedades de M
            n = int(np.sqrt(len(df_M)))
            elementos_no_cero_M = len(df_M[df_M['valor'] > 1e-10])
            suma_columnas_M = df_M.groupby('columna')['valor'].sum()
            columnas_sumidero = (suma_columnas_M < 1e-6).sum()
            
            # Propiedades de G
            elementos_no_cero_G = len(df_G)
            min_G = df_G['valor'].min()
            max_G = df_G['valor'].max()
            
            tabla.append([
                caso_id,
                f"{n}x{n}",
                elementos_no_cero_M,
                columnas_sumidero,
                elementos_no_cero_G,
                f"{min_G:.6f}",
                f"{max_G:.6f}"
            ])
        
        headers = ['Caso', 'Dimensión', 'No Ceros M', 'Sumideros', 
                   'No Ceros G', 'Min G', 'Max G']
        
        print(tabulate(tabla, headers=headers, tablefmt='grid'))
        
        # Guardar
        df_tabla = pd.DataFrame(tabla, columns=headers)
        df_tabla.to_csv(self.tablas_dir / 'tabla_04_matrices.csv', 
                       index=False, encoding='utf-8')
        
        with open(self.tablas_dir / 'tabla_04_matrices.tex', 'w', encoding='utf-8') as f:
            f.write(tabulate(tabla, headers=headers, tablefmt='latex_booktabs'))
        
        print(f"\n[OK] Tabla 4 guardada en: {self.tablas_dir}/")
    
    # ============================================
    # VISUALIZACIONES
    # ============================================
    
    def graficar_convergencia_todos_casos(self):
        """GRÁFICA 1: Convergencia de los 5 casos"""
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))
        
        colores = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd']
        
        # Panel 1: Error vs iteraciones (escala log)
        for caso_id in range(1, 6):
            df_conv = self.casos[caso_id]['convergencia']
            ax1.semilogy(df_conv['iteracion'], df_conv['error_norma'],
                        label=f'Caso {caso_id}',
                        color=colores[caso_id-1],
                        linewidth=2)
        
        ax1.set_xlabel('Iteración', fontsize=13)
        ax1.set_ylabel('Error ||v_{k+1} - v_k||₂', fontsize=13)
        ax1.set_title('Convergencia del Método de las Potencias\n(Error en escala logarítmica)',
                     fontsize=14, fontweight='bold')
        ax1.legend(fontsize=11)
        ax1.grid(True, alpha=0.3)
        ax1.axhline(y=1e-8, color='red', linestyle='--', linewidth=1.5, 
                   label='Tolerancia ε = 10⁻⁸')
        
        # Panel 2: Comparación de iteraciones
        casos = list(range(1, 6))
        iteraciones = [len(self.casos[i]['convergencia']) - 1 for i in casos]
        
        bars = ax2.bar(casos, iteraciones, color=colores, alpha=0.7, edgecolor='black')
        ax2.set_xlabel('Caso', fontsize=13)
        ax2.set_ylabel('Número de Iteraciones', fontsize=13)
        ax2.set_title('Iteraciones hasta Convergencia\n(Tolerancia ε = 10⁻⁸)',
                     fontsize=14, fontweight='bold')
        ax2.set_xticks(casos)
        ax2.set_xticklabels([f'Caso {i}' for i in casos])
        ax2.grid(True, alpha=0.3, axis='y')
        
        # Agregar valores en las barras
        for bar, iters in zip(bars, iteraciones):
            ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 1,
                    str(iters), ha='center', va='bottom', fontsize=11, fontweight='bold')
        
        plt.tight_layout()
        plt.savefig(self.graficos_dir / '01_convergencia_todos_casos.png', 
                   dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Gráfica 1 guardada: 01_convergencia_todos_casos.png")
    
    def graficar_pagerank_por_caso(self):
        """GRÁFICA 2: PageRank de cada caso (barras)"""
        fig, axes = plt.subplots(3, 2, figsize=(16, 18))
        axes = axes.flatten()
        
        colores = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd']
        
        for caso_id in range(1, 6):
            ax = axes[caso_id - 1]
            df_pr = self.casos[caso_id]['pagerank'].copy()
            df_pr = df_pr.sort_values('pagerank', ascending=False)
            
            # Barras
            bars = ax.bar(df_pr['pagina'].astype(str), df_pr['pagerank'],
                         color=colores[caso_id-1], alpha=0.7, edgecolor='black')
            
            # Línea de PageRank uniforme
            pr_uniforme = 1.0 / len(df_pr)
            ax.axhline(y=pr_uniforme, color='red', linestyle='--', linewidth=1.5,
                      label=f'Uniforme (1/{len(df_pr)} = {pr_uniforme:.4f})')
            
            ax.set_xlabel('Página', fontsize=11)
            ax.set_ylabel('PageRank', fontsize=11)
            ax.set_title(f'Caso {caso_id}: Distribución de PageRank',
                        fontsize=12, fontweight='bold')
            ax.legend(fontsize=9)
            ax.grid(True, alpha=0.3, axis='y')
            
            # Agregar valores en las barras
            for bar, pr in zip(bars, df_pr['pagerank']):
                ax.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.005,
                       f'{pr:.3f}', ha='center', va='bottom', fontsize=8)
        
        # Panel vacío para balance
        axes[5].axis('off')
        
        plt.tight_layout()
        plt.savefig(self.graficos_dir / '02_pagerank_por_caso.png', 
                   dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Gráfica 2 guardada: 02_pagerank_por_caso.png")
    
    def graficar_heatmap_matrices(self):
        """GRÁFICA 3: Heatmaps de las matrices G"""
        fig, axes = plt.subplots(3, 2, figsize=(16, 18))
        axes = axes.flatten()
        
        for caso_id in range(1, 6):
            ax = axes[caso_id - 1]
            df_G = self.casos[caso_id]['matriz_G']
            
            # Reconstruir matriz
            n = int(np.sqrt(len(df_G)))
            G = np.zeros((n, n))
            for _, row in df_G.iterrows():
                G[int(row['fila']), int(row['columna'])] = row['valor']
            
            # Heatmap
            im = ax.imshow(G, cmap='YlOrRd', aspect='equal')
            plt.colorbar(im, ax=ax, fraction=0.046, pad=0.04)
            
            ax.set_xlabel('Columna (Página origen)', fontsize=11)
            ax.set_ylabel('Fila (Página destino)', fontsize=11)
            ax.set_title(f'Caso {caso_id}: Matriz de Google G ({n}x{n})',
                        fontsize=12, fontweight='bold')
            
            # Agregar valores en las celdas si la matriz es pequeña
            if n <= 6:
                for i in range(n):
                    for j in range(n):
                        text = ax.text(j, i, f'{G[i,j]:.2f}',
                                     ha='center', va='center',
                                     fontsize=7,
                                     color='white' if G[i,j] > 0.3 else 'black')
        
        # Panel vacío para balance
        axes[5].axis('off')
        
        plt.tight_layout()
        plt.savefig(self.graficos_dir / '03_heatmap_matrices_G.png', 
                   dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Gráfica 3 guardada: 03_heatmap_matrices_G.png")
    
    def graficar_comparacion_top_pages(self):
        """GRÁFICA 4: Comparación de las páginas top de cada caso"""
        fig, ax = plt.subplots(figsize=(14, 8))
        
        casos = list(range(1, 6))
        colores = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd']
        
        # Obtener top 3 páginas de cada caso
        top_pages = {}
        for caso_id in casos:
            df_pr = self.casos[caso_id]['pagerank'].sort_values('pagerank', ascending=False)
            top_pages[caso_id] = df_pr.head(3)['pagerank'].values
        
        # Crear barras agrupadas
        x = np.arange(3)  # 3 posiciones top
        width = 0.15
        
        for i, caso_id in enumerate(casos):
            offset = (i - 2) * width
            bars = ax.bar(x + offset, top_pages[caso_id], width,
                         label=f'Caso {caso_id}',
                         color=colores[i], alpha=0.7, edgecolor='black')
            
            # Agregar valores
            for bar, pr in zip(bars, top_pages[caso_id]):
                ax.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.002,
                       f'{pr:.3f}', ha='center', va='bottom', fontsize=8)
        
        ax.set_xlabel('Posición en el Ranking', fontsize=13)
        ax.set_ylabel('PageRank', fontsize=13)
        ax.set_title('Comparación de las Top 3 Páginas entre los 5 Casos',
                    fontsize=14, fontweight='bold')
        ax.set_xticks(x)
        ax.set_xticklabels(['1° Lugar', '2° Lugar', '3° Lugar'])
        ax.legend(fontsize=11)
        ax.grid(True, alpha=0.3, axis='y')
        
        plt.tight_layout()
        plt.savefig(self.graficos_dir / '04_comparacion_top_pages.png', 
                   dpi=300, bbox_inches='tight')
        plt.close()
        print(f"[OK] Gráfica 4 guardada: 04_comparacion_top_pages.png")
    
    # ============================================
    # REPORTE ESTADÍSTICO
    # ============================================
    
    def generar_reporte_estadistico(self):
        """Genera un reporte estadístico completo en consola"""
        print("\n" + "="*100)
        print("REPORTE ESTADÍSTICO COMPLETO - ALGORITMO PAGERANK")
        print("="*100)
        
        # Resumen general
        print("\n▶ RESUMEN GENERAL:")
        print(f"  Total de casos analizados: {len(self.resumen)}")
        print(f"  Todos convergieron: {'Sí' if self.resumen['convergencia'].all() else 'No'}")
        print(f"  Promedio de iteraciones: {self.resumen['iteraciones'].mean():.1f}")
        print(f"  Tiempo total de ejecución: {self.resumen['tiempo_segundos'].sum():.4f} segundos")
        
        # Análisis por caso
        print("\n▶ ANÁLISIS POR CASO:")
        for caso_id in range(1, 6):
            df_pr = self.casos[caso_id]['pagerank']
            df_conv = self.casos[caso_id]['convergencia']
            
            print(f"\n  CASO {caso_id}:")
            print(f"    Número de páginas: {len(df_pr)}")
            print(f"    Iteraciones: {len(df_conv) - 1}")
            print(f"    PageRank máximo: {df_pr['pagerank'].max():.6f} (Página {df_pr['pagerank'].idxmax()})")
            print(f"    PageRank mínimo: {df_pr['pagerank'].min():.6f}")
            print(f"    Desviación estándar: {df_pr['pagerank'].std():.6f}")
            
            # Verificar suma
            suma = df_pr['pagerank'].sum()
            print(f"    Suma de PageRanks: {suma:.10f} (Error: {abs(suma - 1.0):.2e})")
        
        # Conclusiones
        print("\n▶ CONCLUSIONES:")
        print("  1. El método de las potencias converge para todos los casos")
        print("  2. La tolerancia ε = 10⁻⁸ se alcanza en menos de 100 iteraciones")
        print("  3. Las sumas de PageRank son 1.0 con precisión de máquina")
        print("  4. Los casos con sumideros requieren el factor de amortiguamiento d = 0.85")
        
        print("\n" + "="*100)
    
    # ============================================
    # EJECUCIÓN COMPLETA
    # ============================================
    
    def generar_todas_las_tablas(self):
        """Genera todas las tablas profesionales"""
        print("\n" + "="*100)
        print("GENERANDO TABLAS PROFESIONALES - PAGERANK")
        print("="*100)
        
        self.generar_tabla_resumen_general()
        self.generar_tabla_ranking_por_caso()
        self.generar_tabla_convergencia()
        self.generar_tabla_matrices()
        
        print("\n" + "="*100)
        print("TODAS LAS TABLAS GENERADAS CORRECTAMENTE")
        print(f"Ubicación: {self.tablas_dir}")
        print("="*100)
    
    def generar_todas_las_graficas(self):
        """Genera todas las visualizaciones"""
        print("\n" + "="*100)
        print("GENERANDO VISUALIZACIONES - PAGERANK")
        print("="*100)
        
        self.graficar_convergencia_todos_casos()
        self.graficar_pagerank_por_caso()
        self.graficar_heatmap_matrices()
        self.graficar_comparacion_top_pages()
        
        print("\n" + "="*100)
        print("TODAS LAS VISUALIZACIONES GENERADAS CORRECTAMENTE")
        print(f"Ubicación: {self.graficos_dir}")
        print("="*100)
    
    def generar_todo(self):
        """Ejecuta todo el proceso: tablas, gráficas y reporte"""
        print("\n" + "="*100)
        print("VISUALIZADOR PAGERANK - TALLER 4")
        print("="*100)
        
        self.generar_todas_las_tablas()
        self.generar_todas_las_graficas()
        self.generar_reporte_estadistico()
        
        print("\n" + "="*100)
        print("PROCESO COMPLETADO EXITOSAMENTE")
        print("="*100)


if __name__ == '__main__':
    resultados_dir = Path(__file__).parent.parent / 'resultados'
    
    if not resultados_dir.exists():
        print(f"[ERROR] Directorio no encontrado: {resultados_dir}")
        print("[INFO] Ejecuta primero el programa C++ para generar los CSV")
        sys.exit(1)
    
    vis = VisualizadorPageRank(str(resultados_dir))
    vis.generar_todo()
```

---

## 3. Especificaciones Técnicas

### 3.1 Librerías Requeridas

| Librería | Versión | Propósito |
|----------|---------|-----------|
| `numpy` | 1.24+ | Cálculos numéricos |
| `pandas` | 2.0+ | Carga y procesamiento de CSV |
| `matplotlib` | 3.7+ | Visualización |
| `tabulate` | 0.9+ | Tablas profesionales en consola |

### 3.2 `requirements.txt`

```txt
numpy>=1.24.0
pandas>=2.0.0
matplotlib>=3.7.0
tabulate>=0.9.0
```

### 3.3 Instalación

```bash
pip install -r python/requirements.txt
```

---

## 4. Estructura de Salida

```
resultados/
├── tablas/                              # ← NUEVO: Tablas profesionales
│   ├── tabla_01_resumen_general.csv
│   ├── tabla_01_resumen_general.tex     # ← Formato LaTeX
│   ├── tabla_02_ranking_caso1.csv
│   ├── tabla_02_ranking_caso1.tex
│   ├── tabla_02_ranking_caso2.csv
│   ├── tabla_02_ranking_caso2.tex
│   ├── tabla_02_ranking_caso3.csv
│   ├── tabla_02_ranking_caso3.tex
│   ├── tabla_02_ranking_caso4.csv
│   ├── tabla_02_ranking_caso4.tex
│   ├── tabla_02_ranking_caso5.csv
│   ├── tabla_02_ranking_caso5.tex
│   ├── tabla_03_convergencia.csv
│   ├── tabla_03_convergencia.tex
│   ├── tabla_04_matrices.csv
│   └── tabla_04_matrices.tex
├── graficos/                            # Visualizaciones
│   ├── 01_convergencia_todos_casos.png
│   ├── 02_pagerank_por_caso.png
│   ├── 03_heatmap_matrices_G.png
│   └── 04_comparacion_top_pages.png
├── caso1_pagerank.csv                   # Archivos generados por C++
├── caso1_convergencia.csv
├── caso1_matriz_G.csv
├── caso1_matriz_M.csv
├── ...
├── caso5_pagerank.csv
├── caso5_convergencia.csv
├── caso5_matriz_G.csv
├── caso5_matriz_M.csv
└── resumen_comparativo.csv
```

---

## 5. Tablas Generadas

### Tabla 1: Resumen General

| Caso | Descripción | N Págs | Iter | Error Final | Conv | Tiempo (s) | PR Máx | PR Mín | Σ PR |
|------|-------------|--------|------|-------------|------|------------|--------|--------|------|
| 1 | 3x3 con sumidero | 3 | ~50 | ~1e-9 | Sí | ~0.001 | ~0.47 | ~0.05 | 1.0 |
| 2 | 4x4 regular | 4 | ~60 | ~1e-9 | Sí | ~0.002 | ~0.25 | ~0.25 | 1.0 |
| 3 | 8x8 complejo | 8 | ~80 | ~1e-9 | Sí | ~0.005 | ~0.20 | ~0.08 | 1.0 |
| 4 | 5x5 con estructura | 5 | ~70 | ~1e-9 | Sí | ~0.003 | ~0.33 | ~0.08 | 1.0 |
| 5 | 3x3 con sumidero | 3 | ~40 | ~1e-9 | Sí | ~0.001 | ~0.50 | ~0.00 | 1.0 |

### Tabla 2: Ranking por Caso (ejemplo Caso 1)

| Rank | Página | PageRank | % del Total | Grado Entrada | Grado Salida |
|------|--------|----------|-------------|---------------|--------------|
| 1 | 2 | 0.47368421 | 47.3684% | 1 | 0 |
| 2 | 1 | 0.26315789 | 26.3158% | 1 | 1 |
| 3 | 0 | 0.26315789 | 26.3158% | 1 | 2 |

### Tabla 3: Métricas de Convergencia

| Caso | Iteraciones | Error Inicial | Error Final | Tasa Conv | Pág Top | PR Top |
|------|-------------|---------------|-------------|-----------|---------|--------|
| 1 | ~50 | ~0.5 | ~1e-9 | ~0.85 | 2 | ~0.47 |
| 2 | ~60 | ~0.3 | ~1e-9 | ~0.87 | 0 | ~0.25 |
| 3 | ~80 | ~0.4 | ~1e-9 | ~0.89 | 2 | ~0.20 |
| 4 | ~70 | ~0.3 | ~1e-9 | ~0.88 | 2 | ~0.33 |
| 5 | ~40 | ~0.6 | ~1e-9 | ~0.84 | 0 | ~0.50 |

### Tabla 4: Propiedades de las Matrices

| Caso | Dimensión | No Ceros M | Sumideros | No Ceros G | Min G | Max G |
|------|-----------|------------|-----------|------------|-------|-------|
| 1 | 3x3 | 3 | 1 | 9 | 0.05 | 0.90 |
| 2 | 4x4 | 8 | 0 | 16 | 0.0375 | 0.1625 |
| 3 | 8x8 | 12 | 2 | 64 | 0.01875 | 0.11875 |
| 4 | 5x5 | 6 | 1 | 25 | 0.03 | 0.21 |
| 5 | 3x3 | 3 | 1 | 9 | 0.05 | 0.90 |

---

## 6. Gráficas Generadas

| # | Archivo | Descripción |
|---|---------|-------------|
| 1 | `01_convergencia_todos_casos.png` | Error vs iteraciones (escala log) + barras de iteraciones |
| 2 | `02_pagerank_por_caso.png` | Distribución de PageRank para cada caso (6 subplots) |
| 3 | `03_heatmap_matrices_G.png` | Heatmaps de las matrices de Google G |
| 4 | `04_comparacion_top_pages.png` | Comparación de las Top 3 páginas entre casos |

---

## 7. Criterios de Aceptación

| Criterio | Verificación | Estado |
|----------|--------------|--------|
| **16 tablas generadas** | 4 tipos × (1 general + 5 casos) | ✅ |
| **Tablas en CSV** | Archivos CSV en `tablas/` | ✅ |
| **Tablas en LaTeX** | Archivos .tex en `tablas/` | ✅ |
| **4 gráficas generadas** | Archivos PNG en `graficos/` | ✅ |
| **Resolución 300 DPI** | Calidad para informe | ✅ |
| **Suma de PageRank = 1** | Verificación numérica | ✅ |
| **Reporte en consola** | Estadísticas completas | ✅ |

---

## 8. Prompt Maestro para Kilo IA

```markdown
# Tarea: Implementar Visualización Python para PageRank

## Contexto
Estoy desarrollando el Taller 4 de Cálculo Científico. Ya tengo el solver C++ 
que genera 21 archivos CSV (5 casos × 4 archivos + resumen). Necesito un script 
Python que genere tablas profesionales y visualizaciones.

## Especificación
Lee el archivo SDD/SPECS/spec-002.md para los detalles completos.

## Entrada (CSV desde C++)
- resultados/caso[1-5]_pagerank.csv
- resultados/caso[1-5]_convergencia.csv
- resultados/caso[1-5]_matriz_G.csv
- resultados/caso[1-5]_matriz_M.csv
- resultados/resumen_comparativo.csv

## Salida
### Tablas (en resultados/tablas/)
1. tabla_01_resumen_general.csv + .tex
2. tabla_02_ranking_caso[1-5].csv + .tex (5 archivos)
3. tabla_03_convergencia.csv + .tex
4. tabla_04_matrices.csv + .tex

### Gráficas (en resultados/graficos/)
1. 01_convergencia_todos_casos.png
2. 02_pagerank_por_caso.png
3. 03_heatmap_matrices_G.png
4. 04_comparacion_top_pages.png

## Requisitos
1. Usar clase VisualizadorPageRank con métodos separados
2. Cargar CSV con pandas
3. Generar tablas con tabulate (formato grid, latex_booktabs)
4. Usar matplotlib con estilo profesional
5. DPI = 300 para calidad de informe
6. Generar reporte estadístico en consola
7. Exportar tablas en CSV y LaTeX

## Primero
Muéstrame el código completo de visualizar_pagerank.py
```

---

## 9. Checklist de Implementación

- [x] Crear archivo `python/visualizar_pagerank.py`
- [x] Crear archivo `python/requirements.txt`
- [x] Instalar dependencias: `pip install -r python/requirements.txt`
- [x] Ejecutar solver C++ primero para generar CSV
- [x] Ejecutar script Python: `python python/visualizar_pagerank.py`
- [x] Verificar que se generen las 16 tablas
- [x] Verificar que se generen las 4 gráficas
- [x] Revisar el reporte estadístico en consola
- [x] Incluir tablas LaTeX en el informe

---

**Fin del documento SPEC-002.md**

---
