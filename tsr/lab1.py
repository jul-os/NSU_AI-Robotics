import numpy as np
import pandas as pd
import seaborn as sns

import matplotlib.pyplot as plt

from plots import raincloud, upset, scatterplot
from plots.upset import build_upset_visualization

# Фиксируем random_state для воспроизводимости
np.random.seed(42)

n_samples = 1000
n_features = 4

# 1. Желаемая корреляционная матрица
R = np.array([
    [1.0, 0.9, 0.0, 0.0],
    [0.9, 1.0, 0.0, 0.0],
    [0.0, 0.0, 1.0, 0.0],
    [0.0, 0.0, 0.0, 1.0]
])

# 2. Генерируем базовые независимые признаки с помощью multivariate_normal
mean = np.zeros(n_features)
cov_independent = np.eye(n_features)
X = np.random.multivariate_normal(mean, cov_independent, size=n_samples)

# 3. Центрируем данные (вычитаем выборочное среднее, чтобы оно стало строго 0)
X_centered = X - X.mean(axis=0)

# 4. Приводим признаки к строго ортонормированному базису через QR-разложение.
# Это убирает любые случайные корреляции, возникшие при генерации.
Q, _ = np.linalg.qr(X_centered)

# 5. Применяем разложение Холецкого к желаемой матрице R (R = L @ L.T)
L = np.linalg.cholesky(R)

# 6. Трансформируем ортонормированные признаки для получения СТРОГО заданных
# выборочных корреляций. Множитель sqrt(n - 1) необходим, так как столбцы Q
# имеют единичную норму, а выборочная ковариация pandas делится на (n - 1).
Y = Q * np.sqrt(n_samples - 1) @ L.T

# 7. Генерируем 1 класс (например, бинарный)

# 8. Формируем итоговый датафрейм dfS00
dfS00 = pd.DataFrame(Y, columns=['A', 'B', 'C', 'D'])
# 7. Генерируем 1 класс для всех 1000 объектов
dfS00['class'] = 'class_1'  # Все объекты принадлежат одному классу


# Проверка выборочной корреляционной матрицы
print("Корреляционная матрица выборки dfS00:")
print(dfS00[['A', 'B', 'C', 'D']].corr())
"""
Корреляционная матрица выборки dfS00:
              A             B             C             D
A  1.000000e+00  9.000000e-01 -2.667202e-18 -4.067484e-17
B  9.000000e-01  1.000000e+00  1.244694e-17 -2.639419e-17
C -2.667202e-18  1.244694e-17  1.000000e+00  4.723171e-18
D -4.067484e-17 -2.639419e-17  4.723171e-18  1.000000e+00
Пояснение: признаки A,B имеют корреляцию 0.9, все остальные значения имеют порядок 1e-18, 1e-17, то есть почти 0
"""
print("=" * 60)
print("1. РАЗМЕРНОСТЬ ДАТАСЕТА")
print("=" * 60)
print(f"Общее количество объектов (строк): {dfS00.shape[0]}")
print(f"Общее количество признаков (столбцов): {dfS00.shape[1]}")

print("\n" + "=" * 60)
print("2. ТИП КАЖДОГО ПРИЗНАКА")
print("=" * 60)
types_df = dfS00.dtypes.to_frame('Тип данных')
print(types_df)

print("\n" + "=" * 60)
print("3. КОЛИЧЕСТВО КЛАССОВ И ОБЪЕКТОВ В КАЖДОМ ИЗ НИХ")
print("=" * 60)
class_counts = dfS00['class'].value_counts().to_frame('Количество объектов').rename_axis('Класс')
# Добавим процентное соотношение для информативности
class_counts['Доля (%)'] = (class_counts['Количество объектов'] / dfS00.shape[0] * 100).round(2)
print(class_counts)

print("\n" + "=" * 60)
print("4. ИНЫЕ ОЦЕНКИ ВЫБОРКИ (основные статистики)")
print("=" * 60)
# Транспонируем таблицу describe для удобства чтения по строкам
desc_stats = dfS00.describe().T
# Округлим значения для компактности вывода
print(desc_stats.round(4))

print("\n" + "=" * 60)
print("5. ЧИСЛО ОБЪЕКТОВ С NaN ПО КЛАССАМ")
print("=" * 60)
# Проверяем только признаки A, B, C, D, исключая сам столбец class из агрегации
nan_by_class = dfS00.groupby('class')[['A', 'B', 'C', 'D']].agg(lambda x: x.isna().sum())
print(nan_by_class)

print("\n" + "=" * 60)
print("6. ЧИСЛО ОБЪЕКТОВ СО ЗНАЧЕНИЕМ 0 ПО КЛАССАМ")
print("=" * 60)
# Проверяем строгое равенство нулю для признаков A, B, C, D
zero_by_class = dfS00.groupby('class')[['A', 'B', 'C', 'D']].agg(lambda x: (x == 0).sum())
print(zero_by_class)

print("\n" + "=" * 60)
print("ПРИМЕЧАНИЕ: Для непрерывных величин с плавающей точкой")
print("строгое равенство 0 встречается крайне редко. Если требуется")
print("оценить значения, близкие к нулю, используйте условие, например:")
print("(x.abs() < 1e-10).sum() вместо (x == 0).sum()")
print("=" * 60)

# ================================================================
# 8. ВСЕ ПРИЗНАКИ НА ОДНОМ ГРАФИКЕ
# ================================================================
print("\n" + "=" * 60)
print("8. ВСЕ ПРИЗНАКИ НА ОДНОМ ГРАФИКЕ")
print("=" * 60)

import matplotlib.pyplot as plt

# Превращаем "широкий" формат в "длинный":
# было:  A    B    C    D   class
#         1.2  0.3  -0.5  2.1  class_1
# стало: feature  value  class
#         A        1.2    class_1
#         B        0.3    class_1
#         C       -0.5    class_1
#         D        2.1    class_1
df_long = dfS00.melt(
    id_vars=['class'],
    value_vars=['A', 'B', 'C', 'D'],
    var_name='feature',
    value_name='value'
)

class_colors = {
    'A': '#2A9D8F',
    'B': '#E76F51',
    'C': '#264653',
    'D': '#E9C46A'
}

fig, ax = plt.subplots(figsize=(14, 7))

raincloud.raincloud_horizontal(
    dataset=df_long,
    feature_name='value',
    points_start_y=2.0,                  # смещение точек по оси Y
    class_column='feature',          # теперь "класс" — это имя признака
    class_order=['A', 'B', 'C', 'D'],
    class_colors=class_colors,
    boxplot_whiskers=(0, 100),       # Range (min-max)
    axis=ax,
    kde_alpha=0.12,
    x_label='Значение',
    show_legend=True
)

fig.tight_layout()
plt.show()


# ================================================================
# 9. КОРРЕЛЯЦИЯ ПИРСОНА И СПИРМЕНА (HEATMAP)
# ================================================================

print("\n" + "=" * 60)
print("9. КОРРЕЛЯЦИЯ ПИРСОНА И СПИРМЕНА (HEATMAP)")
print("=" * 60)

features = ['A', 'B', 'C', 'D']

def plot_correlation_heatmaps(data_subset, title_prefix):
    """
    Строит две тепловые карты рядом: Пирсон и Спирмен.
    """
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))
    
    # 1. Коэффициент корреляции Пирсона (линейная связь)
    pearson_corr = data_subset.corr(method='pearson')
    sns.heatmap(
        pearson_corr, 
        annot=True, 
        cmap='coolwarm', 
        vmin=-1, 
        vmax=1,
        fmt=".2f", 
        ax=axes[0], 
        square=True,
        cbar_kws={'shrink': 0.8}
    )
    axes[0].set_title(f"{title_prefix}\nПирсон (линейная)", fontsize=12)
    
    # 2. Коэффициент корреляции Спирмена (монотонная связь, ранговая)
    spearman_corr = data_subset.corr(method='spearman')
    sns.heatmap(
        spearman_corr, 
        annot=True, 
        cmap='coolwarm', 
        vmin=-1, 
        vmax=1,
        fmt=".2f", 
        ax=axes[1], 
        square=True,
        cbar_kws={'shrink': 0.8}
    )
    axes[1].set_title(f"{title_prefix}\nСпирмен (ранговая)", fontsize=12)
    
    plt.tight_layout()
    plt.show()

# --- А. Оценка на всем датасете ---
print("Построение heat-map для всего датасета...")
plot_correlation_heatmaps(dfS00[features], "Весь датасет (N=1000)")

# --- Б. Оценка в каждом отдельном классе ---
unique_classes = dfS00['class'].unique()
for cls in unique_classes:
    print(f"Построение heat-map для класса: {cls}")
    df_class_subset = dfS00[dfS00['class'] == cls][features]
    plot_correlation_heatmaps(df_class_subset, f"Класс '{cls}' (N={len(df_class_subset)})")

print("Визуализация корреляций завершена.")


# =====================================================================================
# 10. ДВУМЕРНАЯ ВИЗУАЛИЗАЦИЯ ДАТАСЕТА ПО ВСЕМ ПАРАМ ВЫБРАННЫХ ПРИЗНАКОВ
# =====================================================================================
# На каждом рисунке:
#   * точки объектов, различающиеся по форме и цвету в зависимости от класса
#     (диаграмма рассеяния строится движком plots/scatterplot.py);
#   * маргинальные диаграммы плотности распределения КАЖДОГО класса вдоль каждой
#     из двух осей признака, различающиеся по цвету и прозрачности.
# Все параметры визуализации собраны в словаре PAIR_VIS: цвета и толщины осей,
# число/шаг больших и малых рисков, сетка, квадратность рисунка, размер и тип
# шрифтов каждого типа подписей, отключаемые легенды и заглавия, прозрачность
# объектов.
# =====================================================================================

from itertools import combinations

from matplotlib.lines import Line2D
from matplotlib.patches import Polygon

