"""
analisis_cuantitativo.py
Lee datos/metricas_<id>.csv y datos/valores_singulares_<id>.csv y genera gráficos PNG
en resultados/graficos/:
  - error_vs_k_<id>.png
  - energia_vs_k_<id>.png
  - tasa_compresion_vs_k_<id>.png
  - panel_combinado_<id>.png
"""
import os, sys
import csv
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

# ID de imagen desde argumento (por defecto "1")
id_imagen = sys.argv[1] if len(sys.argv) > 1 else "1"

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATOS_DIR = os.path.join(ROOT, "resultados", "datos")
OUT_DIR   = os.path.join(ROOT, "resultados", "graficos")
os.makedirs(OUT_DIR, exist_ok=True)

def leer_csv(ruta):
    ks, vals = [], []
    with open(ruta, newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            ks.append(int(row[reader.fieldnames[0]]))
            vals.append([float(row[c]) for c in reader.fieldnames[1:]])
    return ks, vals, reader.fieldnames

# ── Leer métricas ──────────────────────────────────────────────
ruta_met = os.path.join(DATOS_DIR, f"metricas_{id_imagen}.csv")
ks, vals, cols = leer_csv(ruta_met)
err   = [v[0] for v in vals]
ener  = [v[1] for v in vals]
tasa  = [v[2] for v in vals]

# ── Gráfico 1: Error relativo vs k ─────────────────────────────
fig1, ax1 = plt.subplots(figsize=(7, 4.5))
ax1.plot(ks, err, "o-", color="#e74c3c", linewidth=2, markersize=7)
ax1.set_xlabel("Rango k")
ax1.set_ylabel("Error relativo (norma Frobenius)")
ax1.set_title("Error de reconstrucción vs k")
ax1.grid(True, linestyle="--", alpha=0.5)
ax1.set_yscale("log")
fig1.tight_layout()
fig1.savefig(os.path.join(OUT_DIR, f"error_vs_k_{id_imagen}.png"), dpi=150)
plt.close(fig1)

# ── Gráfico 2: Energía capturada vs k ──────────────────────────
fig2, ax2 = plt.subplots(figsize=(7, 4.5))
ax2.plot(ks, ener, "s-", color="#27ae60", linewidth=2, markersize=7)
ax2.set_xlabel("Rango k")
ax2.set_ylabel("Energía capturada (%)")
ax2.set_title("Energía acumulada vs k")
ax2.grid(True, linestyle="--", alpha=0.5)
ax2.yaxis.set_major_formatter(ticker.FormatStrFormatter("%.1f%%"))
fig2.tight_layout()
fig2.savefig(os.path.join(OUT_DIR, f"energia_vs_k_{id_imagen}.png"), dpi=150)
plt.close(fig2)

# ── Gráfico 3: Tasa de compresión vs k ─────────────────────────
fig3, ax3 = plt.subplots(figsize=(7, 4.5))
ax3.plot(ks, tasa, "^-", color="#2980b9", linewidth=2, markersize=7)
ax3.invert_xaxis()   # más k → menos compresión (más natural leer izquierda-derecha)
ax3.set_xlabel("Rango k")
ax3.set_ylabel("Tasa de compresión (×)")
ax3.set_title("Tasa de compresión vs k")
ax3.grid(True, linestyle="--", alpha=0.5)
fig3.tight_layout()
fig3.savefig(os.path.join(OUT_DIR, f"tasa_compresion_vs_k_{id_imagen}.png"), dpi=150)
plt.close(fig3)

# ── Gráfico 4: panel combinado ─────────────────────────────────
fig4, axes = plt.subplots(1, 3, figsize=(16, 5))
for ax in axes:
    ax.grid(True, linestyle="--", alpha=0.5)

axes[0].plot(ks, ener, "s-", color="#27ae60", linewidth=2, markersize=7)
axes[0].set_xlabel("k")
axes[0].set_ylabel("Energía (%)")
axes[0].set_title("Energía capturada")
axes[0].yaxis.set_major_formatter(ticker.FormatStrFormatter("%.1f%%"))

axes[1].plot(ks, err, "o-", color="#e74c3c", linewidth=2, markersize=7)
axes[1].set_xlabel("k")
axes[1].set_ylabel("Error relativo")
axes[1].set_title("Error de reconstrucción")
axes[1].set_yscale("log")

axes[2].plot(ks, tasa, "^-", color="#2980b9", linewidth=2, markersize=7)
axes[2].set_xlabel("k")
axes[2].set_ylabel("Tasa de compresión (×)")
axes[2].set_title("Tasa de compresión")
axes[2].invert_xaxis()

fig4.suptitle("Análisis cuantitativo SVD — Compresión de imagen")
fig4.tight_layout()
fig4.savefig(os.path.join(OUT_DIR, f"panel_combinado_{id_imagen}.png"), dpi=150)
plt.close(fig4)

print(f"Gráficos guardados en {OUT_DIR}")