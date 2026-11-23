import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from itertools import combinations
from scipy.stats import gaussian_kde
from matplotlib.colors import ListedColormap
from matplotlib.lines import Line2D
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis, QuadraticDiscriminantAnalysis
from sklearn.naive_bayes import GaussianNB

from plots import scatterplot

# Фиксируем random_state для воспроизводимости результата
np.random.seed(42)

n_samples = 100
n_features = 16

# Матрицы ковариации
# Для первых двух классов задаем большую дисперсию, чтобы их распределения сильно перекрывались
cov_overlap = np.eye(n_features) * 3.0

# Для третьего класса дисперсия стандартная, он будет компактным
cov_separated = np.eye(n_features) * 1.0

# Векторы средних значений (математические ожидания)
# Первые два класса близки друг к другу в пространстве признаков
mean0 = np.zeros(n_features)
mean1 = np.ones(n_features) * 1.0

# Третий класс находится далеко от первых двух
mean2 = np.ones(n_features) * 15.0

# Генерация данных с помощью multivariate_normal
X0 = np.random.multivariate_normal(mean0, cov_overlap, n_samples)
X1 = np.random.multivariate_normal(mean1, cov_overlap, n_samples)
X2 = np.random.multivariate_normal(mean2, cov_separated, n_samples)

# Объединение признаков всех классов в один массив
X = np.vstack((X0, X1, X2))

# Создание меток классов (0, 1, 2)
y = np.concatenate([
    np.zeros(n_samples),
    np.ones(n_samples),
    np.full(n_samples, 2)
])

# Формирование итогового датасета df1
feature_names = [f'feature_{i+1}' for i in range(n_features)]
df1 = pd.DataFrame(X, columns=feature_names)
df1['target'] = y.astype(int)

# Проверка формы и первых строк
print(f"Размер датасета: {df1.shape}")
print(df1.head())


""""
Вывести в табличной форме статистику по датасету, включая
- Размерность всего датасета (объекты и признаки) - Тип каждого признака
- Количество классов и объектов в каждом из классов - Иные оценки выборки
- Число объектов с NaN по классам - Число объектов с 0 по классам

Используя коэффициенты корреляции Пирсона и Спирмана, в форме heat-map
таблицы оценить степень сопряженности пар признаков-переменных:
- на всем датасете - в каждом отдельном классе датасета.

Выполнить scatterplot визуализацию датасета по всем парам выбранных переменных, показав:
- на графиках разных пар признаков объекты классов разными по форме/цвету точками;
- в графиках с одной и той же парой переменных – диаграммы плотности распределения
каждого класса по оси признака.
"""

CLASS_COL = 'target'
FEATURES = feature_names
classes = sorted(df1[CLASS_COL].unique())

# =====================================================================================
# 1. СТАТИСТИКА ДАТАСЕТА
# =====================================================================================
pd.set_option('display.width', 250)
pd.set_option('display.max_columns', 50)

print("\n=== 1. Статистика датасета ===")
print(f"Размерность: {df1.shape[0]} объектов x {df1.shape[1]} столбцов "
      f"({len(FEATURES)} признаков + столбец классов '{CLASS_COL}')")

print("\nТипы признаков и число NaN / нулевых значений:")
types_tbl = pd.DataFrame({
    "тип": df1.dtypes.astype(str),
    "NaN (ячеек)": df1.isna().sum(),
    "0 (ячеек)": (df1 == 0).sum(),
})
print(types_tbl.to_string())

nan_rows = df1[FEATURES].isna().any(axis=1)
zero_rows = (df1[FEATURES] == 0).any(axis=1)
print("\nКоличество классов и объектов в каждом классе:")
class_tbl = pd.DataFrame({
    "объектов": df1.groupby(CLASS_COL).size(),
    "объектов с NaN": nan_rows.groupby(df1[CLASS_COL]).sum(),
    "объектов с 0": zero_rows.groupby(df1[CLASS_COL]).sum(),
})
class_tbl.loc["всего"] = class_tbl.sum()
print(f"Классов: {len(classes)}")
print(class_tbl.to_string())

print("\nОценки выборки (весь датасет и по каждому классу):")
groups = {"все": df1[FEATURES]}
groups.update({f"класс {c}": df1.loc[df1[CLASS_COL] == c, FEATURES] for c in classes})
est_tbl = pd.concat(
    {name: g.agg(["mean", "std", "min", "median", "max", "skew", "kurt"]).T
     for name, g in groups.items()},
    axis=1,
)
print(est_tbl.round(3).to_string())

# =====================================================================================
# 2. КОРРЕЛЯЦИЯ ПИРСОНА И СПИРМЕНА (HEAT-MAP)
# =====================================================================================
def plot_correlation_heatmaps(data, title_prefix):
    """Две тепловые карты рядом: Пирсон и Спирмен."""
    fig, axes = plt.subplots(1, 2, figsize=(20, 9))
    for ax, method, name in zip(axes, ("pearson", "spearman"), ("Пирсон", "Спирмен")):
        corr = data[FEATURES].corr(method=method)
        sns.heatmap(
            corr, annot=True, cmap='coolwarm', vmin=-1, vmax=1, fmt=".2f",
            ax=ax, square=True, annot_kws={"size": 7}, cbar_kws={'shrink': 0.8},
        )
        ax.set_title(f"{title_prefix}\n{name}", fontsize=12)
    plt.tight_layout()
    plt.show()

print("\n=== 2. Heat-map корреляций ===")
plot_correlation_heatmaps(df1, f"Весь датасет (N={len(df1)})")
for cls in classes:
    sub = df1[df1[CLASS_COL] == cls]
    plot_correlation_heatmaps(sub, f"Класс {cls} (N={len(sub)})")