# -------------------------------------------------------------------------------------
# 10.1. НАСТРОЙКИ ВИЗУАЛИЗАЦИИ (регулируются пользователем)
# -------------------------------------------------------------------------------------
PAIR_VIS = {
    # ------------------- что именно рисуем -------------------
    "variables": ["A", "B", "C", "D"],   # выбранные переменные
    "class_column": "class",             # столбец с метками классов
    # None -> все пары из combinations(variables, 2); либо свой список [(x, y), ...]
    "pairs": None,

    # ------------------- заглавия (можно отключить) -------------------
    "title": {
        "show": True,
        "template": "Признаки {x} и {y} · N = {n}",
    },

    # ------------------- легенда (можно отключить) -------------------
    "legend": {
        "show": True,
        "loc": "best",         # "best" | "upper right" | ...
        "frame": True,
        "alpha": 0.9,
        "markerscale": 1.3,
    },

    # ------------------- рисунок и его квадратность -------------------
    "figure": {
        "size": 7.0,           # сторона квадратного рисунка, дюймы
        "square": True,        # True -> рисунок строго квадратный
        "aspect": 1.0,         # ширина/высота, если square == False
        "plot_fraction": 0.58, # сторона квадратной области данных (доля рисунка)
        # если маргинальные панели выключены, область данных расширяется
        "plot_fraction_plain": 0.80,
    },

    # ------------------- оси: цвет и толщина -------------------
    "axes": {
        "color": "#1a1a1a",
        "linewidth": 1.2,
        "spines": "left_bottom",    # "frame" | "left_bottom" | "none"
        "square_data_area": True,   # одинаковое число единиц данных на дюйм по X и Y
        "margin": 0.04,             # автоматический отступ от границ данных
        "xlim": None,               # ручные границы, например (-4, 4)
        "ylim": None,
    },

    # ------------------- риски: число/шаг больших и малых -------------------
    "ticks": {
        "show": True,              # общее включение рисков и их подписей
        "labels": True,
        "minor_labels": False,     # подписывать ли малые риски
        "major_count": 7,          # сколько БОЛЬШИХ рисков на оси
        "major_start": None,       # значение, от которого считаются большие риски
        "major_step": None,        # явный ШАГ больших рисков (важнее major_count)
        "minor_count": 4,          # сколько МАЛЫХ рисков между большими
        "major_size": 5.0,         # длина большого риска, пункты
        "minor_size": 2.5,         # длина малого риска, пункты
        "width": 1.0,              # толщина штрихов рисков
        "direction": "out",        # "out" | "in" | "inout"
        "label_offset": 6.0,       # отступ подписи от оси, пункты
        "precision": None,         # число знаков после запятой
        "rotation": 0.0,           # поворот подписей рисков, градусы
        "collision": {             # борьба с наложением подписей
            "enabled": True,
            "strategy": "hide",    # "hide" | "stagger" | "rotate" | "none"
            "gap": 2.0,
        },
    },

    # ------------------- сетка пространства -------------------
    "grid": {
        "show": True,
        "minor": True,             # показывать ли сетку по малым рискам
        "color": "#7a7a7a",
        "alpha": 0.25,
        "linestyle": "-",
        "linewidth": 0.7,
        "minor_color": "#9a9a9a",
        "minor_alpha": 0.12,
        "minor_linestyle": ":",
        "minor_linewidth": 0.5,
    },

    # ------------------- точки: прозрачность объектов -------------------
    "points": {
        "size": 26.0,              # площадь маркера, пункты^2
        "size_auto": True,         # уменьшать маркеры на больших облаках
        "size_auto_min": 4.0,
        "size_auto_max": 26.0,
        "alpha": 0.65,             # прозрачность точек
        "alpha_mode": "fixed",     # "fixed" | "auto"
        "alpha_auto": {"max_alpha": 0.85, "min_alpha": 0.04,
                       "pivot": 400.0, "power": 0.45},
        "edge_width": 0.4,         # толщина контура маркера
        "zorder": 3,
    },

    # ------------------- классы: разные форма и цвет точек -------------------
    "class_style": {
        "palette": ["#1f77b4", "#d62728", "#2ca02c", "#ff7f0e",
                    "#9467bd", "#8c564b", "#e377c2", "#7f7f7f"],
        "shapes": ["circle", "square", "diamond", "triangle_up",
                   "star", "plus"],
        "filled": True,
        "sizes": None,             # None -> points.size
        "alphas": None,            # None -> points.alpha
        "per_class": {},           # точечные переопределения: {"class_1": {...}}
    },

    # ------------------- шрифты: размер и тип для каждого типа подписей -------
    "font": {
        "family": "DejaVu Sans",  # базовая гарнитура для всех подписей
        "size": {                 # размер, пункты
            "tick": 9.0,          # подписи рисков осей
            "label": 11.0,        # подписи осей (названия признаков)
            "title": 12.0,        # заголовок рисунка
            "legend": 8.0,        # легенда
            "marginal": 7.0,      # подписи маргинальных осей плотности
            "colorbar": 7.0,
        },
        "family_by_role": {},     # {"tick": "Arial"} — гарнитура для одного типа
        "weight": {"title": "bold"},
        "style": {},              # {"label": "italic"}
    },

    # ------------------- маргинальные плотности классов по осям признака ----
    "marginal": {
        "show": True,             # общее включение маргинальных диаграмм
        "grid": 256,              # число точек сетки оценки плотности
        "bw_method": "scott",     # правило сглаживания (или конкретное число)
        "min_points": 8,          # меньше -- используется сглаженная гистограмма
        "fallback_bins": 32,      # число столбцов запасной гистограммы
        "fallback_sigma": 1.5,    # ширина гауссова сглаживания (в ячейках)
        "normalise": "density",   # "density" (площадь 1) | "peak" (максимум 1)
        "scale": "per_class",     # "per_class" | "shared" — сравнимость кривых
        "alpha": 0.35,            # прозрачность заливки под кривой
        "line_width": 1.1,
        "line_alpha": 0.95,
        "color": None,            # None -> цвет класса
        "spines": "box",          # "box" | "bottom_left" | "none"
        "linewidth": 0.8,
        "show_ticks": False,      # свои риски на маргинальных панелях
        "axis_label": True,       # подпись "плотность" у маргинальных осей
        "axis_label_text": "плотность",
        "gap": 0.025,             # зазор между главной и маргинальной панелями
        "fraction": 0.18,         # доля стороны главной панели
        "top_limit": 0.94,        # верхняя граница (место под заголовок)
        "right_limit": 0.97,
    },
}


# -------------------------------------------------------------------------------------
# 10.2. СБОРКА КОНФИГУРАЦИИ ДВИЖКА plots/scatterplot.py ИЗ НАСТРОЕК ПОЛЬЗОВАТЕЛЯ
# -------------------------------------------------------------------------------------
def _pair_class_styles(vis, classes):
    """Разный цвет и форма точек для каждого класса + точечные переопределения."""
    ccfg, pcfg = vis["class_style"], vis["points"]
    styles = {}
    for i, cls in enumerate(classes):
        style = {
            "color": ccfg["palette"][i % len(ccfg["palette"])],
            "shape": ccfg["shapes"][i % len(ccfg["shapes"])],
            "filled": bool(ccfg["filled"]),
            "alpha": pcfg["alpha"] if ccfg["alphas"] is None else ccfg["alphas"][i % len(ccfg["alphas"])],
            "size": pcfg["size"] if ccfg["sizes"] is None else ccfg["sizes"][i % len(ccfg["sizes"])],
        }
        style.update(ccfg["per_class"].get(cls, {}))   # что явно задал пользователь
        styles[cls] = style
    return styles


def _pair_title(vis, x_name, y_name, n_objects):
    """Заголовок рисунка для пары признаков (или None, если заглавия отключены)."""
    tcfg = vis["title"]
    if not tcfg["show"]:
        return None
    return tcfg["template"].format(x=x_name, y=y_name, n=n_objects)


def _engine_config(vis, classes, title, x_name, y_name):
    """Перевести настройки пользователя в конфигурацию движка scatterplot."""
    sizes = vis["font"]["size"]
    engine = {
        "figure": {
            "size": vis["figure"]["size"],
            # маргинальные панели занимают место по краям квадратной области данных
            "plot_fraction": (vis["figure"]["plot_fraction"] if vis["marginal"]["show"]
                              else vis["figure"]["plot_fraction_plain"]),
            "colorbar": False,                     # цветовой шкалы плотности нет
            "title": title,
            "xlabel": x_name,                      # подписи осей = имена признаков
            "ylabel": y_name,
            "title_size": sizes["title"],
            "label_size": sizes["label"],
            "legend": bool(vis["legend"]["show"]),
            "legend_loc": vis["legend"]["loc"],
            "legend_size": sizes["legend"],
            "legend_frameon": vis["legend"]["frame"],
            "legend_alpha": vis["legend"]["alpha"],
            "legend_markerscale": vis["legend"]["markerscale"],
        },
        "font": {
            "tick_size": sizes["tick"],
            "label_size": sizes["label"],
            "title_size": sizes["title"],
            "legend_size": sizes["legend"],
            "colorbar_size": sizes["colorbar"],
            "color": vis["axes"]["color"],
        },
        "axes": dict(vis["axes"]),
        "ticks": {**vis["ticks"], "label_size": sizes["tick"],
                  "color": vis["axes"]["color"]},
        "grid": dict(vis["grid"]),
        "points": dict(vis["points"]),
        "classes": _pair_class_styles(vis, classes),
        # 2D-плотность и линии тренда в этом задании не нужны: плотность
        # показывается маргинально, по каждому классу в отдельности
        "density": {"show": False, "mode": "off"},
        "regression": {"show": False},
        "class_regression": {"show": False},
    }
    return scatterplot.merge_config(scatterplot.DEFAULT_CONFIG, engine)


# -------------------------------------------------------------------------------------
# 10.3. ОДНОМЕРНАЯ ПЛОТНОСТЬ РАСПРЕДЕЛЕНИЯ КЛАССА ПО ОСИ ПРИЗНАКА
# -------------------------------------------------------------------------------------
def _smoothed_histogram(values, grid, bins, sigma_cells):
    """Запасная оценка плотности: гистограмма, сглаженная гауссовым ядром.

    Результат переносится на рабочую сетку ``grid`` линейной интерполяцией, чтобы
    длина кривой всегда совпадала с числом точек сетки.
    """
    counts, edges = np.histogram(values, bins=int(bins), range=(grid[0], grid[-1]))
    centres = 0.5 * (edges[:-1] + edges[1:])
    radius = max(1, int(np.ceil(3.0 * float(sigma_cells))))
    kernel = np.exp(-0.5 * (np.arange(-radius, radius + 1) / float(sigma_cells)) ** 2)
    kernel /= kernel.sum()
    padded = np.pad(counts.astype(float), radius, mode="edge")
    smooth = np.convolve(padded, kernel, mode="same")[radius:-radius]
    area = float(smooth.sum()) * float(centres[1] - centres[0])
    smooth = smooth / max(area, 1e-12)                 # площадь под кривой = 1
    return np.interp(grid, centres, smooth)


def _marginal_density(values, grid, mcfg):
    """Плотность распределения значений одного класса на сетке ``grid``.

    Основная оценка — одномерная ядерная оценка (KDE); если объектов слишком мало
    либо scipy недоступен, используется сглаженная гистограмма.
    """
    values = np.asarray(values, dtype=float)
    values = values[np.isfinite(values)]
    if values.size == 0:
        return None

    if values.size >= int(mcfg["min_points"]) and float(np.ptp(values)) > 0:
        try:
            from scipy.stats import gaussian_kde  # noqa: WPS433 (опциональная зависимость)
            kde = gaussian_kde(values, bw_method=mcfg["bw_method"])
            variance = float(np.atleast_2d(kde.covariance)[0, 0])
            if np.isfinite(variance) and variance > 1e-12:
                z = np.asarray(kde(grid), dtype=float)
                if np.all(np.isfinite(z)) and z.max() > 0:
                    return z
        except Exception:
            pass    # тихо переходим к гистограмме

    if float(np.ptp(values)) <= 0:     # все значения класса совпадают
        z = np.zeros_like(grid)
        z[int(np.argmin(np.abs(grid - values[0])))] = 1.0
        return z
    return _smoothed_histogram(values, grid, mcfg["fallback_bins"], mcfg["fallback_sigma"])


def _normalise_density(z, grid, mode):
    """Привести кривую к единице площади (``density``) или к единице максимума (``peak``)."""
    peak = float(z.max())
    if mode == "peak" or peak <= 0:
        return z / peak if peak > 0 else z
    dx = (grid[-1] - grid[0]) / max(1, grid.size - 1)
    area = float(z.sum()) * dx
    return z / area if area > 0 else z


