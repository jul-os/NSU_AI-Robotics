"""Визуализации пропусков данных: MCAR/MAR/MNAR и методы обработки.

Генерирует два рисунка:
  missing_mechanisms.png  — данные без пропусков vs MCAR/MAR/MNAR
  missing_methods.png      — данные с пропусками и результаты методов
"""
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
from scipy import stats

OUT = Path(__file__).parent
RNG = np.random.default_rng(42)


def make_complete(n=600, rho=0.7):
    mean = [0.0, 0.0]
    cov = [[1.0, rho], [rho, 1.0]]
    return RNG.multivariate_normal(mean, cov, size=n)


def mcar_mask(y, p=0.30):
    return RNG.random(len(y)) < p


def mar_mask(x, p_base=0.1, slope=0.6):
    # вероятность пропуска y растёт с x
    logit0 = np.log(p_base / (1 - p_base))
    prob = 1.0 / (1.0 + np.exp(-(slope * x + logit0)))
    return RNG.random(len(x)) < prob


def mnar_mask(y, p_base=0.1, slope=0.6):
    logit0 = np.log(p_base / (1 - p_base))
    prob = 1.0 / (1.0 + np.exp(-(slope * y + logit0)))
    return RNG.random(len(y)) < prob


def apply_missing(data, mask):
    out = data.copy()
    out[mask, 1] = np.nan
    return out


# ---------- рисунок 1: механизмы ----------
complete = make_complete()
mcar_data = apply_missing(complete, mcar_mask(complete[:, 1]))
mar_data = apply_missing(complete, mar_mask(complete[:, 0]))
mnar_data = apply_missing(complete, mnar_mask(complete[:, 1]))

fig, axes = plt.subplots(2, 2, figsize=(12, 10))
titles = ["Полные данные (без пропусков)", "MCAR", "MAR", "MNAR"]
datasets = [complete, mcar_data, mar_data, mnar_data]

# для демонстрации механизмов рисуем ВСЕ точки с их истинными y,
# подсвечивая красным те, которые в реальности были бы пропущены
complete_all = [complete,
                apply_missing(complete, mcar_mask(complete[:, 1])),
                apply_missing(complete, mar_mask(complete[:, 0])),
                apply_missing(complete, mnar_mask(complete[:, 1]))]
# истинные маски
true_masks = [np.zeros(len(complete), dtype=bool),
              mcar_mask(complete[:, 1]),
              mar_mask(complete[:, 0]),
              mnar_mask(complete[:, 1])]
for ax, title, mask in zip(axes.ravel(), titles, true_masks):
    obs = ~mask
    ax.scatter(complete[obs, 0], complete[obs, 1], s=12, alpha=0.5, color="steelblue", label="наблюдаемые")
    ax.scatter(complete[mask, 0], complete[mask, 1], s=12, alpha=0.7, color="red", marker="x", label="пропущенные (y)")
    ax.set_title(title)
    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.legend(loc="best", fontsize=8)
    ax.grid(True, alpha=0.3)

fig.suptitle("Механизмы возникновения пропусков", fontsize=14)
fig.tight_layout()
fig.savefig(OUT / "missing_mechanisms.png", dpi=150)
plt.close(fig)
print("saved mechanisms")


# ---------- методы ----------
# берём MCAR-выборку с пропусками в Y
src = make_complete()
mask = mcar_mask(src[:, 1])   # ~30%
miss = apply_missing(src, mask)
x = miss[:, 0]
y = miss[:, 1]
obs_mask = ~np.isnan(y)

# listwise deletion
ld_x, ld_y = x[obs_mask], y[obs_mask]

# pairwise deletion — для пары (X,Y) совпадает с complete-case,
# но показываем те же точки (для многомерных данных отличалось бы)
pw_x, pw_y = x[obs_mask], y[obs_mask]

# simple imputation: среднее по наблюдаемым Y
imp_simple = y.copy()
imp_simple[~obs_mask] = np.nanmean(y)
si_x, si_y = x, imp_simple

# MICE (упрощённо, одна переменная с пропусками): итеративная регрессия
imp_mice = y.copy()
imp_mice[~obs_mask] = np.nanmean(y)
for _ in range(5):
    # регрессия Y на X по наблюдаемым
    slope, intercept, _, _, _ = stats.linregress(x[obs_mask], y[obs_mask])
    # обновляем пропущенные предсказанием + шум остатков
    resid = y[obs_mask] - (intercept + slope * x[obs_mask])
    sigma = np.std(resid, ddof=1)
    imp_mice[~obs_mask] = intercept + slope * x[~obs_mask] + RNG.normal(0, sigma, np.sum(~obs_mask))
mi_x, mi_y = x, imp_mice

# Max likelihood (EM) для бивариантного нормального + условное матожидание
# Оценка параметров по полным наблюдениям (упрощённая MLE)
mu_x = np.mean(x)
mu_y_obs = np.nanmean(y)
# условная регрессия E[Y|X] по наблюдаемым
s, ic, _, _, _ = stats.linregress(x[obs_mask], y[obs_mask])
imp_mle = y.copy()
imp_mle[~obs_mask] = ic + s * x[~obs_mask]
ml_x, ml_y = x, imp_mle

panels = [
    ("Данные с пропусками (наблюдаемые)", x[obs_mask], y[obs_mask], None, None),
    ("Listwise deletion", ld_x, ld_y, None, None),
    ("Pairwise deletion", pw_x, pw_y, None, None),
    ("Simple imputation (среднее)", si_x, si_y, ~obs_mask, imp_simple[~obs_mask]),
    ("MICE", mi_x, mi_y, ~obs_mask, imp_mice[~obs_mask]),
    ("Max likelihood (EM)", ml_x, ml_y, ~obs_mask, imp_mle[~obs_mask]),
]

fig, axes = plt.subplots(2, 3, figsize=(15, 9))
for ax, (title, px, py, imp_flag_mask, imp_vals) in zip(axes.ravel(), panels):
    if imp_flag_mask is None:
        ax.scatter(px, py, s=12, alpha=0.6, color="black")
    else:
        m_obs = ~imp_flag_mask
        ax.scatter(px[m_obs], py[m_obs], s=12, alpha=0.6, color="black", label="наблюдаемые")
        ax.scatter(px[imp_flag_mask], py[imp_flag_mask], s=12, alpha=0.6, color="red", label="импутированные")
        ax.legend(fontsize=8)
    ax.set_title(title)
    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.grid(True, alpha=0.3)

fig.suptitle("Методы обработки пропусков (пропуски в Y)", fontsize=14)
fig.tight_layout()
fig.savefig(OUT / "missing_methods.png", dpi=150)
plt.close(fig)
print("saved methods")