# =====================================================================================
# 3. SCATTERPLOT ПО ПАРАМ ПРИЗНАКОВ
# =====================================================================================
# Для читаемости берём первые 4 признака (6 пар). Список можно расширить до FEATURES.
SCATTER_FEATURES = FEATURES[:4]
MARGINAL_GRID = 256
MARGINAL_FRACTION = 0.18
MARGINAL_GAP = 0.025


def draw_marginals(result, data, x_name, y_name):
    """Плотность каждого класса вдоль X (сверху) и вдоль Y (справа) поверх scatter-рисунка."""
    box = result.ax.get_position()
    top_h = min(MARGINAL_FRACTION * box.height, 0.94 - (box.y1 + MARGINAL_GAP))
    right_w = min(MARGINAL_FRACTION * box.width, 0.97 - (box.x1 + MARGINAL_GAP))
    grid_x = np.linspace(result.scaffold.xlim[0], result.scaffold.xlim[1], MARGINAL_GRID)
    grid_y = np.linspace(result.scaffold.ylim[0], result.scaffold.ylim[1], MARGINAL_GRID)

    top_ax = result.fig.add_axes([box.x0, box.y1 + MARGINAL_GAP, box.width, top_h])
    right_ax = result.fig.add_axes([box.x1 + MARGINAL_GAP, box.y0, right_w, box.height])
    for panel in (top_ax, right_ax):
        panel.set_facecolor("none")
        panel.set_xticks([])
        panel.set_yticks([])
        for spine in panel.spines.values():
            spine.set_visible(False)
    top_ax.set_xlim(grid_x[0], grid_x[-1])
    top_ax.set_ylim(0, 1.05)
    right_ax.set_ylim(grid_y[0], grid_y[-1])
    right_ax.set_xlim(0, 1.05)

    for cls in classes:
        color = result.class_styles[cls]["color"]
        mask = data[CLASS_COL] == cls
        for values, grid, panel, horizontal in (
            (data.loc[mask, x_name].to_numpy(), grid_x, top_ax, True),
            (data.loc[mask, y_name].to_numpy(), grid_y, right_ax, False),
        ):
            if np.ptp(values) == 0:
                continue
            z = gaussian_kde(values)(grid)
            z = z / z.max()
            if horizontal:
                panel.fill_between(grid, 0, z, color=color, alpha=0.35, linewidth=0)
                panel.plot(grid, z, color=color, linewidth=1.1)
            else:
                panel.fill_betweenx(grid, 0, z, color=color, alpha=0.35, linewidth=0)
                panel.plot(z, grid, color=color, linewidth=1.1)


def plot_pair(data, x_name, y_name):
    """Диаграмма рассеяния пары признаков с плотностями классов по осям."""
    config = {
        "figure": {
            "size": 7.0,
            "plot_fraction": 0.58,
            "colorbar": False,
            "title": f"Признаки {x_name} и {y_name} · N = {len(data)}",
            "xlabel": x_name,
            "ylabel": y_name,
        },
        "points": {"alpha" : 0.3},
        "density": {"show": False, "mode": "off"},
        "regression": {"show": False},
        "class_regression": {"show": False},
    }
    result = scatterplot.scatter2d(
        data, x=x_name, y=y_name, class_column=CLASS_COL,
        class_order=classes, config=config,
    )
    draw_marginals(result, data, x_name, y_name)
    return result


print("\n=== 3. Scatterplot по парам признаков ===")
PAIR_FIGURES = {}
for x_name, y_name in combinations(SCATTER_FEATURES, 2):
    PAIR_FIGURES[(x_name, y_name)] = plot_pair(df1, x_name, y_name)
    plt.show()


# =====================================================================================
# 4. ДАТАСЕТЫ dfrX: ОБЪЕКТЫ ОДНОГО ПЕРЕСЕКАЮЩЕГОСЯ КЛАССА ПОВТОРЕНЫ X РАЗ
# =====================================================================================
OVERLAP_CLASSES = [0, 1]          # пересекающиеся классы
REPEATS = [1, 2, 5, 10, 100, 1000]  # X с учетом исходного объекта


def make_repeated(data, cls, times):
    """Добавить к данным times - 1 копий объектов класса cls; остальные классы не меняются."""
    cls_rows = data[data[CLASS_COL] == cls]
    return pd.concat([data] + [cls_rows] * (times - 1), ignore_index=True)


def centers_table(data, overlap):
    """Центры масс каждого класса, общий центр выбранных классов и центр отрезка между ними."""
    rows = {}
    for c in classes:
        rows[f"центр масс класса {c}"] = data.loc[data[CLASS_COL] == c, FEATURES].mean()
    rows[f"общий центр классов {overlap}"] = data.loc[data[CLASS_COL].isin(overlap), FEATURES].mean()
    a, b = (rows[f"центр масс класса {c}"] for c in overlap)
    rows[f"центр отрезка {overlap[0]}-{overlap[1]}"] = (a + b) / 2
    return pd.DataFrame(rows).T


DFR = {}
print("\n=== 4. Центры масс для датасетов dfrX ===")
for cls in OVERLAP_CLASSES:
    for times in REPEATS:
        DFR[(cls, times)] = make_repeated(df1, cls, times)
        dfr = DFR[(cls, times)]
        print(f"\ndfr{times}: повторен класс {cls}, объектов = {len(dfr)}, "
              f"по классам = {dfr[CLASS_COL].value_counts().sort_index().to_dict()}")
        print(centers_table(dfr, OVERLAP_CLASSES).round(3).to_string())