# -------------------------------------------------------------------------------------
# 10.4. МАРГИНАЛЬНЫЕ ДИАГРАММЫ ПЛОТНОСТИ (СВЕРХУ ПО X, СПРАВА ПО Y)
# -------------------------------------------------------------------------------------
def _marginal_panel(fig, rect, horizontal, grid, curves, peak, styles, vis):
    """Нарисовать одну маргинальную панель: заливка + контур кривой для каждого класса.

    ``horizontal=True`` -> плотность вдоль признака X (панель сверху),
    ``horizontal=False`` -> плотность вдоль признака Y (панель справа).
    Кривые каждого класса отличаются цветом и прозрачностью.
    """
    mcfg, sizes = vis["marginal"], vis["font"]["size"]
    fcfg = vis["font"]
    font = dict(family=(fcfg.get("family_by_role") or {}).get("marginal", fcfg.get("family")),
                weight=(fcfg.get("weight") or {}).get("marginal", "normal"),
                style=(fcfg.get("style") or {}).get("marginal", "normal"))

    ax = fig.add_axes(rect)
    ax.set_facecolor("none")
    ax.patch.set_alpha(0.0)
    ax.set_xticks([])
    ax.set_yticks([])

    visible = {"box": ("left", "right", "top", "bottom"),
               "bottom_left": ("bottom", "left"),
               "none": ()}[str(mcfg["spines"])]
    for side, spine in ax.spines.items():
        spine.set_visible(side in visible)
        spine.set_linewidth(float(mcfg["linewidth"]))
        spine.set_color(vis["axes"]["color"])
    if not bool(mcfg["show_ticks"]):
        ax.tick_params(which="both", length=0, labelbottom=False, labelleft=False)

    # ------------------------------------------- приведение кривых к общему масштабу
    # "per_class" -- каждая кривая нормируется по своему максимуму (видно все классы),
    # "shared"    -- все кривые делят один максимум (видны реальные различия высот).
    per_class = str(mcfg["scale"]) == "per_class"
    drawn = [(cls, z / float(z.max()) if per_class and z.max() > 0 else z / peak)
             for cls, z in curves]
    height = max((float(z.max()) for _, z in drawn), default=1.0) * 1.06

    # ---------------------------------------------------------------- пределы ----
    if horizontal:
        ax.set_xlim(grid[0], grid[-1])
        ax.set_ylim(0.0, height)
    else:
        ax.set_ylim(grid[0], grid[-1])
        ax.set_xlim(0.0, height)

    # ------------------------------------------------- кривые плотности классов --
    artists = []
    for cls, scaled in drawn:
        style = styles.get(cls, {})
        color = mcfg["color"] or style.get("color", "#1f77b4")
        if horizontal:
            base, tip = (grid[0], 0.0), (grid[-1], 0.0)
            area = Polygon([base, *zip(grid, scaled), tip], closed=True,
                           facecolor=color, edgecolor="none",
                           alpha=float(mcfg["alpha"]), zorder=1)
            line = Line2D(grid, scaled, color=color, linewidth=float(mcfg["line_width"]),
                          alpha=float(mcfg["line_alpha"]), solid_joinstyle="round", zorder=3)
        else:
            base, tip = (0.0, grid[0]), (0.0, grid[-1])
            area = Polygon([base, *zip(scaled, grid), tip], closed=True,
                           facecolor=color, edgecolor="none",
                           alpha=float(mcfg["alpha"]), zorder=1)
            line = Line2D(scaled, grid, color=color, linewidth=float(mcfg["line_width"]),
                          alpha=float(mcfg["line_alpha"]), solid_joinstyle="round", zorder=3)
        ax.add_patch(area)
        ax.add_line(line)
        artists += [area, line]

    # -------------------------------------------- подпись маргинальной оси ------
    if bool(mcfg["axis_label"]):
        text = mcfg["axis_label_text"]
        if horizontal:
            ax.set_ylabel(text, fontdict=font, fontsize=float(sizes["marginal"]),
                          color=vis["axes"]["color"], rotation=90, labelpad=1)
        else:
            ax.set_xlabel(text, fontdict=font, fontsize=float(sizes["marginal"]),
                          color=vis["axes"]["color"], rotation=90, labelpad=1)
    return ax, artists


def _draw_marginals(result, vis):
    """Достроить вокруг готовой диаграммы рассеяния плотности классов по обеим осям."""
    mcfg = vis["marginal"]
    if not bool(mcfg["show"]):                   # маргинальные панели отключены
        return []
    box = result.ax.get_position()
    gap, fraction = float(mcfg["gap"]), float(mcfg["fraction"])

    top_h = min(fraction * box.height, float(mcfg["top_limit"]) - (box.y1 + gap))
    right_w = min(fraction * box.width, float(mcfg["right_limit"]) - (box.x1 + gap))
    if top_h <= 0.02 or right_w <= 0.02:      # места для панелей не хватило
        return []

    data, styles = result.data, result.class_styles
    grid_x = np.linspace(result.scaffold.xlim[0], result.scaffold.xlim[1], int(mcfg["grid"]))
    grid_y = np.linspace(result.scaffold.ylim[0], result.scaffold.ylim[1], int(mcfg["grid"]))

    panels = []
    for axis, horizontal, grid, values, rect in (
        ("x", True, grid_x, data.x, [box.x0, box.y1 + gap, box.width, top_h]),
        ("y", False, grid_y, data.y, [box.x1 + gap, box.y0, right_w, box.height]),
    ):
        curves = []
        for cls in data.classes:
            z = _marginal_density(values[data.mask(cls)], grid, mcfg)
            if z is not None:
                curves.append((cls, _normalise_density(z, grid, mcfg["normalise"])))
        if not curves:
            continue
        peak = max(float(z.max()) for _, z in curves) or 1.0
        panels.append(_marginal_panel(result.fig, rect, horizontal, grid,
                                      curves, peak, styles, vis))
    return panels


# -------------------------------------------------------------------------------------
# 10.5. ШРИФТЫ: РАЗМЕР И ТИП ДЛЯ КАЖДОГО ТИПА ПОДПИСЕЙ
# -------------------------------------------------------------------------------------
def _apply_fonts(result, vis, title, x_name, y_name):
    """Проставить гарнитуру, начертание и размер каждому типу подписей рисунка."""
    fcfg = vis["font"]
    family = fcfg.get("family") or scatterplot.FONT.family
    families, weights, styles, sizes = (fcfg.get("family_by_role") or {},
                                        fcfg.get("weight") or {},
                                        fcfg.get("style") or {},
                                        fcfg.get("size") or {})

    def _apply(text, role):
        text.set_fontfamily(families.get(role, family))
        text.set_fontweight(weights.get(role, "normal"))
        text.set_fontstyle(styles.get(role, "normal"))
        if role in sizes:
            text.set_fontsize(float(sizes[role]))

    for text in result.scaffold.label_artists:            # подписи рисков осей
        _apply(text, "tick")

    roles = {title: "title", x_name: "label", y_name: "label"}
    for text in result.fig.texts:                         # заголовок и подписи осей
        _apply(text, roles.get(text.get_text(), "label"))

    legend = result.ax.get_legend()
    if legend is not None:
        for text in legend.get_texts():
            _apply(text, "legend")

    if result.colorbar_axes is not None:
        for text in result.colorbar_axes.texts:
            _apply(text, "colorbar")


# -------------------------------------------------------------------------------------
# 10.6. ПОСТРОЕНИЕ РИСУНКА ДЛЯ ОДНОЙ ПАРЫ ПРИЗНАКОВ И ДЛЯ ВСЕХ ПАР
# -------------------------------------------------------------------------------------
def plot_feature_pair(dataset, x_name, y_name, vis=None):
    """Диаграмма рассеяния пары признаков + маргинальные плотности классов.

    Параметры
    ----------
    dataset : pandas.DataFrame
        Датасет с числовыми признаками и столбцом меток классов.
    x_name, y_name : str
        Имена двух выбранных признаков (ось X и ось Y).
    vis : dict, optional
        Настройки визуализации; по умолчанию ``PAIR_VIS``.

    Returns
    -------
    scatterplot.ScatterResult
        Рисунок, к которому добавлены маргинальные панели плотности.
    """
    vis = PAIR_VIS if vis is None else vis
    class_column = vis["class_column"]
    if class_column not in dataset.columns:
        raise KeyError(f"в датасете нет столбца классов {class_column!r}")
    for name in (x_name, y_name):
        if name not in dataset.columns:
            raise KeyError(f"в датасете нет признака {name!r}")

    classes = list(dict.fromkeys(dataset[class_column].tolist()))
    title = _pair_title(vis, x_name, y_name, len(dataset))
    config = _engine_config(vis, classes, title, x_name, y_name)

    result = scatterplot.scatter2d(
        dataset,
        x=x_name,
        y=y_name,
        class_column=class_column,
        class_order=classes,
        config=config,
        return_fig=True,
    )

    # квадратность рисунка: по умолчанию строго квадратный
    if not bool(vis["figure"]["square"]):
        side = float(vis["figure"]["size"])
        result.fig.set_size_inches(side * float(vis["figure"]["aspect"]), side)

    result.marginal_panels = _draw_marginals(result, vis)
    _apply_fonts(result, vis, title, x_name, y_name)
    return result


def plot_all_feature_pairs(dataset, vis=None):
    """Построить 2D-визуализацию по всем парам выбранных переменных.

    Возвращает словарь ``{(x, y): ScatterResult}`` — по одному рисунку на пару.
    """
    vis = PAIR_VIS if vis is None else vis
    variables = list(vis["variables"])
    pairs = vis["pairs"] or list(combinations(variables, 2))

    figures = {}
    for x_name, y_name in pairs:
        print(f"  пара признаков: X = {x_name}, Y = {y_name}")
        figures[(x_name, y_name)] = plot_feature_pair(dataset, x_name, y_name, vis)
    print(f"Построено рисунков: {len(figures)} (по одному на пару признаков).")
    return figures


print("\n" + "=" * 60)
print("10. ДВУМЕРНАЯ ВИЗУАЛИЗАЦИЯ ПО ВСЕМ ПАРАМ ПРИЗНАКОВ")
print("=" * 60)
print("Рисуем точки классов разными формой/цветом и маргинальные плотности "
      "каждого класса по обеим осям.")

PAIR_FIGURES = plot_all_feature_pairs(dfS00)

plt.show()


# =====================================================================================
# 11. КЛОНИРОВАНИЕ ДАТАСЕТА С УДАЛЁННЫМИ ЯЧЕЙКАМИ И ПАННО ИЗ ДИАГРАММ РАССЕЯНИЯ
# =====================================================================================
# Из исходного датасета dfS00 создаётся набор клонов dfSdXX (XX — процент удалённых
# ячеек): значение каждой ячейки с вероятностью, заданной пользователем, заменяется на
# NaN. Затем для выбираемой пары признаков каждый клон изображается как панно m x n из
# рисунков scatterplot (по одному рисунку на клон), построенных движком
# plots/scatterplot.py. Во всех клетках панно общие пределы осей, иначе картинки
# нельзя было бы сравнивать между собой.
# =====================================================================================

import math

# -------------------------------------------------------------------------------------
# 11.1. НАСТРОЙКИ ЗАДАНИЯ (регулируются пользователем)
# -------------------------------------------------------------------------------------
DELETION_VIS = {
    # ------------------- что и сколько удаляем -------------------
    "percentages": [0, 1, 2, 5, 10, 20, 50, 80],  # доля удаляемых ячеек, %
    "columns": None,      # None -> все числовые признаки (столбец классов не трогаем)
    "seed": 42,           # для воспроизводимости случайного удаления

    # ------------------- выбираемая пара признаков -------------------
    "class_column": "class",
    "x": "A",
    "y": "B",

    # ------------------- общее для всех клеток панно -------------------
    "shared_limits": True,   # единые пределы осей во всех клетках (сравнимость!)
    "margin": 0.04,          # отступ от крайних значений при расчёте общих пределов

    # ------------------- панно m x n -------------------
    "panel": {
        "ncols": None,            # None -> m и n подбираются автоматически
        "cell_size": 3.3,         # сторона клетки, дюймы (клетка всегда квадратная)
        "gap_x": 0.30,            # горизонтальный зазор между клетками, дюймы
        "gap_y": 0.52,            # вертикальный зазор (место под заголовки клеток)
        "left": 0.75,             # поля рисунка, дюймы
        "right": 0.40,
        "top": 0.75,              # место под заголовок панно, легенду и заголовки клеток
        "bottom": 0.80,
        "dpi": 110,
        "show_title": True,
        "title": "Признаки {x} и {y} · панно из {k} рисунков scatterplot",
        "tick_labels": "outer",   # "outer" (крайние клетки) | "all" | "none"
        "cell_title": "{name} · {percent} %\nудалено {n_del} яч. · объектов {objects}",
        "legend": True,           # одна общая легенда на всё панно
        "legend_loc": "upper right",
        "legend_size": 9.0,
        "axis_labels": True,      # общие подписи осей панно
        "facecolor": "white",
    },

    # ------------------- внешний вид клетки (переопределяет PAIR_VIS) ----------------
    "style": {
        "points": {"size": 16.0, "alpha": 0.55, "size_auto_max": 16.0},
        "ticks": {"major_count": 5, "minor_count": 2, "major_size": 3.5,
                  "minor_size": 2.0, "width": 0.8, "label_offset": 4.0},
        "grid": {"alpha": 0.20, "minor": False, "linewidth": 0.6},
        "font": {"size": {"tick": 8.0, "label": 10.0, "title": 9.0,
                          "legend": 9.0, "marginal": 7.0, "colorbar": 7.0}},
    },
}


