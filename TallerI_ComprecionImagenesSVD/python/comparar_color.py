"""
comparar_color.py  (bonus: imagen a color)
Aplica SVD a cada canal RGB de forma independiente, genera las imágenes
comprimidas y una comparación visual en panel 2×2.

Uso:
  python comparar_color.py [ruta_imagen] [k]
  python comparar_color.py assets/mi_foto.jpg 30
"""
import os, sys
import csv
import numpy as np
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from PIL import Image

ROOT       = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT_DIR    = os.path.join(ROOT, "resultados", "graficos")
RECON_DIR  = os.path.join(ROOT, "resultados", "reconstrucciones")
DATOS_DIR  = os.path.join(ROOT, "resultados", "datos")
os.makedirs(OUT_DIR, exist_ok=True)
os.makedirs(RECON_DIR, exist_ok=True)

# ── Leer CSV de métricas ────────────────────────────────────────
id_imagen = sys.argv[1] if len(sys.argv) > 1 else "1"
ruta_csv = os.path.join(DATOS_DIR, f"metricas_{id_imagen}.csv")
def leer_metricas(ruta):
    ks, energia, tasa = [], [], []
    with open(ruta, newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            ks.append(int(row["k"]))
            energia.append(float(row["energia_pct"]))
            tasa.append(float(row["tasa_compresion"]))
    return ks, energia, tasa

ks, energia, tasa = leer_metricas(ruta_csv)
ks = ks[::-1]
energia = energia[::-1]
tasa    = tasa[::-1]

fig, axes = plt.subplots(1, 3, figsize=(16, 5))
fig.suptitle("Análisis cuantitativo — Modo color")

axes[0].plot(ks, energia, "s-", color="#e67e22", lw=2, ms=7)
axes[0].set_xlabel("k")
axes[0].set_ylabel("Energía capturada (%)")
axes[0].set_title("Energía vs k")
axes[0].grid(True, ls="--", alpha=0.5)
axes[0].yaxis.set_major_formatter(plt.FuncFormatter(lambda y, _: f"{y:.1f}%"))

axes[1].plot(ks, tasa, "^-", color="#8e44ad", lw=2, ms=7)
axes[1].set_xlabel("k")
axes[1].set_ylabel("Tasa de compresión (×)")
axes[1].set_title("Tasa de compresión vs k")
axes[1].grid(True, ls="--", alpha=0.5)

# Labels explícitos sobre puntos
for i, (xk, yk) in enumerate(zip(ks, tasa)):
    axes[1].annotate(f"{yk:.1f}x", (xk, yk), textcoords="offset points",
                     xytext=(6, 4), fontsize=8)

axes[2].bar([str(k) for k in ks], energia, color="#f39c12", alpha=0.8)
axes[2].set_xlabel("k")
axes[2].set_ylabel("Energía (%)")
axes[2].set_title("Energía por nivel de k")
axes[2].grid(True, ls="--", alpha=0.5, axis="y")

fig.tight_layout()
fig.savefig(os.path.join(OUT_DIR, "color_panel_cuantitativo.png"), dpi=150)
plt.close(fig)

print("Gráfico color guardado.")
