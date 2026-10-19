import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path

# Colores por semilla según el spec-02.md
COLORS = {
    42: '#1f77b4',      # azul
    123: '#ff7f0e',     # naranja
    456: '#2ca02c',     # verde
    789: '#d62728',     # rojo
    1024: '#9467bd'     # morado
}

ALPHAS = [0.001, 0.01, 0.5]
SEEDS = [42, 123, 456, 789, 1024]
CONVERGENCE_THRESHOLD = 0.1


def load_results(results_dir: str = "resultados") -> list:
    """Carga historiales de pérdida y predicciones exportados por el programa C++."""
    results_path = Path(results_dir)
    results = []

    for seed in SEEDS:
        for alpha in ALPHAS:
            alpha_file_value = int(round(alpha * 1000))
            hist_file = results_path / f"entrenamiento_alfa_{alpha_file_value}_seed_{seed}.csv"
            if not hist_file.exists():
                continue

            predictions_file = results_path / f"predicciones_alfa_{alpha_file_value}_seed_{seed}.csv"
            if not predictions_file.exists():
                raise FileNotFoundError(f"No se encontró el archivo de predicciones: {predictions_file}")

            history = pd.read_csv(hist_file)
            predictions = pd.read_csv(predictions_file)
            results.append({
                'alpha': alpha,
                'seed': seed,
                'epochs': history['epoch'],
                'loss_values': history['loss'],
                'predictions': predictions
            })

    if not results:
        raise FileNotFoundError(
            f"No se encontraron historiales de entrenamiento en '{results_path}'. "
            "Ejecuta primero el programa C++ para generar los CSV."
        )

    return results


def plot_convergence_histograms(results: list, results_dir: str = "resultados"):
    """Genera una gráfica de convergencia por cada α."""
    graphics_dir = Path(results_dir) / "graficos"
    graphics_dir.mkdir(parents=True, exist_ok=True)

    for alpha in ALPHAS:
        fig, ax = plt.subplots(figsize=(12, 6))

        for result in results:
            if result['alpha'] == alpha:
                ax.plot(
                    result['epochs'],
                    result['loss_values'],
                    color=COLORS.get(result['seed'], '#888888'),
                    label=f"Seed {result['seed']}",
                    linewidth=1.5,
                    alpha=0.8
                )

        ax.set_xlabel('Época')
        ax.set_ylabel('Pérdida (Loss)')
        ax.set_title(f'Convergencia - α = {alpha}')
        ax.legend(loc='upper right', frameon=True)
        ax.grid(True, alpha=0.3)
        fig.tight_layout()
        fig.savefig(graphics_dir / f"convergencia_alfa_{alpha}.png", dpi=300)
        plt.close(fig)

    print(f"✅ Generadas gráficas de convergencia en {graphics_dir}")


def plot_fit_curves(results: list, results_dir: str = "resultados"):
    """Genera gráfica de ajuste (α = 0.01, todas las semillas)."""
    fig, ax = plt.subplots(figsize=(12, 6))
    graphics_dir = Path(results_dir) / "graficos"
    graphics_dir.mkdir(parents=True, exist_ok=True)

    final_predictions = [r for r in results if r['alpha'] == 0.01]
    if not final_predictions:
        plt.close(fig)
        print("⚠️ No se encontraron predicciones para α = 0.01")
        return

    for result in final_predictions:
        predictions = result['predictions']
        ax.scatter(
            predictions['x'],
            predictions['y_pred'],
            color=COLORS.get(result['seed'], '#888888'),
            s=40,
            alpha=0.7,
            label=f"Seed {result['seed']}"
        )

    real_x = np.linspace(-2, 2, 500)
    real_y = real_x ** 2 + np.sin(5 * real_x)
    ax.plot(real_x, real_y, 'k-', linewidth=2, label=r'Función real $f(x) = x^2 + \sin(5x)$')

    ax.set_xlabel('x')
    ax.set_ylabel('y')
    ax.set_title('Ajuste de la red neuronal (α = 0.01)')
    ax.legend()
    ax.grid(True, alpha=0.3)

    fig.tight_layout()
    fig.savefig(graphics_dir / "ajuste_alfa_0.01_todas_semillas.png", dpi=300)
    plt.close(fig)
    print("✅ Generada gráfica de ajuste")


def plot_summary_table(results: list, results_dir: str = "resultados"):
    """Genera tabla resumen con estadísticas de los mínimos locales."""
    summary = []
    for result in results:
        below_threshold = result['epochs'][result['loss_values'] < CONVERGENCE_THRESHOLD]
        converged = not below_threshold.empty
        summary.append({
            'alpha': result['alpha'],
            'seed': result['seed'],
            'loss_final': result['loss_values'].iloc[-1],
            'converges': converged,
            'epoch_converged': int(below_threshold.iloc[0]) if converged else -1
        })

    df_sorted = pd.DataFrame(summary).sort_values('loss_final')

    summary_path = Path(results_dir) / "resumen_comparativo.csv"
    df_sorted.to_csv(summary_path, index=False)
    print(f"✅ Generada tabla resumen: {summary_path}")

    print("\n📊 Resumen de mínimos locales:")
    print(df_sorted.to_string(index=False))


def main():
    """Función principal."""
    results_dir = Path("resultados")
    print("🔄 Cargando resultados de entrenamiento...")
    results = load_results(results_dir)
    print(f"✅ Cargados {len(results)} experimentos")

    print("\n📈 Generando gráficas de convergencia...")
    plot_convergence_histograms(results, results_dir)

    print("\n🎯 Generando gráfica de ajuste (α = 0.01)...")
    plot_fit_curves(results, results_dir)

    print("\n📋 Generando tabla resumen...")
    plot_summary_table(results, results_dir)

    print("\n✨ Todo terminado!")


if __name__ == "__main__":
    main()