# -------------------------------------------------------------------------------------
# 11.2. КЛОНИРОВАНИЕ ДАТАСЕТА: СЛУЧАЙНОЕ УДАЛЕНИЕ ЗАДАННОЙ ДОЛИ ЯЧЕЕК
# -------------------------------------------------------------------------------------
def _deletion_columns(dataset, vis):
    """Столбцы, из которых разрешено удалять значения (по умолчанию — все признаки)."""
    if vis["columns"]:
        return list(vis["columns"])
    skip = vis["class_column"]
    return [c for c in dataset.columns
            if c != skip and pd.api.types.is_numeric_dtype(dataset[c])]


def clone_with_deleted_cells(dataset, percent, vis=None):
    """Клонировать датасет, случайным образом удалив ``percent`` % его ячеек.

    Значение удалённой ячейки заменяется на ``NaN`` (объект с удалёнными значениями
    просто не попадает на диаграмму).  Возвращает
    ``(имя клона, копия датасета, число удалённых ячеек, всего ячеек)``.
    """
    vis = DELETION_VIS if vis is None else vis
    columns = _deletion_columns(dataset, vis)
    clone = dataset.copy()
    values = clone[columns].to_numpy(dtype=float)
    total = int(values.size)

    n_del = int(round(float(percent) / 100.0 * total))
    if n_del > 0:
        rng = np.random.default_rng(vis["seed"])
        flat = values.ravel().copy()
        flat[rng.choice(flat.size, size=min(n_del, flat.size), replace=False)] = np.nan
        clone[columns] = flat.reshape(values.shape)
    return f"dfSd{int(percent):02d}", clone, n_del, total


def build_deletion_datasets(dataset, vis=None):
    """Создать набор клонов ``dfSdXX`` — по одному на каждый процент удалённых ячеек.

    Возвращает ``(datasets, report)``, где ``datasets`` — словарь ``имя -> DataFrame``,
    а ``report`` — словарь ``имя -> (процент, число удалённых ячеек)``.
    """
    vis = DELETION_VIS if vis is None else vis
    datasets, report = {}, {}
    for percent in vis["percentages"]:
        name, clone, n_del, total = clone_with_deleted_cells(dataset, percent, vis)
        datasets[name] = clone
        report[name] = (percent, n_del)
        print(f"  {name}: удалено {n_del:5d} из {total} ячеек "
              f"({percent:>3} %), строк в клоне: {clone.shape[0]}")
    return datasets, report


# -------------------------------------------------------------------------------------
# 11.3. ДАННЫЕ ОДНОЙ КЛЕТКИ В ФОРМАТЕ ДВИЖКА plots/scatterplot.py
# -------------------------------------------------------------------------------------
def _panel_data(dataset, x_name, y_name, class_column, class_order):
    """Данные одной клетки панно в формате ``LongData`` (публичный формат движка).

    Строки, у которых удалено хотя бы одно из двух значений, отбрасываются —
    ровно так же, как это делает ``scatter2d``.
    """
    x = pd.to_numeric(dataset[x_name], errors="coerce").to_numpy(dtype=float)
    y = pd.to_numeric(dataset[y_name], errors="coerce").to_numpy(dtype=float)
    if class_column in dataset.columns:
        labels = dataset[class_column].to_numpy(dtype=object)
    else:
        labels = np.array(["all"] * x.size, dtype=object)

    finite = np.isfinite(x) & np.isfinite(y)
    if not finite.any():
        raise ValueError("в датасете нет ни одной конечной пары значений")
    x, y, labels = x[finite], y[finite], labels[finite]

    present = list(dict.fromkeys(labels.tolist()))
    classes = [c for c in class_order if c in present]
    classes += [c for c in present if c not in classes]
    return scatterplot.LongData(x, y, labels, classes, int((~finite).sum()))


def _panel_shape(n, ncols=None):
    """Подобрать размер панно ``m x n`` под ``n`` рисунков (как можно ближе к квадрату)."""
    if ncols:
        return math.ceil(n / int(ncols)), int(ncols)
    best = min(((math.ceil(n / r), r) for r in range(1, n + 1)),
               key=lambda rc: (rc[0] * rc[1], abs(rc[0] - rc[1]), rc[0]))
    return best


# -------------------------------------------------------------------------------------
# 11.4. ОДНА КЛЕТКА ПАННО
# -------------------------------------------------------------------------------------
def _draw_panel_cell(fig, rect, data, config, font, title, show_labels=(True, True)):
    """Нарисовать в заданной области рисунка одну ячейку панно.

    Каркас (оси, риски, сетка, подписи рисков), точки классов и стили берутся из
    ``plots/scatterplot.py``; вручную задаются только положение ячейки, её заголовок
    и то, какие подписи рисков оставить (``show_labels`` = (X, Y)).
    """
    ax = fig.add_axes(rect)
    ax.set_facecolor(str(config.get("figure", {}).get("axes_facecolor", "white")))
    for spine in ax.spines.values():
        spine.set_visible(False)

    # ---- каркас: рамка оси, риски, сетка, подписи рисков (с разрешением наложений) ----
    scaffold = scatterplot.draw_scaffold(fig, ax, data, config, config.get("font", {}))

    # ---- подписи рисков: у внутренних клеток панно лишние оси убираем --------------
    n_x = len(scaffold.xlabel_layout.hidden) if scaffold.xlabel_layout is not None else 0
    for text in scaffold.label_artists[:n_x]:
        if not show_labels[0]:
            text.set_visible(False)
    for text in scaffold.label_artists[n_x:]:
        if not show_labels[1]:
            text.set_visible(False)

    # ---- точки объектов: свой цвет и форма у каждого класса --------------------
    styles = {}
    for i, cls in enumerate(data.classes):
        style = scatterplot.resolve_class_style(cls, i, config)
        if style.get("alpha_mode") == "auto" and "alpha_auto" not in (
                (config.get("classes", {}) or {}).get(cls, {})):
            style["alpha_auto"] = config.get("points", {}).get("alpha_auto", {})
        styles[cls] = style
    for cls in data.classes:
        mask = data.mask(cls)
        scatterplot.draw_points(ax, data.x[mask], data.y[mask], styles[cls], config)

    # ---- гарнитура подписей рисков ------------------------------------------------
    if font["family"]:
        for text in scaffold.label_artists:
            text.set_fontfamily(font["family"])

    # ---- заголовок ячейки --------------------------------------------------------
    if title:
        ax.set_title(title, fontdict=dict(family=font["family"]),
                     fontsize=font["size"], color=font["color"], pad=4)
    return ax, scaffold, styles


def _panel_legend(fig, labels, styles, config, loc, size):
    """Одна легенда на всё панно (маркеры в ней строит движок scatterplot)."""
    probe = {**config, "figure": {**config.get("figure", {}), "legend": True}}
    tmp_fig, tmp_ax = plt.subplots()
    legend = scatterplot.draw_legend(tmp_fig, tmp_ax, labels, styles, probe)
    if legend is None:
        plt.close(tmp_fig)
        return None
    handles = list(legend.legend_handles)
    texts = [t.get_text() for t in legend.get_texts()]
    legend.remove()
    plt.close(tmp_fig)
    if not handles:
        return None

    legend = fig.legend(handles, texts, loc=loc, fontsize=float(size), frameon=True)
    for text in legend.get_texts():
        text.set_fontfamily(config.get("font", {}).get("family") or scatterplot.FONT.family)
    return legend


# -------------------------------------------------------------------------------------
# 11.5. ПАННО m x n ИЗ ДИАГРАММ РАССЕЯНИЯ
# -------------------------------------------------------------------------------------
def plot_deletion_panel(datasets, vis=None, report=None, source=None, cell_labels=None):
    """Изобразить набор клонов ``dfSdXX`` как панно ``m x n`` из диаграмм рассеяния.

    Параметры
    ----------
    datasets : dict
        ``имя клона -> DataFrame`` (как возвращает :func:`build_deletion_datasets`).
    vis : dict, optional
        Настройки задания; по умолчанию ``DELETION_VIS``.
    report : dict, optional
        ``имя -> (процент, число удалённых ячеек)`` для подписей ячеек.
    source : DataFrame, optional
        Исходный датасет: по нему считаются ОБЩИЕ пределы осей панно.
    cell_labels : dict, optional
        ``имя -> текст``; подставляется в поле ``{label}`` шаблона ``cell_title``.
        Позволяет подписать клетку произвольным текстом (например, именем метода
        заполнения), не подставляя его в поле ``{percent}``.

    Returns
    -------
    matplotlib.figure.Figure
        Рисунок-панно; список осей доступен через ``fig.axes``.
    """
    vis = DELETION_VIS if vis is None else vis
    pcfg, report = vis["panel"], report or {}
    cell_labels = cell_labels or {}
    x_name, y_name = vis["x"], vis["y"]
    class_column = vis["class_column"]
    names = list(datasets)
    n = len(names)
    nrows, ncols = _panel_shape(n, pcfg["ncols"])

    # ------------------------------------------------ размеры панно в дюймах ----
    cell = float(pcfg["cell_size"])
    gap_x, gap_y = float(pcfg["gap_x"]), float(pcfg["gap_y"])
    left, right = float(pcfg["left"]), float(pcfg["right"])
    top, bottom = float(pcfg["top"]), float(pcfg["bottom"])
    fig_w = ncols * cell + (ncols - 1) * gap_x + left + right
    fig_h = nrows * cell + (nrows - 1) * gap_y + top + bottom

    fig = plt.figure(figsize=(fig_w, fig_h), dpi=int(pcfg["dpi"]),
                     facecolor=pcfg["facecolor"])

    # ------------------------------------------ внешний вид ячейки (из PAIR_VIS) --
    cell_vis = scatterplot.merge_config(PAIR_VIS, vis["style"])
    cell_vis["marginal"]["show"] = False           # в панно только точки
    font = {"family": cell_vis["font"].get("family"),
            "size": float(cell_vis["font"]["size"]["title"]),
            "color": cell_vis["axes"]["color"]}

    # ---------------------------------------------- общие пределы осей панно ----
    base = source if source is not None else datasets[names[0]]
    class_order = (list(dict.fromkeys(base[class_column].tolist()))
                   if class_column in base.columns else ["all"])

    xlim = ylim = None
    if vis["shared_limits"]:
        base_data = _panel_data(base, x_name, y_name, class_column, class_order)
        xlim, ylim = scatterplot.compute_bounds(base_data, None, None,
                                                float(vis["margin"]))
        print(f"Общие пределы осей панно: X = [{xlim[0]:.2f}, {xlim[1]:.2f}], "
              f"Y = [{ylim[0]:.2f}, {ylim[1]:.2f}]")

    # --------------------------------------------------- ячейки панно 2 x 4 ----
    mode = str(pcfg["tick_labels"])
    cells = []
    for index, name in enumerate(names):
        col, row = index % ncols, index // ncols
        dataset = datasets[name]
        data = _panel_data(dataset, x_name, y_name, class_column, class_order)

        config = _engine_config(cell_vis, list(data.classes), None, x_name, y_name)
        config["figure"].update({"title": None, "xlabel": None, "ylabel": None,
                                 "legend": False, "colorbar": False})
        if xlim is not None:                          # одинаковый масштаб во всех клетках
            config["axes"]["xlim"], config["axes"]["ylim"] = xlim, ylim
            config["axes"]["square_data_area"] = False
        if mode == "all":
            show_labels = (True, True)
        elif mode == "none":
            show_labels = (False, False)
        else:
            # "outer": подписи X только в нижнем ряду, подписи Y — только в левом столбце
            show_labels = (row == nrows - 1, col == 0)
        config["ticks"]["labels"] = any(show_labels)   # лишние оси прячем при отрисовке

        percent, n_del = report.get(name, (0, 0))
        # если в отчёте стоит None, считаем число пропусков в самом клоне -- так
        # в подписи попадает именно число ЗАПОЛНЕННЫХ ячеек, а не удалённых
        if n_del is None:
            n_del = int(dataset[_deletion_columns(dataset, vis)].isna().to_numpy().sum())
        title = pcfg["cell_title"].format(
            name=name, percent=percent, n_del=n_del, objects=len(data),
            label=cell_labels.get(name, f"объектов {len(data)}"))
        rect = [(left + col * (cell + gap_x)) / fig_w,
                (bottom + (nrows - 1 - row) * (cell + gap_y)) / fig_h,
                cell / fig_w, cell / fig_h]
        ax, _scaffold, styles = _draw_panel_cell(fig, rect, data, config, font, title,
                                                 show_labels)
        cells.append((name, ax, styles))
        print(f"  панно, клетка {index + 1}/{n} ({name}): нарисовано объектов {len(data)}"
              + (f", строк отброшено из-за удалённых ячеек: {data.dropped}"
                 if data.dropped else ""))

    # ------------------------------------------- общие подписи осей и заголовок --
    label_size = float(cell_vis["font"]["size"]["label"])
    label_font = dict(family=font["family"], size=label_size, color=font["color"])
    if pcfg["axis_labels"]:
        fig.text(0.5, 0.008, x_name, ha="center", va="bottom", fontdict=label_font)
        fig.text(0.006, 0.5, y_name, ha="left", va="center", rotation=90,
                 fontdict=label_font)
    if pcfg["show_title"]:
        fig.text(0.5, 1.0 - 0.06 / fig_h, pcfg["title"].format(x=x_name, y=y_name, k=n),
                 ha="center", va="top",
                 fontdict=dict(family=font["family"],
                               size=float(cell_vis["font"]["size"]["label"]) + 2,
                               weight="bold", color=font["color"]))

    # ------------------------------------------- одна общая легенда на панно ----
    if pcfg["legend"]:
        reference = next(styles for _n, _a, styles in cells if len(styles) > 0)
        _panel_legend(fig, [str(c) for c in reference], list(reference.values()),
                      _engine_config(cell_vis, list(reference), None, x_name, y_name),
                      pcfg["legend_loc"], pcfg["legend_size"])

    print(f"Панно {nrows} x {ncols} из {n} рисунков построено.")
    return fig


