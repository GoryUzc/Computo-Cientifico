#!/usr/bin/env python3
"""Visualización de resultados PCA para análisis de ancestria genética."""

import os
import pandas as pd
import matplotlib.pyplot as plt
import argparse


def leer_pca(ruta: str) -> pd.DataFrame:
    """Lee archivo CSV con coordenadas PCA."""
    return pd.read_csv(ruta)


def leer_varianza(ruta: str) -> pd.DataFrame:
    """Lee archivo CSV con porcentajes de varianza explicada."""
    return pd.read_csv(ruta)


def generar_scatter_plot(pca_data: pd.DataFrame, varianza_data: pd.DataFrame, titulo: str, ruta_salida: str):
    """Genera scatter plot PC1 vs PC2 con estilo académico."""
    # Configurar backend para entornos sin display
    plt.switch_backend('Agg')
    
    # Extraer varianzas
    var_pc1 = varianza_data.loc[varianza_data['Componente'] == 'PC1', 'VarianzaExplicada'].values[0]
    var_pc2 = varianza_data.loc[varianza_data['Componente'] == 'PC2', 'VarianzaExplicada'].values[0]
    
    # Crear figura
    fig, ax = plt.subplots(figsize=(10, 8), dpi=150)
    
    # Scatter plot con estilo
    scatter = ax.scatter(
        pca_data['PC1'], 
        pca_data['PC2'],
        c=pca_data['PC1'],
        cmap='viridis',
        s=80,
        edgecolors='black',
        alpha=0.7
    )
    
    # Etiquetas con varianza
    ax.set_xlabel(f'PC1 ({var_pc1:.2f}% varianza)', fontsize=12)
    ax.set_ylabel(f'PC2 ({var_pc2:.2f}% varianza)', fontsize=12)
    ax.set_title(titulo, fontsize=14, fontweight='bold', loc='center')
    
    # Grid y líneas de referencia
    ax.grid(True, linestyle='--', alpha=0.3)
    ax.axhline(y=0, color='gray', linestyle=':', alpha=0.5)
    ax.axvline(x=0, color='gray', linestyle=':', alpha=0.5)
    
    # Numerar individuos
    for i, row in pca_data.iterrows():
        ax.annotate(
            str(i + 1),
            (row['PC1'], row['PC2']),
            xytext=(3, 3),
            textcoords='offset points',
            fontsize=8
        )
    
    # Guardar
    plt.tight_layout()
    plt.savefig(ruta_salida, dpi=150, facecolor='white')
    plt.close()


def main():
    parser = argparse.ArgumentParser(description='Visualización de resultados PCA')
    parser.add_argument('--input', type=str, default='resultados/resultado_pca.csv',
                        help='Ruta al archivo PCA')
    parser.add_argument('--varianza', type=str, default='resultados/varianza.csv',
                        help='Ruta al archivo de varianza')
    parser.add_argument('--output', type=str, default='resultados/grafico_pca.png',
                        help='Ruta de salida del gráfico')
    parser.add_argument('--titulo', type=str, 
                        default='Análisis de Componentes Principales - Estructura Poblacional',
                        help='Título del gráfico')
    args = parser.parse_args()
    
    # Crear directorio de salida si no existe
    os.makedirs(os.path.dirname(args.output), exist_ok=True)
    
    try:
        pca_data = leer_pca(args.input)
        varianza_data = leer_varianza(args.varianza)
        generar_scatter_plot(pca_data, varianza_data, args.titulo, args.output)
        print(f'Gráfico generado exitosamente en {args.output}')
    except FileNotFoundError as e:
        print(f'Error: Archivo no encontrado - {e}')
    except Exception as e:
        print(f'Error inesperado: {str(e)}')


if __name__ == '__main__':
    main()