# =====================================================================================
# 5. РЕШАЮЩИЕ ФУНКЦИИ LDA, QDA, GNB, MDC ДЛЯ ДАТАСЕТОВ dfrX (ПУНКТ 3.3)
# 6. ОЦЕНКА КАЧЕСТВА: ROC И PR КРИВЫЕ С БУТСТРЕП-ПОЛОСАМИ 95% (ПУНКТ 3.4)
# 7. ДРУГОЙ ЦЕЛЕВОЙ КЛАСС: ПОВТОР ПУНКТОВ 3.3-3.4 (ПУНКТ 3.5)
# =====================================================================================
from sklearn.metrics import (roc_curve, roc_auc_score, precision_recall_curve,
                             average_precision_score)

PAIR_CLASSES = [0, 1]                   # выбранная пара классов (класс 0 с повторенными объектами)
REPEATED_CLASS = 0
TARGET_CLASSES = [1, 0]                 # 1 -- пункты 3.3-3.4; 0 -- другой целевой класс (3.5)
FEAT_PAIR = ["feature_1", "feature_2"]  # выбранная пара количественных признаков
METHODS = ["LDA", "QDA", "GNB", "MDC"]
COLORS = {0: "#1f77b4", 1: "#d62728"}
LIGHT_COLORS = {0: "#cfe2f3", 1: "#f4cccc"}  # светлые оттенки областей классов
MARKERS = {0: "o", 1: "s"}
METHOD_COLORS = {"LDA": "#1f77b4", "QDA": "#ff7f0e", "GNB": "#2ca02c", "MDC": "#9467bd"}

BOOT_N = 100
CI_LEVEL = 0.95
CI_Q = [100 * (1 - CI_LEVEL) / 2, 100 * (1 + CI_LEVEL) / 2]
FPR_GRID = np.linspace(0.0, 1.0, 201)
REC_GRID = np.linspace(0.0, 1.0, 201)
boot_rng = np.random.default_rng(42)

pair_df = df1[df1[CLASS_COL].isin(PAIR_CLASSES)]
pts = pair_df[FEAT_PAIR].to_numpy(float)
pad = 1.0
gx = np.linspace(pts[:, 0].min() - pad, pts[:, 0].max() + pad, 200)
gy = np.linspace(pts[:, 1].min() - pad, pts[:, 1].max() + pad, 200)
xx, yy = np.meshgrid(gx, gy)
grid = np.column_stack([xx.ravel(), yy.ravel()])


def other_class(target):
    return [c for c in PAIR_CLASSES if c != target][0]


def decision_score(method, X, y, points):
    """Решающая функция: > 0 в области целевого класса (y == 1), == 0 на решающей границе."""
    if method == "MDC":   # минимальное расстояние до центра масс класса
        m0 = X[y == 0].mean(axis=0)
        m1 = X[y == 1].mean(axis=0)
        return np.linalg.norm(points - m0, axis=1) - np.linalg.norm(points - m1, axis=1)
    model = {"LDA": LinearDiscriminantAnalysis(),
             "QDA": QuadraticDiscriminantAnalysis(),
             "GNB": GaussianNB()}[method]
    model.fit(X, y)
    p = np.clip(model.predict_proba(points), 1e-300, 1.0)
    return np.log(p[:, 1]) - np.log(p[:, 0])


def fit_data(times, target):
    """Обучающие данные dfr{times}: признаки, бинарная метка (1 -- целевой класс), центры масс."""
    fit_df = DFR[(REPEATED_CLASS, times)]
    fit_df = fit_df[fit_df[CLASS_COL].isin(PAIR_CLASSES)]
    X = fit_df[FEAT_PAIR].to_numpy(float)
    y = (fit_df[CLASS_COL].to_numpy() == target).astype(int)
    centres = {c: fit_df.loc[fit_df[CLASS_COL] == c, FEAT_PAIR].mean().to_numpy(float)
               for c in PAIR_CLASSES}
    return X, y, centres


def draw_decision_cell(ax, times, method, target):
    """Решающая граница, области классов, центры масс и отрезок для dfr{times} и метода method."""
    X, y, centres = fit_data(times, target)
    mid = (centres[PAIR_CLASSES[0]] + centres[PAIR_CLASSES[1]]) / 2
    score = decision_score(method, X, y, grid).reshape(xx.shape)
    acc = np.mean((decision_score(method, X, y, X) > 0) == y)

    ax.contourf(xx, yy, (score > 0).astype(int), levels=[-0.5, 0.5, 1.5],
                colors=[LIGHT_COLORS[other_class(target)], LIGHT_COLORS[target]])
    ax.contour(xx, yy, score, levels=[0.0], colors="k", linewidths=1.3)
    for c in PAIR_CLASSES:
        obs = pair_df.loc[pair_df[CLASS_COL] == c, FEAT_PAIR].to_numpy(float)
        ax.scatter(obs[:, 0], obs[:, 1], s=12, marker=MARKERS[c], color=COLORS[c],
                   alpha=0.7, edgecolors="none")
    c0, c1 = centres[PAIR_CLASSES[0]], centres[PAIR_CLASSES[1]]
    ax.plot([c0[0], c1[0]], [c0[1], c1[1]], ls="--", color="k", linewidth=1)
    for c in PAIR_CLASSES:
        ax.scatter(*centres[c], marker="X", s=160, color=COLORS[c], edgecolors="k", zorder=5)
    ax.scatter(*mid, marker="*", s=220, color="gold", edgecolors="k", zorder=6)

    ax.set_xlim(gx[0], gx[-1])
    ax.set_ylim(gy[0], gy[-1])
    ax.set_title(f"dfr{times} · {method} · acc = {acc:.3f}", fontsize=11, pad=1)