print("\n" + "=" * 60)
print("11. КЛОНИРОВАНИЕ ДАТАСЕТА С УДАЛЁННЫМИ ЯЧЕЙКАМИ + ПАННО SCATTERPLOT")
print("=" * 60)
print(f"Проценты удаляемых ячеек: {DELETION_VIS['percentages']}")
print(f"Пара признаков на панно: X = {DELETION_VIS['x']}, Y = {DELETION_VIS['y']}")

DF_SD, DELETION_REPORT = build_deletion_datasets(dfS00)
DELETION_FIG = plot_deletion_panel(DF_SD, report=DELETION_REPORT, source=dfS00)

plt.show()


# =====================================================================================
# 12. ЗАПОЛНЕНИЕ ПРОПУСКОВ МЕДИАНОЙ: ПАННО dfSdXXmed + HEATMAP КОРРЕЛЯЦИИ ПИРСОНА
# =====================================================================================
# Из каждого клона dfSdXX (в нём часть ячеек заменена на NaN) получается клон
# dfSdXXmed, в котором все пропуски заполнены медианой. Далее:
#   * для выбираемой пары признаков строится панно m x n из диаграмм рассеяния
#     по всем dfSdXXmed (тот же движок plots/scatterplot.py, что и в пункте 11);
#   * по всем dfSdXXmed выводится heatmap корреляции Пирсона в том же оформлении,
#     что и в пункте [1.5] (раздел 9 файла: annot, cmap="coolwarm", vmin=-1, vmax=1).
# =====================================================================================

IMPUTE_VIS = {
    # ------------------- чем заполняем пропуски -------------------
    # "clone"    -> медиана считается по каждому клону отдельно (внутри его столбца),
    # "original" -> медиана берётся из исходного dfS00
    "median_source": "clone",

    # ------------------- выбираемая пара признаков для панно -------------------
    "x": "A",
    "y": "B",

    # ------------------- внешний вид панно scatterplot ---------------------------
    "style": {"points": {"size": 11.0, "alpha": 0.45, "size_auto_max": 11.0},
              "grid": {"alpha": 0.18}},
    "panel": {
        # в заполненных клонах пропусков нет, поэтому в клетке сообщается, сколько
        # ячеек было ЗАПОЛНЕНО медианой, а сколько удалено
        "cell_title": "{name} · удалено {percent} %\n"
                      "заполнено медианой: {n_del} яч. · объектов {objects}",
    },

    # ------------------- heatmap корреляции Пирсона -------------------
    "heatmap": {
        "columns": None,            # None -> все числовые признаки
        "cbar": "shared",           # "shared" | "each"
        "cell_size": 3.6,           # сторона одной heatmap, дюймы
        "gap": 0.35,                # зазор между heatmap, дюймы
        "title": "{name}\nудалено {percent} % ячеек",
        "title_size": 11.0,
        "show_title": True,
        "show_values": True,        # подписи коэффициентов внутри ячеек
        "value_format": ".2f",
        "cmap": "coolwarm",
        "vmin": -1.0,
        "vmax": 1.0,
        "font_size": 10.0,
        "suptitle": "Корреляция Пирсона после заполнения пропусков медианой",
    },
}


# -------------------------------------------------------------------------------------
# 12.1. ЗАПОЛНЕНИЕ ПРОПУСКОВ МЕДИАНОЙ
# -------------------------------------------------------------------------------------
def impute_median(dataset, columns, reference=None):
    """Заполнить пропуски медианой по столбцам.

    Параметры
    ----------
    dataset : pandas.DataFrame
        Клон с пропусками (NaN).
    columns : list
        Столбцы, в которых ищутся и заполняются пропуски.
    reference : pandas.DataFrame, optional
        Если задана, медианы берутся из этого (исходного) датасета, а не из клона.

    Returns
    -------
    (копия с заполненными пропусками, Series медиан)
    """
    out = dataset.copy()
    if reference is not None:
        medians = reference[columns].median()
    else:
        medians = out[columns].median()
    out[columns] = out[columns].fillna(medians)
    return out, medians


def build_imputed_datasets(datasets, vis=None, source=None):
    """Создать из клонов ``dfSdXX`` клоны ``dfSdXXmed`` с пропусками, заполненными медианой.

    Параметры
    ----------
    datasets : dict
        ``имя клона -> DataFrame`` (как возвращает :func:`build_deletion_datasets`).
    vis : dict, optional
        Настройки задания; по умолчанию ``IMPUTE_VIS``.
    source : pandas.DataFrame, optional
        Исходный датасет — нужен при ``median_source == "original"`` и для печати
        числа заполненных ячеек.

    Returns
    -------
    (datasets_med, filled), где ``datasets_med`` — ``имя клона + "med" -> DataFrame``,
    а ``filled`` — ``имя -> число заполненных ячеек``.
    """
    vis = IMPUTE_VIS if vis is None else vis
    columns = _deletion_columns(
        source if source is not None else next(iter(datasets.values())), DELETION_VIS)

    datasets_med, filled = {}, {}
    print(f"Заполнение пропусков медианой (источник медианы: {vis['median_source']})")
    for name, dataset in datasets.items():
        reference = source if (vis["median_source"] == "original" and source is not None) else None
        clone, medians = impute_median(dataset, columns, reference)
        new_name = f"{name}med"
        datasets_med[new_name] = clone
        n_filled = int(dataset[columns].isna().to_numpy().sum())
        filled[new_name] = n_filled
        left = int(clone[columns].isna().to_numpy().sum())
        print(f"  {new_name}: заполнено {n_filled:5d} ячеек медианой, "
              f"осталось пропусков: {left}")
    return datasets_med, filled


# -------------------------------------------------------------------------------------
# 12.2. ПАННО SCATTERPLOT ПО ВСЕМ dfSdXXmed
# -------------------------------------------------------------------------------------
def plot_imputed_panel(datasets_med, vis=None, report=None, source=None, filled=None):
    """Панно ``m x n`` из диаграмм рассеяния по всем клонам ``dfSdXXmed``.

    Используется тот же построитель панно, что и в пункте 11, поэтому клетки
    совпадают по масштабу осей и внешнему виду; отличается только сам набор данных —
    в нём пропусков уже нет, поэтому в каждой клетке отрисовываются все объекты.

    Параметры
    ----------
    filled : dict, optional
        ``имя клона+"med" -> число заполненных ячеек`` (как возвращает
        :func:`build_imputed_datasets`); попадает в подпись клетки.
    """
    vis = IMPUTE_VIS if vis is None else vis
    panel_vis = {
        **DELETION_VIS,
        "x": vis["x"],
        "y": vis["y"],
        "style": {**DELETION_VIS["style"], **vis["style"]},
        "panel": {**DELETION_VIS["panel"], **vis["panel"]},
    }
    # Отчёт переименовывается так же, как и датасеты: dfSdXX -> dfSdXXmed.
    # Второе поле — процент удалённых ячеек и число ячеек, ЗАПОЛНЕННЫХ медианой.
    filled = filled if filled is not None else {}
    report_med = {f"{name}med": (percent, filled.get(f"{name}med"))
                  for name, (percent, _) in (report or {}).items()}
    return plot_deletion_panel(datasets_med, vis=panel_vis, report=report_med,
                               source=source)


# -------------------------------------------------------------------------------------
# 12.3. HEATMAP КОРРЕЛЯЦИИ ПИРСОНА (оформление пункта [1.5])
# -------------------------------------------------------------------------------------
def plot_pearson_heatmap(data_subset, title_prefix, ax=None, vis=None, cbar=True):
    """Тепловая карта корреляции Пирсона в оформлении пункта [1.5].

    Оформление повторяет :func:`plot_correlation_heatmaps` (раздел 9): подписи
    коэффициентов внутри ячеек, палитра ``coolwarm``, фиксированный диапазон
    ``[-1, 1]``, квадратные ячейки.
    """
    vis = IMPUTE_VIS if vis is None else vis
    hcfg = vis["heatmap"]
    columns = hcfg["columns"] or _deletion_columns(data_subset, DELETION_VIS)
    corr = data_subset[columns].corr(method="pearson")

    if ax is None:
        _fig, ax = plt.subplots(figsize=(hcfg["cell_size"], hcfg["cell_size"]))
    sns.heatmap(
        corr,
        annot=bool(hcfg["show_values"]),
        fmt=hcfg["value_format"],
        cmap=hcfg["cmap"],
        vmin=float(hcfg["vmin"]),
        vmax=float(hcfg["vmax"]),
        ax=ax,
        square=True,
        cbar=bool(cbar),
    )
    for text in ax.texts:                      # подписи коэффициентов
        text.set_fontsize(float(hcfg["font_size"]))
    if hcfg["show_title"] and title_prefix:
        ax.set_title(title_prefix, fontsize=float(hcfg["title_size"]))
    return ax, corr


