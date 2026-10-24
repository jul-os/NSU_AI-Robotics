import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from itertools import combinations
from scipy.stats import gaussian_kde

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