def decision_legend_handles(target):
    other = other_class(target)
    return [
        Line2D([], [], marker=MARKERS[other], ls="", color=COLORS[other], label=f"класс {other}"),
        Line2D([], [], marker=MARKERS[target], ls="", color=COLORS[target],
               label=f"класс {target} (целевой)"),
        Line2D([], [], color="k", lw=1.3, label="решающая граница"),
        Line2D([], [], marker="X", ls="", color="grey", markeredgecolor="k", markersize=10,
               label="центры масс классов"),
        Line2D([], [], ls="--", color="k", lw=1, label="отрезок между центрами"),
        Line2D([], [], marker="*", ls="", color="gold", markeredgecolor="k", markersize=14,
               label="середина отрезка"),
    ]


def plot_decision_panel(target):
    """Панно 3.3: строки -- датасеты dfrX, столбцы -- методы."""
    fig, axes = plt.subplots(len(REPEATS), len(METHODS),
                             figsize=(26, 5.1 * len(REPEATS)), squeeze=False)
    for r, times in enumerate(REPEATS):
        for k, method in enumerate(METHODS):
            draw_decision_cell(axes[r, k], times, method, target)
            if r == len(REPEATS) - 1:
                axes[r, k].set_xlabel(FEAT_PAIR[0])
            if k == 0:
                axes[r, k].set_ylabel(FEAT_PAIR[1])
    fig.legend(handles=decision_legend_handles(target), loc="lower center", ncol=6,
               bbox_to_anchor=(0.5, 0.0), fontsize=10)
    fig.suptitle(f"Целевой класс {target}. Датасеты dfrX (повторен класс {REPEATED_CLASS}) x методы "
                 f"LDA, QDA, GNB, MDC; признаки {FEAT_PAIR[0]}, {FEAT_PAIR[1]}", fontsize=13, y=0.995)
    fig.subplots_adjust(left=0.03, right=0.997, top=0.975, bottom=0.045, wspace=0.14, hspace=0.25)
    plt.show()


def bootstrap_curves(y, s, n_boot):
    """Бутстреп по объектам: ROC и PR кривые на общих сетках и значения AUROC / AUPRC."""
    tpr_b, prec_b, auroc_b, auprc_b = [], [], [], []
    n = len(y)
    for _ in range(n_boot):
        idx = boot_rng.integers(0, n, n)
        yb, sb = y[idx], s[idx]
        fpr, tpr, _ = roc_curve(yb, sb)
        tpr_b.append(np.interp(FPR_GRID, fpr, tpr))
        auroc_b.append(roc_auc_score(yb, sb))
        prec, rec, _ = precision_recall_curve(yb, sb)
        prec_b.append(np.interp(REC_GRID, rec[::-1], prec[::-1]))
        auprc_b.append(average_precision_score(yb, sb))
    return np.array(tpr_b), np.array(prec_b), np.array(auroc_b), np.array(auprc_b)


def evaluate(times, method, target):
    """Кривые и метрики для dfr{times} и метода method при целевом классе target."""
    X, y, _ = fit_data(times, target)
    s = decision_score(method, X, y, X)
    fpr, tpr, _ = roc_curve(y, s)
    prec, rec, _ = precision_recall_curve(y, s)
    tpr_b, prec_b, auroc_b, auprc_b = bootstrap_curves(y, s, BOOT_N)
    tpr_lo, tpr_hi = np.percentile(tpr_b, CI_Q, axis=0)
    prec_lo, prec_hi = np.percentile(prec_b, CI_Q, axis=0)
    return {
        "fpr": fpr, "tpr": tpr, "rec": rec, "prec": prec,
        "tpr_b": tpr_b, "prec_b": prec_b,
        "tpr_lo": tpr_lo, "tpr_hi": tpr_hi, "prec_lo": prec_lo, "prec_hi": prec_hi,
        "auroc": roc_auc_score(y, s), "auroc_ci": np.percentile(auroc_b, CI_Q),
        "auprc": average_precision_score(y, s), "auprc_ci": np.percentile(auprc_b, CI_Q),
        "prevalence": y.mean(),
    }


def ci_text(value, ci):
    return f"{value:.3f} [{ci[0]:.3f}; {ci[1]:.3f}]"


def draw_roc_single(ax, res, times, method):
    """ROC кривая одного классификатора: бутстреп-линии и полоса 95%."""
    color = METHOD_COLORS[method]
    for curve in res["tpr_b"]:
        ax.plot(FPR_GRID, curve, color=color, linewidth=0.4, alpha=0.15)
    ax.fill_between(FPR_GRID, res["tpr_lo"], res["tpr_hi"], color=color, alpha=0.3, linewidth=0)
    ax.plot(res["fpr"], res["tpr"], color="k", linewidth=1.5)
    ax.plot([0, 1], [0, 1], ls=":", color="grey", linewidth=1)
    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1.01)
    ax.set_xlabel("FPR")
    ax.set_ylabel("TPR")
    ax.set_title(f"dfr{times} · {method} · ROC\nAUROC = {ci_text(res['auroc'], res['auroc_ci'])}",
                 fontsize=10, pad=1)


def draw_pr_single(ax, res, times, method):
    """PR кривая одного классификатора: бутстреп-линии и полоса 95%."""
    color = METHOD_COLORS[method]
    for curve in res["prec_b"]:
        ax.plot(REC_GRID, curve, color=color, linewidth=0.4, alpha=0.15)
    ax.fill_between(REC_GRID, res["prec_lo"], res["prec_hi"], color=color, alpha=0.3, linewidth=0)
    ax.plot(res["rec"], res["prec"], color="k", linewidth=1.5)
    ax.axhline(res["prevalence"], ls=":", color="grey", linewidth=1)
    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1.01)
    ax.set_xlabel("Recall")
    ax.set_ylabel("Precision")
    ax.set_title(f"dfr{times} · {method} · PR\nAUPRC = {ci_text(res['auprc'], res['auprc_ci'])}",
                 fontsize=10, pad=1)