def plot_pearson_panel(datasets_med, vis=None, report=None, source=None):
    """Вывести heatmap корреляции Пирсона по каждому клону ``dfSdXXmed`` панно ``m x n``.

    Первым рисуется heatmap исходного датасета (если он передан) — она служит
    эталоном, с которым сравниваются заполненные клоны.
    """
    vis = IMPUTE_VIS if vis is None else vis
    hcfg = vis["heatmap"]
    report = report or {}

    # ------------------------------------------------- список тепловых карт --------
    panels = []
    if source is not None:
        panels.append(("исходный dfS00", source))
    for name, dataset in datasets_med.items():
        panels.append((name, dataset))

    n = len(panels)
    ncols = int(math.ceil(math.sqrt(n)))
    nrows = int(math.ceil(n / ncols))
    # при общей шкале резервируем отдельный узкий столбец под неё -- так карты
    # гарантированно не наезжают ни друг на друга, ни на шкалу
    shared = hcfg["cbar"] == "shared"
    cbar_space = 0.9 if shared else 0.0
    fig_w = (ncols * hcfg["cell_size"] + (ncols - 1) * hcfg["gap"] + 0.55 + cbar_space)
    fig, axes = plt.subplots(
        nrows, ncols,
        figsize=(fig_w, nrows * hcfg["cell_size"] + (nrows - 1) * hcfg["gap"] + 1.0),
        squeeze=False,
    )
    flat = list(axes.ravel())
    if hcfg["show_title"]:
        fig.suptitle(hcfg["suptitle"], fontsize=float(hcfg["title_size"]) + 1)

    # ------------------------------------------------- построение карт ------------
    reference_corr = None
    for i, (name, dataset) in enumerate(panels):
        percent = report.get(name.removesuffix("med"), (0, None))[0]
        title = (hcfg["title"].format(name=name, percent=percent)
                 if percent is not None else name)
        ax, corr = plot_pearson_heatmap(dataset, title, ax=flat[i], vis=vis,
                                        cbar=str(hcfg["cbar"]) == "each")
        if reference_corr is None:
            reference_corr = corr
        else:
            # насколько заполнение медианой исказило корреляцию относительно эталона
            delta = float(np.abs(corr.to_numpy() - reference_corr.to_numpy()).max())
            print(f"  {name}: max |Δr| относительно исходного датасета = {delta:.4f}")
    for j in range(n, len(flat)):              # лишние ячейки панно
        flat[j].set_visible(False)

    if shared:
        # Одна общая шкала на всю панно (диапазон у всех карт одинаковый: [-1, 1]).
        # Её ось добавляется ПОСЛЕ tight_layout в зарезервированное справа место --
        # так на неё не влияет пересчёт отступов, и карты не наезжают на шкалу.
        fig.tight_layout(rect=(0.0, 0.0, 1.0 - cbar_space / fig_w, 1.0))
        cax = fig.add_axes((1.0 - 0.50 / fig_w, 0.30, 0.14 / fig_w, 0.40))
        fig.colorbar(flat[0].collections[-1], cax=cax)
    else:
        fig.tight_layout()
    print(f"Heatmap корреляции Пирсона: панно {nrows} x {ncols} из {n} карт построена.")
    return fig, reference_corr


print("\n" + "=" * 60)
print("12. ЗАПОЛНЕНИЕ ПРОПУСКОВ МЕДИАНОЙ + HEATMAP КОРРЕЛЯЦИИ ПИРСОНА")
print("=" * 60)
print(f"Пара признаков для панно scatterplot: X = {IMPUTE_VIS['x']}, Y = {IMPUTE_VIS['y']}")

DF_SD_MED, FILLED_MED = build_imputed_datasets(DF_SD, source=dfS00)
MED_FIG = plot_imputed_panel(DF_SD_MED, report=DELETION_REPORT, source=dfS00,
                             filled=FILLED_MED)

print("Корреляция Пирсона по заполненным клонам:")
PEARSON_FIG, PEARSON_REF = plot_pearson_panel(DF_SD_MED, report=DELETION_REPORT,
                                              source=dfS00)

plt.show()


# =====================================================================================
# 13. СРАВНЕНИЕ МЕТОДОВ ЗАПОЛНЕНИЯ ПРОПУСКОВ НА ОДНОМ ИЗ ДАТАСЕТОВ dfSdXX
# =====================================================================================
# Пользователь выбирает один клон dfSdXX. Из него строятся четыре заполненных
# датасета -- dfSdXXmed, dfSdXXknn, dfSdXXreg, dfSdXXmice (медианы, kNN, регрессия,
# MICE). Затем:
#   * выводится heatmap корреляции Пирсона (оформление пункта [1.5]) для dfSd00,
#     выбранного dfSdXX и каждого из четырёх заполненных датасетов;
#   * по каждой из двух выбираемых пар признаков (исходно A+B и C+D) строится своё
#     панно m x n из диаграмм рассеяния -- по ячейке на каждый из шести датасетов.
# =====================================================================================

METHODS_VIS = {
    # ------------------- какой клон заполняем -------------------
    "clone": "dfSd20",           # любой из DF_SD; None -> берётся первый доступный
    "seed": 42,                  # воспроизводимость kNN и MICE

    # ------------------- две пары признаков для панено -------------------
    "pairs": [("A", "B"), ("C", "D")],
    "panel_titles": ["Пара признаков {x} и {y}: сравнение методов заполнения",
                     "Пара признаков {x} и {y}: сравнение методов заполнения"],

    # ------------------- методы заполнения -------------------
    "methods": {
        "med": {
            "label": "медианами",
            "enabled": True,
        },
        "knn": {
            "label": "kNN-заполнением",
            "enabled": True,
            "n_neighbors": 5,      # число соседей
            "weights": "distance",  # "uniform" | "distance"
        },
        "reg": {
            "label": "регрессионным заполнением",
            "enabled": True,
            "model": "linear",    # "linear" | "ridge" | "forest"
            "ridge_alpha": 1.0,
            "n_estimators": 100,  # для "forest"
        },
        "mice": {
            "label": "MICE-заполнением",
            "enabled": True,
            "n_iter": 10,         # число итераций EM
            "n_nearest_features": None,  # None -> в модель идут все признаки
            "initial_strategy": "mean",  # начальное приближение: "mean" | "median"
            "tol": 1e-4,
        },
    },

    # ------------------- heatmap корреляции Пирсона -------------------
    "heatmap": {
        **IMPUTE_VIS["heatmap"],           # оформление как в пункте [1.5]
        "cell_size": 3.2,
        "title": "{name}",
        "suptitle": "Корреляция Пирсона: сравнение методов заполнения пропусков",
    },

    # ------------------- внешний вид панено scatterplot -------------------
    "style": {"points": {"size": 11.0, "alpha": 0.45, "size_auto_max": 11.0},
              "grid": {"alpha": 0.18}},
    "panel": {
        "ncols": 3,                        # None -> подбирается автоматически
        "cell_size": 2.9,
        "tick_labels": "outer",
        "legend": False,
    },
}


# -------------------------------------------------------------------------------------
# 13.1. ЗАПОЛНЕНИЕ ПРОПУСКОВ РАЗНЫМИ МЕТОДАМИ
# -------------------------------------------------------------------------------------
def _impute_median_columns(dataset, columns, source=None):
    """Заполнение медианами по столбцам (тот же приём, что в пункте 12)."""
    out = dataset.copy()
    medians = (source if source is not None else out)[columns].median()
    out[columns] = out[columns].fillna(medians)
    return out


def _impute_knn(dataset, columns, cfg, seed=0):
    """kNN-заполнение: пропуск в признаке восстанавливается по соседям по остальным."""
    from sklearn.impute import KNNImputer

    out = dataset.copy()
    values = out[columns].to_numpy(dtype=float)
    imputer = KNNImputer(
        n_neighbors=int(cfg["n_neighbors"]),
        weights=str(cfg["weights"]),
    )
    out[columns] = imputer.fit_transform(values)
    return out


def _impute_regression(dataset, columns, cfg, seed=0):
    """Регрессионное заполнение: для каждого признака обучим модель по остальным.

    Строки без пропусков -- обучающая выборка; предсказание делается только для
    строк с пропуском в целевом признаке.
    """
    from sklearn.ensemble import RandomForestRegressor
    from sklearn.linear_model import LinearRegression, Ridge

    out = dataset.copy()
    # np.array с copy=True: to_numpy может вернуть массив, доступный только
    # для чтения, а ниже значения в нём заменяются на предсказанные
    values = np.array(out[columns].to_numpy(dtype=float), copy=True)
    for target in range(values.shape[1]):
        missing = np.isnan(values[:, target])
        if not missing.any():
            continue
        known = ~missing                                   # строки для обучения
        if known.sum() < 2:
            raise ValueError(
                f"недостаточно строк без пропусков в признаке {columns[target]!r} "
                "для регрессионного заполнения")

        model_name = str(cfg["model"])
        if model_name == "ridge":
            model = Ridge(alpha=float(cfg["ridge_alpha"]), random_state=seed)
        elif model_name == "forest":
            model = RandomForestRegressor(
                n_estimators=int(cfg["n_estimators"]), random_state=seed)
        else:
            model = LinearRegression()

        others = [c for c in range(values.shape[1]) if c != target]
        train_x = values[np.ix_(known, others)].copy()
        test_x = values[np.ix_(missing, others)].copy()
        # пропуски в прочих признаках закрываем медианами -- иначе модель
        # не сможет ни обучиться, ни сделать предсказание
        for col in range(len(others)):
            col_median = np.nanmedian(train_x[:, col])
            if not np.isfinite(col_median):
                col_median = 0.0
            train_x[np.isnan(train_x[:, col]), col] = col_median
            test_x[np.isnan(test_x[:, col]), col] = col_median

        model.fit(train_x, values[known, target])
        values[missing, target] = model.predict(test_x)

    out[columns] = values
    return out


def _impute_mice(dataset, columns, cfg, seed=0):
    """MICE: IterativeImputer -- набор регрессий, по очереди достраивающих признаки."""
    from sklearn.experimental import enable_iterative_imputer  # noqa: F401
    from sklearn.impute import IterativeImputer

    out = dataset.copy()
    values = out[columns].to_numpy(dtype=float)
    # начиная с sklearn 1.4 доля признаков в модели задаётся через
    # n_nearest_features (старое max_pct удалено); None -> все признаки
    n_nearest = cfg.get("n_nearest_features")
    imputer = IterativeImputer(
        max_iter=int(cfg["n_iter"]),
        n_nearest_features=(None if n_nearest in (None, 0)
                            else max(1, int(n_nearest))),
        tol=float(cfg["tol"]),
        initial_strategy=str(cfg.get("initial_strategy", "mean")),
        sample_posterior=False,
        random_state=seed,
    )
    out[columns] = imputer.fit_transform(values)
    return out


#: Имя метода -> функция заполнения. Аргументы во всех одинаковые.
IMPUTERS = {
    "med": lambda df, cols, cfg, seed: _impute_median_columns(df, cols),
    "knn": _impute_knn,
    "reg": _impute_regression,
    "mice": _impute_mice,
}


def build_filled_dataset(dataset, vis=None, source=None):
    """Создать заполненные версии одного клона ``dfSdXX`` всеми включёнными методами.

    Параметры
    ----------
    dataset : pandas.DataFrame
        Клон с пропусками (из ``DF_SD``).
    methods_vis : dict, optional
        Настройки задания; по умолчанию ``METHODS_VIS``.
    source : pandas.DataFrame, optional
        Исходный датасет: из него берутся медианы для метода ``med`` и пределы осей.

    Returns
    -------
    (filled, order, n_missing), где ``filled`` -- ``имя -> DataFrame`` с пропусками,
    ``order`` -- список имён в порядке построения, ``n_missing`` -- сколько ячеек
    было пропущено в исходном клоне.
    """
    vis = METHODS_VIS if vis is None else vis
    columns = _deletion_columns(
        source if source is not None else dataset, DELETION_VIS)
    n_missing = int(dataset[columns].isna().to_numpy().sum())

    filled, order = {}, []
    for name, cfg in vis["methods"].items():
        if not cfg.get("enabled", True) or name not in IMPUTERS:
            continue
        clone = IMPUTERS[name](dataset, columns, cfg, int(vis["seed"]))
        filled[name] = clone
        order.append(name)
        left = int(clone[columns].isna().to_numpy().sum())
        print(f"  метод {cfg['label']:<26} -> пропусков после заполнения: {left}")
    return filled, order, n_missing


# -------------------------------------------------------------------------------------
# 13.2. HEATMAP КОРРЕЛЯЦИИ ПИРСОНА ДЛЯ dfSd00, dfSdXX И ВСЕХ ЗАПОЛНЕННЫХ ДАТАСЕТОВ
# -------------------------------------------------------------------------------------
def plot_methods_pearson_panel(datasets, vis=None, source=None):
    """Панно heatmap'ов корреляции Пирсона: эталон, клон с пропусками и заполненные.

    Тонкая обёртка над :func:`plot_pearson_panel` (пункт 12.3) с настройками
    оформления из ``METHODS_VIS["heatmap"]``.

    Параметры
    ----------
    datasets : dict
        ``имя -> DataFrame`` -- что рисовать (порядок словаря сохраняется).
    source : DataFrame, optional
        Исходный ``dfS00``: если задан, он рисуется первым как эталон.

    Returns
    -------
    (figure, матрица корреляции эталонной карты)
    """
    vis = METHODS_VIS if vis is None else vis
    panels = {}
    if source is not None:
        panels["исходный dfS00"] = source
    panels.update(datasets)
    return plot_pearson_panel(panels, report=None, source=None,
                              vis={**IMPUTE_VIS, "heatmap": vis["heatmap"]})


# -------------------------------------------------------------------------------------
# 13.3. ПАННО SCATTERPLOT ДЛЯ КАЖДОЙ ИЗ ДВУХ ПАР ПРИЗНАКОВ
# -------------------------------------------------------------------------------------
def plot_methods_panels(datasets, vis=None, source=None, labels=None):
    """Построить по панно ``m x n`` из диаграмм рассеяния для каждой пары признаков.

    В каждой клетке -- один датасет: эталонный ``dfSd00``, клон с пропусками и все
    его заполненные версии. Пределы осей общие для всех клеток, поэтому их можно
    сравнивать напрямую.

    Параметры
    ----------
    datasets : dict
        ``имя -> DataFrame`` (порядок словаря задаёт порядок клеток).
    labels : dict, optional
        ``имя -> подпись`` для заголовка клетки (например, имя метода заполнения).

    Returns
    -------
    список рисунков-панно, по одному на каждую пару признаков
    """
    vis = METHODS_VIS if vis is None else vis
    labels = labels or {}
    figures = []
    pairs = [tuple(p) for p in vis["pairs"]]

    for index, (x_name, y_name) in enumerate(pairs):
        panel_vis = {
            **DELETION_VIS,
            "x": x_name,
            "y": y_name,
            "style": {**DELETION_VIS["style"], **vis["style"]},
            "panel": {**DELETION_VIS["panel"], **vis["panel"]},
        }
        panel_vis["panel"]["title"] = vis["panel_titles"][
            index % len(vis["panel_titles"])].format(x=x_name, y=y_name)

        # метки нужны только ради процента в отчёте, поэтому проставляем 0
        # labels[name] -- готовый текст второй строки подписи клетки, поэтому
        # метки кладём в шаблон cell_title, а не в процент удалённых ячеек
        panel_vis["panel"]["cell_title"] = "{name}\n{label}"
        figures.append(plot_deletion_panel(datasets, vis=panel_vis, report={},
                                           source=source, cell_labels=labels))
    return figures


# -------------------------------------------------------------------------------------
# 13.4. ПОСТРОЕНИЕ
# -------------------------------------------------------------------------------------
def run_methods_comparison(vis=None, source=None, clones=None, report=None):
    """Полный цикл: заполнение выбранного клона всеми методами, heatmap и два панно.

    Параметры
    ----------
    vis : dict, optional
        Настройки задания; по умолчанию ``METHODS_VIS``.
    source : DataFrame, optional
        Исходный датасет ``dfS00``.
    clones : dict, optional
        Набор клонов ``имя -> DataFrame`` (как ``DF_SD``), из которых выбирается
        клон для заполнения.
    report : dict, optional
        Отчёт по клонам (как ``DELETION_REPORT``) -- попадает в подписи панено.

    Returns
    -------
    dict с ключами:
        ``clone``    -- имя выбранного клона;
        ``dataset``  -- сам клон с пропусками (нужен для пункта [1.9]);
        ``filled``   -- ``имя+метод -> заполненный датасет``;
        ``heatmap``  -- рисунок heatmap корреляции Пирсона;
        ``panels``   -- список рисунков-панно, по одному на пару признаков;
        ``n_missing``-- сколько ячеек было заполнено.
    """
    vis = METHODS_VIS if vis is None else vis
    clones = clones or {}
    report = report or {}

    # ------------------------------------------------------ выбор клона -----------
    wanted = vis["clone"] or (next(iter(clones), None) if clones else None)
    if wanted is None:
        raise ValueError("нечего заполнять: не выбран ни один клон dfSdXX")
    if clones and wanted not in clones:
        raise ValueError(f"клона {wanted!r} нет среди датасетов "
                         f"{sorted(clones)}; выберите один из них в METHODS_VIS['clone']")
    if clones:
        dataset = clones[wanted]
        percent = report.get(wanted, (None, None))[0]
    else:
        dataset, percent = wanted, None

    columns = _deletion_columns(source if source is not None else dataset, DELETION_VIS)
    print(f"Выбранный клон: {wanted} (удалено {percent} % ячеек, "
          f"пропусков в признаках: {int(dataset[columns].isna().to_numpy().sum())})")

    # ------------------------------------------------------ заполнение ------------
    filled, order, n_missing = build_filled_dataset(dataset, vis=vis, source=source)

    # ------------------------------------------------------ подписи клеток ---------
    named = {f"{wanted}{name}": filled[name] for name in order}
    order_names = list(order)

    # ------------------------------------------- heatmap: dfSd00, клон, заполненные
    heatmap_datasets = {"dfSd00 (0 %)": source} if source is not None else {}
    heatmap_datasets[f"{wanted} ({percent} %, пропуски)"] = dataset
    heatmap_datasets.update(
        {f"{wanted}{name} — заполнено": named[f"{wanted}{name}"] for name in order_names})
    print("Строим heatmap корреляции Пирсона (оформление пункта [1.5]):")
    heatmap_fig, _ref = plot_pearson_panel(heatmap_datasets, report=None, source=None,
                                           vis={**IMPUTE_VIS,
                                                "heatmap": vis["heatmap"]})

    # ------------------------------------------------------ два панно scatterplot --
    panel_datasets = {}
    if source is not None:
        panel_datasets["dfSd00"] = source
    panel_datasets[f"{wanted} (пропуски)"] = dataset
    panel_datasets.update(named)

    cell_labels = {}
    if source is not None:
        cell_labels["dfSd00"] = f"исходный датасет, 0 %, объектов {len(source)}"
    cell_labels[f"{wanted} (пропуски)"] = f"{percent} %, пропуски не заполнены"
    cell_labels.update({f"{wanted}{name}": vis["methods"][name]["label"]
                        for name in order_names})
    print("Строим панно scatterplot для каждой пары признаков:")
    panels = plot_methods_panels(panel_datasets, vis=vis, source=source,
                                 labels=cell_labels)

    return {"clone": wanted, "dataset": dataset, "filled": named,
            "heatmap": heatmap_fig, "panels": panels, "n_missing": n_missing}


print("\n" + "=" * 60)
print("13. СРАВНЕНИЕ МЕТОДОВ ЗАПОЛНЕНИЯ ПРОПУСКОВ")
print("=" * 60)
print(f"Выбранный клон: {METHODS_VIS['clone']}")
print(f"Пары признаков: {METHODS_VIS['pairs']}")
print(f"Методы: {[cfg['label'] for cfg in METHODS_VIS['methods'].values()]}")

METHODS_RESULT = run_methods_comparison(source=dfS00, clones=DF_SD,
                                        report=DELETION_REPORT)
print(f"Пропусков было заполнено: {METHODS_RESULT['n_missing']} ячеек.")

plt.show()


# =====================================================================================
# 14. UPSET PLOT: ПРОПУСКИ И СОВПАДАЮЩИЕ ЗНАЧЕНИЯ ПО ПРИЗНАКАМ
# =====================================================================================
# Для каждого датасета из пункта [1.8] (исходный dfSd00, клон с пропусками и четыре
# его заполненные версии) строятся два UpSet plot:
#   (а) объекты с пропущенными значениями -- «варианты группировки пропусков»;
#   (б) объекты с совпадающими значениями -- «варианты группировки признаков».
#
# Оба построения сводятся к одному приёму: из датасета получается таблица индикаторов
# 0/1, где столбец признака равен 1, если объект относится к множеству этого
# признака. Множество = пропуск в признаке (а) либо значение признака, близкое к
# значению какого-либо другого признака той же строки (б). Далее upset.py считает
# размеры множеств и все их пересечения.
# =====================================================================================

UPSET_VIS = {
    # ------------------- признаки и допуск -------------------
    "columns": ["A", "B", "C", "D"],
    # Допуск для (б): |x_i - x_k| <= tolerance. На непрерывных данных точных
    # совпадений нет (0 объектов при tolerance = 0), поэтому допуск обязателен.
    "tolerance": 0.1,
    # Для справки печатается, сколько объектов совпало при разных допусках
    "tolerance_report": [0.0, 1e-9, 0.01, 0.05, 0.1, 0.25, 0.5],

    # ------------------- оформление UpSet plot -------------------
    "layout": "horizontal",      # "horizontal" | "vertical"
    "top_combinations": 15,      # сколько пересечений показывать
    "minimum_count": 1,          # rarer drop
    "color": "#4A1A6B",          # основной цвет (пропуски)
    "color_coincide": "#0B6E4F",  # основной цвет (совпадения)
    "dim_opacity": 0.30,
    "guide_line_opacity": 0.18,
    "dot_size": 160,
    "heading_missing": "UpSet: объекты с пропущенными значениями — {name}",
    "heading_coincide": "UpSet: объекты с совпадающими значениями — {name} (допуск {tol})",
}


# -------------------------------------------------------------------------------------
# 14.1. ТАБЛИЦЫ ИНДИКАТОРОВ ДЛЯ ДВУХ ВИДОВ ГРУППИРОВКИ
# -------------------------------------------------------------------------------------
def missing_indicators(dataset, columns):
    """Индикатор пропуска: 1, если в ячейке признака стоит NaN.

    Индекс строки сохраняется -- upset.py работает с индексами.
    """
    values = dataset[columns]
    return values.isna().astype(int)


def coincidence_indicators(dataset, columns, tolerance):
    """Индикатор совпадения: 1, если значение признака близко к значению
    какого-либо ДРУГОГО признака той же строки (|x_i - x_k| <= tolerance).

    Пересечение множеств тогда означает ровно то, что нужно заданию: объекты,
    у которых совпадают ВСЕ признаки выбранной группы.
    """
    values = dataset[columns].to_numpy(dtype=float)
    n_rows, n_cols = values.shape
    indicator = np.zeros((n_rows, n_cols), dtype=int)

    for i in range(n_rows):
        row = values[i]
        for j in range(n_cols):
            # пропуск не может совпасть ни с чем
            if not np.isfinite(row[j]):
                continue
            for k in range(n_cols):
                if k == j or not np.isfinite(row[k]):
                    continue
                if abs(row[j] - row[k]) <= tolerance:
                    indicator[i, j] = 1
                    break
    return pd.DataFrame(indicator, index=dataset.index, columns=columns)


# -------------------------------------------------------------------------------------
# 14.2. ПОСТРОЕНИЕ ОДНОГО UPSET PLOT
# -------------------------------------------------------------------------------------
def upset_from_indicators(indicators, heading, vis=None, kind="missing"):
    """Построить UpSet plot по таблице индикаторов 0/1.

    Индикаторы передаются в ``selection_method="nonzero"`` с огромным
    ``missing_limit``, поэтому в множество попадают ровно те строки, где
    соответствующий индикатор равен 1.

    Параметры
    ----------
    kind : str
        ``"missing"`` или ``"coincide"`` -- задаёт цвет из ``UPSET_VIS``
        (``color`` либо ``color_coincide``), чтобы два вида построения на экране
        различались.
    """
    vis = UPSET_VIS if vis is None else vis
    color_key = "color_coincide" if str(kind) == "coincide" else "color"
    figure, output = upset.build_upset_visualization(
        indicators,
        columns=list(vis["columns"]),
        selection_method="nonzero",
        missing_limit=10 ** 9,          # NaN в индикаторах не бывает
        top_combinations=int(vis["top_combinations"]),
        minimum_count=int(vis["minimum_count"]),
        layout=str(vis["layout"]),
        dim_opacity=float(vis["dim_opacity"]),
        guide_line_opacity=float(vis["guide_line_opacity"]),
        heading=heading,
        color=str(vis[color_key]),
        dot_size=float(vis["dot_size"]),
    )
    return figure, output


def print_upset_summary(output, label=None):
    """Напечатать размеры множеств и пересечения -- содержимое построенного графика."""
    sizes = output["column_sizes"]
    print(f"  [{label}] размеры множеств: " +
          ", ".join(f"{col} = {sizes[col]}" for col in sizes))
    total = sum(c["frequency"] for c in output["combinations"])
    print(f"  пересечений показано: {len(output['combinations'])}, "
          f"объектов в них: {total}")
    for entry in output["combinations"][:6]:
        groups = " ∩ ".join(entry["columns"])
        print(f"    {groups:<24} {entry['frequency']}")