def draw_roc_compare(ax, results, times):
    """ROC кривые всех классификаторов с полосами 95% (без линий бутстрепа)."""
    for method in METHODS:
        res, color = results[(times, method)], METHOD_COLORS[method]
        ax.fill_between(FPR_GRID, res["tpr_lo"], res["tpr_hi"], color=color, alpha=0.15, linewidth=0)
        ax.plot(res["fpr"], res["tpr"], color=color, linewidth=1.4,
                label=f"{method}: AUROC = {ci_text(res['auroc'], res['auroc_ci'])}")
    ax.plot([0, 1], [0, 1], ls=":", color="grey", linewidth=1)
    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1.01)
    ax.set_xlabel("FPR")
    ax.set_ylabel("TPR")
    ax.set_title(f"dfr{times} · ROC всех классификаторов", fontsize=10, pad=1)


def plot_quality_panel(target, results):
    """Панно 3.4: решения LDA | ROC LDA | PR LDA | ROC всех классификаторов."""
    fig, axes = plt.subplots(len(REPEATS), 4, figsize=(24, 4.8 * len(REPEATS)), squeeze=False)
    for r, times in enumerate(REPEATS):
        draw_decision_cell(axes[r, 0], times, "LDA", target)
        draw_roc_single(axes[r, 1], results[(times, "LDA")], times, "LDA")
        draw_pr_single(axes[r, 2], results[(times, "LDA")], times, "LDA")
        draw_roc_compare(axes[r, 3], results, times)
        axes[r, 3].legend(fontsize=8, loc="upper left", bbox_to_anchor=(1.02, 1.0), borderaxespad=0)
    fig.suptitle(f"Целевой класс {target}. Колонки: решающие области LDA | ROC LDA с полосой 95% и "
                 f"бутстреп-линиями | PR LDA с полосой 95% и бутстреп-линиями | "
                 f"ROC всех классификаторов с полосами 95%", fontsize=12, y=0.985)
    fig.subplots_adjust(left=0.04, right=0.84, top=0.965, bottom=0.04, wspace=0.3, hspace=0.25)
    plt.show()


# =====================================================================================
# 7. ЗАПУСК ДЛЯ КАЖДОГО ЦЕЛЕВОГО КЛАССА (3.3-3.4 и пункт 3.5 с другим целевым классом)
# =====================================================================================
for target in TARGET_CLASSES:
    print(f"\n=== Целевой класс: {target} ===")
    plot_decision_panel(target)
    RESULTS = {(times, method): evaluate(times, method, target)
               for times in REPEATS for method in METHODS}
    plot_quality_panel(target, RESULTS)


# =====================================================================================
# 8. КРОСС-ВАЛИДАЦИЯ dfr10: КРИВЫЕ AUROC И AUPRC С ПОЛОСАМИ CI95 (ПУНКТ 3.6)
# =====================================================================================
from sklearn.model_selection import StratifiedGroupKFold

CV_DATASET = 10
CV_FOLDS = [3, 5, 10, 20, 50, 100]
CV_TARGET = 1
CV_METHOD = "LDA"


def cv_splits(times, target, n_folds):
    """Стратифицированные фолды; копии одного объекта всегда попадают в один фолд."""
    fit_df = DFR[(REPEATED_CLASS, times)]
    fit_df = fit_df[fit_df[CLASS_COL].isin(PAIR_CLASSES)].reset_index(drop=True)
    X = fit_df[FEAT_PAIR].to_numpy(float)
    y = (fit_df[CLASS_COL].to_numpy() == target).astype(int)
    groups = fit_df.groupby(feature_names).ngroup().to_numpy()
    splitter = StratifiedGroupKFold(n_splits=n_folds, shuffle=True, random_state=42)
    return X, y, list(splitter.split(X, y, groups))


def cv_curves(times, n_folds):
    """Кривые и метрики по фолдам кросс-валидации для dfr{times}."""
    X, y, splits = cv_splits(times, CV_TARGET, n_folds)
    tpr_f, prec_f, auroc_f, auprc_f = [], [], [], []
    for train, test in splits:
        s = decision_score(CV_METHOD, X[train], y[train], X[test])
        yt = y[test]
        fpr, tpr, _ = roc_curve(yt, s)
        tpr_f.append(np.interp(FPR_GRID, fpr, tpr))
        auroc_f.append(roc_auc_score(yt, s))
        prec, rec, _ = precision_recall_curve(yt, s)
        prec_f.append(np.interp(REC_GRID, rec[::-1], prec[::-1]))
        auprc_f.append(average_precision_score(yt, s))
    return {"tpr_f": np.array(tpr_f), "prec_f": np.array(prec_f),
            "auroc_f": np.array(auroc_f), "auprc_f": np.array(auprc_f),
            "prevalence": y.mean()}


def plot_cv_curve(kind, n_folds, res):
    """Отдельный график: все линии фолдов, средняя кривая и полоса CI95 по фолдам."""
    color = METHOD_COLORS[CV_METHOD]
    if kind == "AUROC":
        grid, curves, values = FPR_GRID, res["tpr_f"], res["auroc_f"]
    else:
        grid, curves, values = REC_GRID, res["prec_f"], res["auprc_f"]
    lo, hi = np.percentile(curves, CI_Q, axis=0)
    v_lo, v_hi = np.percentile(values, CI_Q)

    fig, ax = plt.subplots(figsize=(7, 7))
    for curve in curves:
        ax.plot(grid, curve, color=color, linewidth=0.6, alpha=0.35)
    ax.fill_between(grid, lo, hi, color=color, alpha=0.3, linewidth=0, label="CI95 полоса")
    ax.plot(grid, curves.mean(axis=0), color="k", linewidth=1.8, label="среднее по фолдам")
    if kind == "AUROC":
        ax.plot([0, 1], [0, 1], ls=":", color="grey", linewidth=1)
        ax.set_xlabel("FPR")
        ax.set_ylabel("TPR")
    else:
        ax.axhline(res["prevalence"], ls=":", color="grey", linewidth=1)
        ax.set_xlabel("Recall")
        ax.set_ylabel("Precision")
    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1.01)
    ax.legend(fontsize=9, loc="lower right")
    ax.set_title(f"{CV_METHOD}, dfr{CV_DATASET}, {n_folds}-fold CV · {kind}\n"
                 f"{kind} = {values.mean():.3f} [{v_lo:.3f}; {v_hi:.3f}]", fontsize=11)
    plt.tight_layout()
    plt.show()


for n_folds in CV_FOLDS:
    CV_RESULTS = cv_curves(CV_DATASET, n_folds)
    plot_cv_curve("AUROC", n_folds, CV_RESULTS)
    plot_cv_curve("AUPRC", n_folds, CV_RESULTS)


# =====================================================================================
# 9. LDA НА 2, 4, 8 И 16 ПРИЗНАКАХ ДЛЯ dfr1 (ПУНКТ 3.7)
# =====================================================================================
FEATURE_COUNTS = [2, 4, 8, 16]
FEATURE_COLORS = {2: "#1f77b4", 4: "#ff7f0e", 8: "#2ca02c", 16: "#9467bd"}
FEATURE_BOOT_N = 100


def lda_feature_curves(times=1, target=1):
    """ROC/PR-кривые LDA для разных количеств первых признаков с bootstrap CI95."""
    data = DFR[(REPEATED_CLASS, times)]
    data = data[data[CLASS_COL].isin(PAIR_CLASSES)]
    y = (data[CLASS_COL].to_numpy() == target).astype(int)
    result = {}

    for n_features_used in FEATURE_COUNTS:
        selected = FEATURES[:n_features_used]
        model = LinearDiscriminantAnalysis()
        model.fit(data[selected].to_numpy(float), y)
        scores = model.predict_proba(data[selected].to_numpy(float))[:, 1]

        fpr, tpr, _ = roc_curve(y, scores)
        precision, recall, _ = precision_recall_curve(y, scores)
        tpr_boot, precision_boot = [], []
        auroc_boot, auprc_boot = [], []

        for _ in range(FEATURE_BOOT_N):
            indices = boot_rng.integers(0, len(y), len(y))
            y_boot, scores_boot = y[indices], scores[indices]
            fpr_boot, tpr_one, _ = roc_curve(y_boot, scores_boot)
            tpr_boot.append(np.interp(FPR_GRID, fpr_boot, tpr_one))
            precision_one, recall_boot, _ = precision_recall_curve(y_boot, scores_boot)
            precision_boot.append(np.interp(REC_GRID, recall_boot[::-1], precision_one[::-1]))
            auroc_boot.append(roc_auc_score(y_boot, scores_boot))
            auprc_boot.append(average_precision_score(y_boot, scores_boot))

        result[n_features_used] = {
            "fpr": fpr,
            "tpr": tpr,
            "recall": recall,
            "precision": precision,
            "tpr_lo": np.percentile(tpr_boot, CI_Q[0], axis=0),
            "tpr_hi": np.percentile(tpr_boot, CI_Q[1], axis=0),
            "precision_lo": np.percentile(precision_boot, CI_Q[0], axis=0),
            "precision_hi": np.percentile(precision_boot, CI_Q[1], axis=0),
            "auroc": roc_auc_score(y, scores),
            "auroc_lo": np.percentile(auroc_boot, CI_Q[0]),
            "auroc_hi": np.percentile(auroc_boot, CI_Q[1]),
            "auprc": average_precision_score(y, scores),
            "auprc_lo": np.percentile(auprc_boot, CI_Q[0]),
            "auprc_hi": np.percentile(auprc_boot, CI_Q[1]),
        }
    return result


LDA_FEATURE_RESULTS = lda_feature_curves(times=1, target=1)

fig, (roc_ax, pr_ax) = plt.subplots(1, 2, figsize=(18, 7.2))
for n_features_used in FEATURE_COUNTS:
    result = LDA_FEATURE_RESULTS[n_features_used]
    color = FEATURE_COLORS[n_features_used]
    label_roc = (f"{n_features_used} признака: AUROC = {result['auroc']:.3f} "
                 f"[{result['auroc_lo']:.3f}; {result['auroc_hi']:.3f}]")
    label_pr = (f"{n_features_used} признака: AUPRC = {result['auprc']:.3f} "
                f"[{result['auprc_lo']:.3f}; {result['auprc_hi']:.3f}]")

    roc_ax.fill_between(FPR_GRID, result["tpr_lo"], result["tpr_hi"],
                        color=color, alpha=0.18, linewidth=0)
    roc_ax.plot(result["fpr"], result["tpr"], color=color, linewidth=1.7, label=label_roc)
    pr_ax.fill_between(REC_GRID, result["precision_lo"], result["precision_hi"],
                       color=color, alpha=0.18, linewidth=0)
    pr_ax.plot(result["recall"], result["precision"], color=color, linewidth=1.7, label=label_pr)

roc_ax.plot([0, 1], [0, 1], ls=":", color="grey", linewidth=1)
roc_ax.set(xlim=(0, 1), ylim=(0, 1.01), xlabel="FPR", ylabel="TPR")
roc_ax.set_title("dfr1 · LDA на разном числе признаков · ROC", pad=1)
roc_ax.legend(fontsize=8, loc="lower right")