# -------------------------------------------------------------------------------------
# 14.3. ОБА ВИДА ГРУППИРОВКИ ДЛЯ ОДНОГО ДАТАСЕТА
# -------------------------------------------------------------------------------------
def upset_for_dataset(dataset, name, vis=None):
    """Построить (а) UpSet по пропускам и (б) UpSet по совпадениям для одного датасета.

    Пустые множества (например, когда пропусков нет вовсе) -- штатный случай:
    такой UpSet plot невозможен, и вместо него печатается причина.
    """
    vis = UPSET_VIS if vis is None else vis
    figures, outputs = {}, {}

    # ------------------------------- (а) группировка пропусков --------------------
    n_missing = int(dataset[vis["columns"]].isna().to_numpy().sum())
    print(f"  пропусков всего: {n_missing}")
    if n_missing == 0:
        print("  (а) пропусков нет -- все множества пусты, UpSet plot не строится")
    else:
        figure, output = upset_from_indicators(
            missing_indicators(dataset, vis["columns"]),
            vis["heading_missing"].format(name=name),
            vis=vis, kind="missing",
        )
        print_upset_summary(output, f"{name} / пропуски")
        figures["missing"], outputs["missing"] = figure, output

    # ------------------------------- (б) группировка совпадений ------------------
    indicators = coincidence_indicators(dataset, vis["columns"],
                                        float(vis["tolerance"]))
    n_any = int(indicators.to_numpy().any(axis=1).sum())
    print(f"  объектов с хотя бы одной парой совпадающих значений: {n_any}")
    if n_any == 0:
        print(f"  (б) при допуске {vis['tolerance']} совпадений нет -- "
              "UpSet plot не строится")
    else:
        figure, output = upset_from_indicators(
            indicators,
            vis["heading_coincide"].format(name=name, tol=vis["tolerance"]),
            vis=vis, kind="coincide",
        )
        print_upset_summary(output, f"{name} / совпадения")
        figures["coincide"], outputs["coincide"] = figure, output

    return figures, outputs


# -------------------------------------------------------------------------------------
# 14.4. ПОСТРОЕНИЕ ПО ВСЕМ ДАТАСЕТАМ ПУНКТА [1.8]
# -------------------------------------------------------------------------------------
def run_upset_analysis(datasets, vis=None):
    """Построить оба вида UpSet plot для каждого датасета.

    Параметры
    ----------
    datasets : dict
        ``имя -> DataFrame``; порядок словаря задаёт порядок построения.

    Returns
    -------
    (figures, outputs): словари ``имя -> {"missing": fig, "coincide": fig}`` и
    ``имя -> {"missing": output, "coincide": output}``. Отсутствующий ключ означает,
    что множества оказались пустыми и график не строился.
    """
    vis = UPSET_VIS if vis is None else vis

    # ------------------------------------- (б) чувствительность к допуску ----------
    print("Сколько объектов имеют хотя бы одну пару совпадающих значений "
          "при разных допусках (для каждого датасета):")
    header = "  допуск:".ljust(12) + "".join(
        f"{name:>16}" for name in datasets)
    print(header)
    for tol in vis["tolerance_report"]:
        row = f"  {tol:<10g}"
        for dataset in datasets.values():
            hits = int(coincidence_indicators(
                dataset, vis["columns"], float(tol)).to_numpy().any(axis=1).sum())
            row += f"{hits:>16}"
        print(row)
    print(f"  (используется допуск {vis['tolerance']})")

    figures, outputs = {}, {}
    for name, dataset in datasets.items():
        print("\n" + "-" * 60)
        print(f"Датасет: {name}")
        print("-" * 60)
        figures[name], outputs[name] = upset_for_dataset(dataset, name, vis=vis)

    print("\n" + "=" * 60)
    print("UpSet plot построены.")
    print("=" * 60)
    return figures, outputs


#: Датасеты пункта [1.8]: исходный dfSd00, клон с пропусками и четыре заполненных.
UPSET_DATASETS = {"dfSd00": dfS00}
UPSET_DATASETS[METHODS_VIS["clone"]] = DF_SD[METHODS_VIS["clone"]]
for _method in METHODS_VIS["methods"]:
    _key = f"{METHODS_VIS['clone']}{_method}"
    if _key in METHODS_RESULT["filled"]:
        UPSET_DATASETS[_key] = METHODS_RESULT["filled"][_key]

print("\n" + "=" * 60)
print("14. UPSET PLOT: ПРОПУСКИ И СОВПАДАЮЩИЕ ЗНАЧЕНИЯ")
print("=" * 60)
print(f"Датасеты: {list(UPSET_DATASETS)}")
print(f"Допуск для совпадений: {UPSET_VIS['tolerance']}")

UPSET_FIGURES, UPSET_OUTPUTS = run_upset_analysis(UPSET_DATASETS)

plt.show()


# =====================================================================================
# 15. MNAR: ПРОПУСКИ, ЗАВИСЯЩИЕ ОТ САМИХ ЗНАЧЕНИЙ
# =====================================================================================
# В dfSdXX (пункт [1.7]) ячейки удалялись СЛУЧАЙНО -- пропуски не зависели от
# значений (Missing Completely At Random), поэтому их можно было честно оценить
# медианой или регрессией. Здесь строится dfMNAR: у каждого объекта удаляются все
# значения признаков C и D, которые МЕНЬШЕ признаковой медианы.
#
# Механизм пропуска зависит от значения: маленькие C и D исчезают всегда, большие
# не исчезают никогда. Восстановить их нечем -- информация утрачена безвозвратно,
# и это главный вывод, который должны показать пункты [1.8] и [1.9].
# =====================================================================================

MNAR_VIS = {
    # ------------------- какие признаки и в какую сторону обрезаются --------------
    "columns": ["C", "D"],        # признаки, значения которых удаляются
    "median_of": "column",        # "column" -> медиана каждого столбца отдельно
                                  # "original" -> медиана из исходного dfS00
    "side": "below",              # "below" -> удаляем значения < медианы
                                  # "above" -> удаляем значения > медианы
    "keep_equal": False,          # True -> значения РОВНО медианы остаются
    "name": "dfMNAR",
}


# -------------------------------------------------------------------------------------
# 15.1. ПОСТРОЕНИЕ dfMNAR
# -------------------------------------------------------------------------------------
def build_mnar_dataset(dataset, vis=None, source=None):
    """Клонировать датасет, удалив значения, зависящие от самих себя (MNAR).

    Для каждого признака из ``vis["columns"]`` удаляются те ячейки, чьё значение
    оказалось по нужную сторону от признаковой медианы. Именно зависимость
    пропуска от значения и делает механизм MNAR: большие значения не пропадают
    никогда, маленькие -- всегда, поэтому по наблюдаемой части распределение
    восстановить нельзя.

    Параметры
    ----------
    dataset : pandas.DataFrame
        Исходный датасет (обычно dfS00).
    vis : dict, optional
        Настройки задания; по умолчанию ``MNAR_VIS``.
    source : DataFrame, optional
        Нужен при ``median_of == "original"`` -- откуда берутся медианы.

    Returns
    -------
    (копия с пропусками, словарь ``признак -> (медиана, удалено, всего)``)
    """
    vis = MNAR_VIS if vis is None else vis
    out = dataset.copy()
    stats = {}

    for column in vis["columns"]:
        series = out[column]
        median = (source[column].median() if (vis["median_of"] == "original"
                                              and source is not None)
                  else series.median())

        # "below" -> удаляем всё, что строго ниже медианы (равные остаются),
        # "above" -> удаляем всё, что строго выше
        if vis["side"] == "below":
            mask = series < median if vis["keep_equal"] else series <= median
        else:
            mask = series > median if vis["keep_equal"] else series >= median

        out.loc[mask, column] = np.nan
        stats[column] = (float(median), int(mask.sum()), int(series.size))
    return out, stats


# -------------------------------------------------------------------------------------
# 15.2. ПУНКТ [1.8] ДЛЯ dfMNAR
# -------------------------------------------------------------------------------------
def run_mnar_methods(dataset, vis=None, source=None):
    """Выполнить пункт [1.8] для dfMNAR: заполнить четырьмя методами, heatmap, панно."""
    vis = METHODS_VIS if vis is None else vis
    mnar_vis = {**vis, "clone": dataset.attrs.get("name", MNAR_VIS["name"])}

    # report нужен только для процента в подписях; считаем фактическую долю
    columns = _deletion_columns(source if source is not None else dataset, DELETION_VIS)
    n_missing = int(dataset[columns].isna().to_numpy().sum())
    percent = round(100.0 * n_missing / (dataset[columns].size or 1), 2)
    report = {mnar_vis["clone"]: (percent, n_missing)}

    return run_methods_comparison(vis=mnar_vis, source=source,
                                  clones={mnar_vis["clone"]: dataset},
                                  report=report)


# -------------------------------------------------------------------------------------
# 15.3. ПУНКТ [1.9] ДЛЯ dfMNAR
# -------------------------------------------------------------------------------------
def collect_comparison_datasets(mnar_result, mnar_name, source):
    """Собрать шесть датасетов пункта [1.9] для MNAR-анализа."""
    datasets = {"dfSd00": source,
                f"{mnar_name} (пропуски)": mnar_result["dataset"]}
    for key, value in mnar_result["filled"].items():
        datasets[key] = value
    return datasets


print("\n" + "=" * 60)
print("15. MNAR: ПРОПУСКИ, ЗАВИСЯЩИЕ ОТ ЗНАЧЕНИЙ")
print("=" * 60)
print(f"Удаляются значения {'<' if MNAR_VIS['side'] == 'below' else '>'} медианы "
      f"признаков {MNAR_VIS['columns']}")

dfMNAR, MNAR_STATS = build_mnar_dataset(dfS00, source=dfS00)
dfMNAR.attrs["name"] = MNAR_VIS["name"]

print("\nЧто удалено (механизм пропуска зависит от значения!):")
_mnar_missing = 0
_mnar_total = 0
for _column, (_median, _removed, _size) in MNAR_STATS.items():
    print(f"  {_column}: медиана {_median:+.4f}, удалено {_removed} из {_size} "
          f"({100.0 * _removed / _size:.1f} %)")
    _mnar_missing += _removed
    _mnar_total += _size
print(f"  всего удалено {_mnar_missing} из {_mnar_total} ячеек "
      f"({100.0 * _mnar_missing / _mnar_total:.1f} %)")
print(f"  признаки A и B не тронуты: пропусков "
      f"{int(dfMNAR[['A', 'B']].isna().to_numpy().sum())}")
print(f"  строк хотя бы с одним пропуском: "
      f"{int(dfMNAR[['A', 'B', 'C', 'D']].isna().any(axis=1).sum())} из {len(dfMNAR)}")

# ------------------------------- [1.8] для dfMNAR --------------------------------
print("\n" + "-" * 60)
print("Пункт [1.8] для dfMNAR: четыре метода заполнения")
print("-" * 60)
MNAR_RESULT = run_mnar_methods(dfMNAR, source=dfS00)
print(f"Пропусков заполнено: {MNAR_RESULT['n_missing']} ячеек.")

# ------------------------------- [1.9] для dfMNAR --------------------------------
print("\n" + "-" * 60)
print("Пункт [1.9] для dfMNAR: UpSet по пропускам и совпадениям")
print("-" * 60)
MNAR_DATASETS = collect_comparison_datasets(MNAR_RESULT, MNAR_VIS["name"], dfS00)
MNAR_FIGURES, MNAR_OUTPUTS = run_upset_analysis(MNAR_DATASETS)

print("\n" + "=" * 60)
print("MNAR-АНАЛИЗ ЗАВЕРШЁН")
print("=" * 60)
print(f"Датасеты для UpSet: {list(MNAR_DATASETS)}")

plt.show()


print("UpSet по пропускам строится только для клона с пропусками: у остальных "
      "датасетов их нет вовсе (в dfSd00 их не было изначально, "
      "а в заполненных версиях они были восстановлены).")