pr_ax.axhline(0.5, ls=":", color="grey", linewidth=1, label="базовый уровень")
pr_ax.set(xlim=(0, 1), ylim=(0, 1.01), xlabel="Recall", ylabel="Precision")
pr_ax.set_title("dfr1 · LDA на разном числе признаков · PR", pad=1)
pr_ax.legend(fontsize=8, loc="lower left")

fig.suptitle("Пункт 3.7: целевой класс 1, первые 2/4/8/16 признаков, CI95 bootstrap",
             fontsize=12, y=0.995)
fig.subplots_adjust(top=0.93, bottom=0.12, wspace=0.28)
plt.show()


# =====================================================================================
# 10. LDA, MDC, ЛИНЕЙНЫЙ SVM И LogReg (ПУНКТ 3.8*)
# =====================================================================================
from sklearn.svm import LinearSVC
from sklearn.linear_model import LogisticRegression

METHODS_38 = ["LDA", "MDC", "SVM", "LogReg"]
METHOD_COLORS_38 = {
    "LDA": "#1f77b4", "MDC": "#9467bd", "SVM": "#d62728", "LogReg": "#2ca02c",
}


def decision_score_38(method, X, y, points):
    """Скор > 0 соответствует целевому классу y == 1."""
    if method == "MDC":
        m0 = X[y == 0].mean(axis=0)
        m1 = X[y == 1].mean(axis=0)
        return np.linalg.norm(points - m0, axis=1) - np.linalg.norm(points - m1, axis=1)
    if method == "LDA":
        model = LinearDiscriminantAnalysis()
    elif method == "SVM":
        model = LinearSVC(dual="auto", max_iter=10000, random_state=42)
    elif method == "LogReg":
        model = LogisticRegression(max_iter=10000, random_state=42)
    else:
        raise ValueError(f"Неизвестный метод: {method}")
    model.fit(X, y)
    if method == "LDA":
        probabilities = np.clip(model.predict_proba(points), 1e-300, 1.0)
        return np.log(probabilities[:, 1]) - np.log(probabilities[:, 0])
    return model.decision_function(points)


def draw_decision_cell_38(ax, times, method, target=1):
    """Одна ячейка панно решений для пункта 3.8*."""
    fit_df = DFR[(REPEATED_CLASS, times)]
    fit_df = fit_df[fit_df[CLASS_COL].isin(PAIR_CLASSES)]
    X = fit_df[FEAT_PAIR].to_numpy(float)
    y = (fit_df[CLASS_COL].to_numpy() == target).astype(int)
    score_grid = decision_score_38(method, X, y, grid).reshape(xx.shape)
    score_objects = decision_score_38(method, X, y, X)
    accuracy = np.mean((score_objects > 0) == y)
    other = other_class(target)

    ax.contourf(xx, yy, (score_grid > 0).astype(int), levels=[-0.5, 0.5, 1.5],
                colors=[LIGHT_COLORS[other], LIGHT_COLORS[target]])
    ax.contour(xx, yy, score_grid, levels=[0.0], colors="k", linewidths=1.3)
    centres = {c: fit_df.loc[fit_df[CLASS_COL] == c, FEAT_PAIR].mean().to_numpy(float)
               for c in PAIR_CLASSES}
    midpoint = (centres[PAIR_CLASSES[0]] + centres[PAIR_CLASSES[1]]) / 2
    for c in PAIR_CLASSES:
        obs = pair_df.loc[pair_df[CLASS_COL] == c, FEAT_PAIR].to_numpy(float)
        ax.scatter(obs[:, 0], obs[:, 1], s=12, marker=MARKERS[c], color=COLORS[c],
                   alpha=0.7, edgecolors="none")
        ax.scatter(*centres[c], marker="X", s=160, color=COLORS[c], edgecolors="k", zorder=5)
    ax.plot(centres[0][[0, 0]], centres[0][[1, 1]], alpha=0)  # сохраняем одинаковую структуру осей
    ax.plot([centres[0][0], centres[1][0]], [centres[0][1], centres[1][1]],
            ls="--", color="k", linewidth=1)
    ax.scatter(*midpoint, marker="*", s=220, color="gold", edgecolors="k", zorder=6)
    ax.set_xlim(gx[0], gx[-1])
    ax.set_ylim(gy[0], gy[-1])
    ax.set_title(f"dfr{times} · {method} · acc = {accuracy:.3f}", fontsize=11)


fig, axes = plt.subplots(len(REPEATS), len(METHODS_38),
                         figsize=(18, 4.2 * len(REPEATS)), squeeze=False)
for row, times in enumerate(REPEATS):
    for col, method in enumerate(METHODS_38):
        draw_decision_cell_38(axes[row, col], times, method, target=1)
        if row == len(REPEATS) - 1:
            axes[row, col].set_xlabel(FEAT_PAIR[0])
        if col == 0:
            axes[row, col].set_ylabel(FEAT_PAIR[1])
fig.suptitle("Пункт 3.8*: LDA, MDC, линейный SVM и LogReg; целевой класс 1",
             fontsize=13, y=1.02)
fig.subplots_adjust(left=0.04, right=0.99, top=0.975, bottom=0.07, wspace=0.25, hspace=0.25)
plt.show()


def evaluate_38(times, method, target=1):
    """Метрики и bootstrap-кривые для одного dfrX и одного метода."""
    fit_df = DFR[(REPEATED_CLASS, times)]
    fit_df = fit_df[fit_df[CLASS_COL].isin(PAIR_CLASSES)]
    X = fit_df[FEAT_PAIR].to_numpy(float)
    y = (fit_df[CLASS_COL].to_numpy() == target).astype(int)
    scores = decision_score_38(method, X, y, X)
    fpr, tpr, _ = roc_curve(y, scores)
    precision, recall, _ = precision_recall_curve(y, scores)
    tpr_boot, precision_boot, auroc_boot, auprc_boot = [], [], [], []
    for _ in range(BOOT_N):
        indices = boot_rng.integers(0, len(y), len(y))
        y_boot, scores_boot = y[indices], scores[indices]
        fpr_b, tpr_b, _ = roc_curve(y_boot, scores_boot)
        p_b, r_b, _ = precision_recall_curve(y_boot, scores_boot)
        tpr_boot.append(np.interp(FPR_GRID, fpr_b, tpr_b))
        precision_boot.append(np.interp(REC_GRID, r_b[::-1], p_b[::-1]))
        auroc_boot.append(roc_auc_score(y_boot, scores_boot))
        auprc_boot.append(average_precision_score(y_boot, scores_boot))
    return {
        "fpr": fpr, "tpr": tpr, "precision": precision, "recall": recall,
        "tpr_boot": np.asarray(tpr_boot), "precision_boot": np.asarray(precision_boot),
        "tpr_lo": np.percentile(tpr_boot, CI_Q[0], axis=0),
        "tpr_hi": np.percentile(tpr_boot, CI_Q[1], axis=0),
        "precision_lo": np.percentile(precision_boot, CI_Q[0], axis=0),
        "precision_hi": np.percentile(precision_boot, CI_Q[1], axis=0),
        "auroc": roc_auc_score(y, scores), "auroc_ci": np.percentile(auroc_boot, CI_Q),
        "auprc": average_precision_score(y, scores), "auprc_ci": np.percentile(auprc_boot, CI_Q),
        "prevalence": y.mean(),
    }


RESULTS_38 = {(times, method): evaluate_38(times, method, target=1)
              for times in REPEATS for method in METHODS_38}


def draw_roc_38(ax, times):
    """ROC SVM с бутстреп-линиями и CI95; справа — сравнение четырёх методов."""
    svm = RESULTS_38[(times, "SVM")]
    for curve in svm["tpr_boot"]:
        ax.plot(FPR_GRID, curve, color=METHOD_COLORS_38["SVM"], linewidth=0.4, alpha=0.15)
    ax.fill_between(FPR_GRID, svm["tpr_lo"], svm["tpr_hi"],
                    color=METHOD_COLORS_38["SVM"], alpha=0.3, linewidth=0)
    ax.plot(svm["fpr"], svm["tpr"], color=METHOD_COLORS_38["SVM"], linewidth=1.6)
    ax.plot([0, 1], [0, 1], ls=":", color="grey")
    ax.set(xlim=(0, 1), ylim=(0, 1.01), xlabel="FPR", ylabel="TPR")
    ax.set_title(f"dfr{times} · SVM ROC\nAUROC = {ci_text(svm['auroc'], svm['auroc_ci'])}", fontsize=10)


def draw_pr_38(ax, times):
    """PR SVM с бутстреп-линиями и CI95."""
    svm = RESULTS_38[(times, "SVM")]
    for curve in svm["precision_boot"]:
        ax.plot(REC_GRID, curve, color=METHOD_COLORS_38["SVM"], linewidth=0.4, alpha=0.15)
    ax.fill_between(REC_GRID, svm["precision_lo"], svm["precision_hi"],
                    color=METHOD_COLORS_38["SVM"], alpha=0.3, linewidth=0)
    ax.plot(svm["recall"], svm["precision"], color=METHOD_COLORS_38["SVM"], linewidth=1.6)
    ax.axhline(svm["prevalence"], ls=":", color="grey")
    ax.set(xlim=(0, 1), ylim=(0, 1.01), xlabel="Recall", ylabel="Precision")
    ax.set_title(f"dfr{times} · SVM PR\nAUPRC = {ci_text(svm['auprc'], svm['auprc_ci'])}", fontsize=10)


def draw_compare_38(ax, times):
    """Сравнение ROC LDA, MDC, SVM и LogReg с CI95 без bootstrap-линий."""
    for method in METHODS_38:
        result = RESULTS_38[(times, method)]
        color = METHOD_COLORS_38[method]
        ax.fill_between(FPR_GRID, result["tpr_lo"], result["tpr_hi"], color=color, alpha=0.15)
        ax.plot(result["fpr"], result["tpr"], color=color, linewidth=1.4,
                label=f"{method}: {ci_text(result['auroc'], result['auroc_ci'])}")
    ax.plot([0, 1], [0, 1], ls=":", color="grey")
    ax.set(xlim=(0, 1), ylim=(0, 1.01), xlabel="FPR", ylabel="TPR")
    ax.set_title(f"dfr{times} · ROC сравнение", fontsize=10)
    ax.legend(fontsize=8, loc="upper left", bbox_to_anchor=(1.02, 1.0), borderaxespad=0)


fig, axes = plt.subplots(len(REPEATS), 4, figsize=(24, 4.8 * len(REPEATS)), squeeze=False)
for row, times in enumerate(REPEATS):
    draw_decision_cell_38(axes[row, 0], times, "SVM", target=1)
    draw_roc_38(axes[row, 1], times)
    draw_pr_38(axes[row, 2], times)
    draw_compare_38(axes[row, 3], times)
fig.suptitle("Пункт 3.8*: SVM — ROC/PR с CI95; сравнение ROC LDA, MDC, SVM, LogReg",
             fontsize=12, y=0.985)
fig.subplots_adjust(left=0.04, right=0.84, top=0.965, bottom=0.04, wspace=0.3, hspace=0.25)
plt.show()
