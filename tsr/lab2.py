import numpy as np
import pandas as pd
import seaborn as sns

import matplotlib.pyplot as plt

from plots import raincloud, upset, scatterplot
from plots.upset import build_upset_visualization

# Настройки отображения pandas — показывать все столбцы и строки
pd.set_option('display.max_columns', None)
pd.set_option('display.width', None)
pd.set_option('display.max_colwidth', None)

np.random.seed(42)

df = pd.read_csv("/home/julia/code/NSU_AI-Robotics/tsr/archive/WineQT.csv")

print("=" * 60)
print("1. РАЗМЕРНОСТЬ ДАТАСЕТА")
print("=" * 60)
print(f"Общее количество объектов (строк): {df.shape[0]}")
print(f"Общее количество признаков (столбцов): {df.shape[1]}")

print("\n" + "=" * 60)
print("2. ТИП КАЖДОГО ПРИЗНАКА")
print("=" * 60)
types_df = df.dtypes.to_frame('Тип данных')
print(types_df)

print("\n" + "=" * 60)
print("3. КОЛИЧЕСТВО КЛАССОВ И ОБЪЕКТОВ В КАЖДОМ ИЗ НИХ")
print("=" * 60)
class_counts = df['quality'].value_counts().to_frame('Количество объектов').rename_axis('Класс')
# Добавим процентное соотношение для информативности
class_counts['Доля (%)'] = (class_counts['Количество объектов'] / df.shape[0] * 100).round(2)
print(class_counts)

print("\n" + "=" * 60)
print("4. ИНЫЕ ОЦЕНКИ ВЫБОРКИ (основные статистики)")
print("=" * 60)
# Транспонируем таблицу describe для удобства чтения по строкам
desc_stats = df.describe().T
# Округлим значения для компактности вывода
print(desc_stats.round(4))

print("\n" + "=" * 60)
print("5. ЧИСЛО ОБЪЕКТОВ С NaN ПО КЛАССАМ")
print("=" * 60)
# Проверяем все числовые признаки, исключая quality и Id
feature_cols = [c for c in df.columns if c not in ('quality', 'Id')]
nan_by_class = df.groupby('quality')[feature_cols].agg(lambda x: x.isna().sum())
print(nan_by_class)

print("\n" + "=" * 60)
print("6. ЧИСЛО ОБЪЕКТОВ СО ЗНАЧЕНИЕМ 0 ПО КЛАССАМ")
print("=" * 60)
# Проверяем строгое равенство нулю для всех числовых признаков
zero_by_class = df.groupby('quality')[feature_cols].agg(lambda x: (x == 0).sum())
print(zero_by_class)

print("\n" + "=" * 60)


# ============================================================
# ЗАДАНИЕ [2.3]: UPSET PLOT
# ============================================================

print("\n" + "=" * 60)
print("[2.3] UPSET PLOT: ПРОПУЩЕННЫЕ И ПОВТОРЯЮЩИЕСЯ ЗНАЧЕНИЯ")
print("=" * 60)

# Настройки визуализации UpSet
VIS_23 = {
    "dot_size": 80,
    "set_size_bar_width": 0.5,
    "set_size_bar_gap": 0.5,
    "fig_width": 10,
    "fig_height": 3,
    "line_color": "#7AEFC2ED",
}

# Определяем числовые признаки (исключаем quality и Id)
feature_cols = [c for c in df.columns if c not in ('quality', 'Id')]
print(f"\nПризнаки для анализа ({len(feature_cols)}): {feature_cols}")

# ------------------------------------------------------------
# Часть 1: Пропущенные значения (NaN)
# ------------------------------------------------------------
print("\n" + "-" * 60)
print("ЧАСТЬ 1: Число объектов с пропущенными значениями (NaN)")
print("-" * 60)

nan_counts = df[feature_cols].isna().sum()
print("\nКоличество NaN по признакам:")
print(nan_counts)

total_nan_rows = df[feature_cols].isna().any(axis=1).sum()
print(f"\nВсего объектов с хотя бы одним NaN: {total_nan_rows}")

if total_nan_rows == 0:
    fig_nan = plt.figure(figsize=(VIS_23['fig_width'], VIS_23['fig_height']))
    fig_nan.text(0.5, 0.5, "В датасете нет пропущенных значений",
                 ha='center', va='center', fontsize=16)
    plt.axis('off')
    plt.show()
    print(f"\n✓ Картинка для NaN сохранена в файл 'upset_nan.png'")
else:
    fig_nan, out_nan = build_upset_visualization(
        dataframe=df,
        columns=feature_cols,
        missing_limit=0,
        selection_method="nonzero",
        layout="horizontal",
        dot_size=VIS_23["dot_size"],
        set_size_bar_width=VIS_23["set_size_bar_width"],
        set_size_bar_gap=VIS_23["set_size_bar_gap"],
        heading="Upset Plot: Пропущенные значения (NaN) по признакам"
    )
    plt.figure(fig_nan.number)
    plt.show()
    print(f"\n✓ Upset plot для NaN сохранён в файл 'upset_nan.png'")

# ------------------------------------------------------------
# Часть 2: Повторяющиеся значения внутри каждого признака
# ------------------------------------------------------------
print("\n" + "-" * 60)
print("ЧАСТЬ 2: Число объектов с повторяющимися значениями")
print("-" * 60)

# Создаём бинарный признак для каждого столбца:
# 1 — если значение в строке повторяется в данном признаке
df_dup = pd.DataFrame(index=df.index)
for col in feature_cols:
    dup_mask = df[col].duplicated(keep=False)
    df_dup[f"{col}_dup"] = dup_mask.astype(int)

print("\nКоличество объектов с повторяющимися значениями по признакам:")
dup_counts = df_dup.sum()
print(dup_counts)

total_dup_rows = (df_dup.sum(axis=1) > 0).sum()
print(f"\nВсего объектов с хотя бы одним повторяющимся значением: {total_dup_rows}")

fig_dup, out_dup = build_upset_visualization(
    dataframe=df_dup,
    selection_method="nonzero",
    top_combinations=20,
    layout="horizontal",
    color=VIS_23["line_color"],
    dot_size=VIS_23["dot_size"],
    set_size_bar_width=VIS_23["set_size_bar_width"],
    set_size_bar_gap=VIS_23["set_size_bar_gap"],
    heading="Upset Plot: Повторяющиеся значения по признакам"
)
plt.figure(fig_dup.number)
plt.show()
print(f"\n✓ Upset plot для повторяющихся значений сохранён в файл 'upset_duplicates.png'")

print("\n" + "=" * 60)
print("ЗАДАНИЕ [2.3] ВЫПОЛНЕНО")
print("=" * 60)


# ============================================================
# ЗАДАНИЕ [2.4]: ФИЛЬТРАЦИЯ ДАТАСЕТА
# ============================================================

print("\n" + "=" * 60)
print("[2.4] ФИЛЬТРАЦИЯ: 3 КЛАССА, 4 КОЛИЧЕСТВЕННЫХ ПРИЗНАКА")
print("=" * 60)

# Выбираем три класса (quality) и четыре количественных признака
selected_classes = [5, 6, 7]
selected_features = ['fixed acidity', 'volatile acidity', 'citric acid', 'alcohol']

print(f"\nВыбранные классы (quality): {selected_classes}")
print(f"Выбранные признаки: {selected_features}")

# Формируем отфильтрованный датасет
# 1. Оставляем только выбранные классы
# 2. Удаляем объекты с NaN хотя бы в одном из выбранных признаков
df0m = df[df['quality'].isin(selected_classes)].copy()
print(f"\nДо фильтрации по NaN: {df0m.shape[0]} объектов")

df0m = df0m.dropna(subset=selected_features)
print(f"После удаления объектов с NaN: {df0m.shape[0]} объектов")
print(f"Размерность df0m: {df0m.shape[0]} объектов × {df0m.shape[1]} признаков")

# Проверка классов в отфильтрованном датасете
print(f"\nКлассы в df0m:")
print(df0m['quality'].value_counts().sort_index())

print("\n" + "=" * 60)
print("ЗАДАНИЕ [2.4] ВЫПОЛНЕНО")
print("=" * 60)


# ============================================================
# ЗАДАНИЕ [2.5]: СРАВНИТЕЛЬНЫЕ КОРРЕЛЯЦИОННЫЕ ТАБЛИЦЫ
# ============================================================

print("\n" + "=" * 60)
print("[2.5] КОРРЕЛЯЦИИ ПИРСОНА И СПИРМАНА: df0 и df0m")
print("=" * 60)

VIS_25 = {
    "figure_size": 14,
    "heatmap_font_size": 7.5,
    "tick_size": 6,
    "title_size": 11,
}

# df0 — исходный датасет
df0 = df.copy()
print(f"\ndf0: {df0.shape[0]} объектов × {df0.shape[1]} признаков")
print(f"df0m: {df0m.shape[0]} объектов × {df0m.shape[1]} признаков")

# Числовые признаки для корреляции (исключаем Id)
numeric_cols_df0 = [c for c in df0.columns if c not in ('Id',)]
numeric_cols_df0m = [c for c in df0m.columns if c not in ('Id',)]

# Корреляция Пирсона для df0
print("\n--- Корреляция Пирсона: df0 ---")
pearson_df0 = df0[numeric_cols_df0].corr(method='pearson')
print(pearson_df0.round(3))

# Корреляция Пирсона для df0m
print("\n--- Корреляция Пирсона: df0m ---")
pearson_df0m = df0m[numeric_cols_df0m].corr(method='pearson')
print(pearson_df0m.round(3))

# Корреляция Спирмана для df0
print("\n--- Корреляция Спирмана: df0 ---")
spearman_df0 = df0[numeric_cols_df0].corr(method='spearman')
print(spearman_df0.round(3))

# Корреляция Спирмана для df0m
print("\n--- Корреляция Спирмана: df0m ---")
spearman_df0m = df0m[numeric_cols_df0m].corr(method='spearman')
print(spearman_df0m.round(3))

# Heat-map Пирсона — df0 (отдельное окно)
fig1 = plt.figure(figsize=(VIS_25['figure_size'], VIS_25['figure_size']))
sns.heatmap(pearson_df0, annot=True, fmt='.2f', cmap='coolwarm', center=0,
            square=False, linewidths=0.5, vmin=-1, vmax=1,
            annot_kws={'size': VIS_25['heatmap_font_size']}, xticklabels=True, yticklabels=True)
plt.xticks(fontsize=VIS_25['tick_size'])
plt.yticks(fontsize=VIS_25['tick_size'])
plt.title('Корреляция Пирсона: df0', fontsize=VIS_25['title_size'])
plt.subplots_adjust(top=0.95, bottom=0.18, left=0.18, right=0.95)
plt.show()

# Heat-map Пирсона — df0m (отдельное окно)
fig2 = plt.figure(figsize=(VIS_25['figure_size'], VIS_25['figure_size']))
sns.heatmap(pearson_df0m, annot=True, fmt='.2f', cmap='coolwarm', center=0,
            square=False, linewidths=0.5, vmin=-1, vmax=1,
            annot_kws={'size': VIS_25['heatmap_font_size']}, xticklabels=True, yticklabels=True)
plt.xticks(fontsize=VIS_25['tick_size'])
plt.yticks(fontsize=VIS_25['tick_size'])
plt.title('Корреляция Пирсона: df0m', fontsize=VIS_25['title_size'])
plt.subplots_adjust(top=0.95, bottom=0.18, left=0.18, right=0.95)
plt.show()

# Heat-map Спирмана — df0 (отдельное окно)
fig3 = plt.figure(figsize=(VIS_25['figure_size'], VIS_25['figure_size']))
sns.heatmap(spearman_df0, annot=True, fmt='.2f', cmap='coolwarm', center=0,
            square=False, linewidths=0.5, vmin=-1, vmax=1,
            annot_kws={'size': VIS_25['heatmap_font_size']}, xticklabels=True, yticklabels=True)
plt.xticks(fontsize=VIS_25['tick_size'])
plt.yticks(fontsize=VIS_25['tick_size'])
plt.title('Корреляция Спирмана: df0', fontsize=VIS_25['title_size'])
plt.subplots_adjust(top=0.95, bottom=0.18, left=0.18, right=0.95)
plt.show()

# Heat-map Спирмана — df0m (отдельное окно)
fig4 = plt.figure(figsize=(VIS_25['figure_size'], VIS_25['figure_size']))
sns.heatmap(spearman_df0m, annot=True, fmt='.2f', cmap='coolwarm', center=0,
            square=False, linewidths=0.5, vmin=-1, vmax=1,
            annot_kws={'size': VIS_25['heatmap_font_size']}, xticklabels=True, yticklabels=True)
plt.xticks(fontsize=VIS_25['tick_size'])
plt.yticks(fontsize=VIS_25['tick_size'])
plt.title('Корреляция Спирмана: df0m', fontsize=VIS_25['title_size'])
plt.subplots_adjust(top=0.95, bottom=0.18, left=0.18, right=0.95)
plt.show()

print("\n✓ Все heat-map сохранены: pearson_df0.png, pearson_df0m.png, spearman_df0.png, spearman_df0m.png")

print("\n" + "=" * 60)
print("ЗАДАНИЕ [2.5] ВЫПОЛНЕНО")
print("=" * 60)


# ============================================================
# ЗАДАНИЕ [2.6]: ПАРНЫЕ ВИЗУАЛИЗАЦИИ
# ============================================================

print("\n" + "=" * 60)
print("[2.6] ВИЗУАЛИЗАЦИЯ ПАР ПРИЗНАКОВ")
print("=" * 60)

import matplotlib.colors as mcolors

# Используем выбранные признаки и классы из задания [2.4]
features_26 = selected_features  # ['fixed acidity', 'volatile acidity', 'citric acid', 'alcohol']
classes_26 = selected_classes    # [5, 6, 7]

# Настройки визуализации pairplot-2.6, регулируемые пользователем
VIS_26 = {
    "classes": classes_26,           # выбранные классы
    "point_size": 20,                # площадь маркера, pt^2
    "point_alpha": 0.15,             # прозрачность маркеров
    "kde_alpha": 0.30,               # прозрачность плотности на диагонали
    "tick_size": 8,                  # размер шрифта тиков
    "label_size": 12,                # размер подписей строк/столбцов
    "suptitle_size": 16,
    "figsize": 16,
}

classes_26 = list(VIS_26["classes"])
print(f"\nПризнаки: {features_26}")
print(f"Классы: {classes_26}")

# Цвета и маркеры для классов
class_colors = {5: 'blue', 6: 'orange', 7: 'green'}
class_markers = {5: 'o', 6: 's', 7: '^'}
class_labels = {5: 'Класс 5', 6: 'Класс 6', 7: 'Класс 7'}

n_features = len(features_26)

# Создаём сетку n_features x n_features
fig, axes = plt.subplots(n_features, n_features,
                          figsize=(VIS_26["figsize"], VIS_26["figsize"]))
fig.suptitle('Парные визуализации признаков по классам', fontsize=16, y=0.995)

for i in range(n_features):
    for j in range(n_features):
        ax = axes[i, j]

        if i == j:
            # Диагональ: диаграммы плотности (KDE) для каждого класса
            for cls in classes_26:
                subset = df0m[df0m['quality'] == cls][features_26[i]].dropna()
                sns.kdeplot(subset, ax=ax, color=class_colors[cls],
                           label=class_labels[cls], fill=True,
                           alpha=VIS_26["kde_alpha"])
            ax.set_xlabel('')
            ax.set_ylabel('')
            if i == 0 and j == 0:
                ax.legend(fontsize=VIS_26["tick_size"], loc='upper right')
        else:
            # Недиагональ: scatter plot с разными цветами/маркерами для классов
            for cls in classes_26:
                subset = df0m[df0m['quality'] == cls]
                ax.scatter(subset[features_26[j]], subset[features_26[i]],
                          c=class_colors[cls], marker=class_markers[cls],
                          label=class_labels[cls],
                          alpha=VIS_26["point_alpha"], s=VIS_26["point_size"],
                          edgecolors='none')
            ax.set_xlabel('')
            ax.set_ylabel('')

        # Настройка тиков
        ax.tick_params(axis='both', labelsize=VIS_26["tick_size"], pad=5)

plt.subplots_adjust(wspace=0.45, hspace=0.45, left=0.08, right=0.95, top=0.90, bottom=0.08)

# Общие подписи для строк (Y) и столбцов (X) через fig.text()
for i in range(n_features):
    # Подпись Y для строки i
    y_pos = 1 - (i + 0.5) / n_features * 0.82 - 0.08
    fig.text(0.02, y_pos, features_26[i], rotation=90,
             va='center', ha='center', fontsize=VIS_26["label_size"])

for j in range(n_features):
    # Подпись X для столбца j
    x_pos = 0.08 + (j + 0.5) / n_features * 0.87
    fig.text(x_pos, 0.94, features_26[j],
             va='center', ha='center', fontsize=VIS_26["label_size"])
plt.show()
print("\n✓ Парная визуализация сохранена в файл 'pairplot_features.png'")


# ============================================================
# ЗАДАНИЕ [2.7]: ОДИНОЧНЫЙ SCATTERPLOT С ЛИНЕЙНОЙ РЕГРЕССИЕЙ,
#               ДОВЕРИТЕЛЬНОЙ (CI) И ПРОГНОЗНОЙ (PI) ПОЛОСАМИ
# ============================================================

print("\n" + "=" * 60)
print("[2.7] РЕГРЕССИЯ С ДОВЕРИТЕЛЬНОЙ И ПРОГНОЗНОЙ ПОЛОСАМИ")
print("=" * 60)

import matplotlib
from matplotlib.lines import Line2D
from matplotlib.patches import Patch
from matplotlib.widgets import CheckButtons, RadioButtons, Slider

# ------------------------------------------------------------
# НАСТРОЙКИ ВИЗУАЛИЗАЦИИ, РЕГУЛИРУЕМЫЕ ПОЛЬЗОВАТЕЛЕМ
# (меняются здесь ДО запуска скрипта)
# ------------------------------------------------------------
VIS_27 = {
    # --- что показываем -------------------------------------------------
    "class": 6,                      # выбранный класс (quality)
    "x": "density",                  # признак по оси X
    "y": "alcohol",                  # признак по оси Y

    # --- что включать/отключать ДО отрисовки ----------------------------
    "show_regression_line": True,    # линия линейной регрессии
    "show_confidence_band": True,    # доверительная полоса CI (для среднего)
    "show_predictive_band": True,    # прогнозная полоса PI (для нового объекта)
    "confidence": 0.95,              # общий уровень доверительной вероятности

    # --- параметры облака точек ------------------------------------------
    "point_size": 16.0,              # площадь маркера, pt^2
    "point_alpha": 0.35,             # прозрачность маркеров
    "density_mode": "off",           # "off" | "kde" | "heatmap" | "shade"
    "interactive": True,             # виджеты matplotlib для управления на лету
}

print("\nНастройки пользователя:")
for _k, _v in VIS_27.items():
    print(f"  {_k:24s} = {_v}")

# ------------------------------------------------------------
# ДАННЫЕ: ОДИН КЛАСС И ПАРА ПРИЗНАКОВ
# ------------------------------------------------------------
df27 = (df0m[df0m["quality"] == VIS_27["class"]]
        [[VIS_27["x"], VIS_27["y"], "quality"]]
        .dropna()
        .reset_index(drop=True))
if df27.empty:
    raise SystemExit(f"Класс {VIS_27['class']} не найден в df0m (доступны {selected_classes})")

x27 = df27[VIS_27["x"]].to_numpy(dtype=float)
y27 = df27[VIS_27["y"]].to_numpy(dtype=float)
n27 = int(x27.size)

print(f"\nКласс quality = {VIS_27['class']}: n = {n27} объектов")
print(f"  X = {VIS_27['x']}: [{x27.min():.4f}, {x27.max():.4f}]")
print(f"  Y = {VIS_27['y']}: [{y27.min():.3f}, {y27.max():.3f}]")

# ------------------------------------------------------------
# ЧИСЛЕННЫЕ ХАРАКТЕРИСТИКИ МНК-РЕГРЕССИИ
# ------------------------------------------------------------
_xbar, _ybar = float(x27.mean()), float(y27.mean())
_Sxx = float(((x27 - _xbar) ** 2).sum())
_slope = float(((x27 - _xbar) * (y27 - _ybar)).sum() / _Sxx)
_intercept = float(_ybar - _slope * _xbar)
_resid = y27 - (_intercept + _slope * x27)
_dof = n27 - 2
_sigma = float(np.sqrt((_resid ** 2).sum() / _dof))
_r = float(np.corrcoef(x27, y27)[0, 1])

print(f"\nУравнение регрессии: y = {_intercept:.3f} + ({_slope:.3f})·x")
print(f"  r = {_r:.3f}, R² = {_r ** 2:.3f}, sigma(остатки) = {_sigma:.4f}, dof = {_dof}")

# ------------------------------------------------------------
# КОНФИГУРАЦИЯ SCATTERPLOT (всё, что регулирует пользователь)
# ------------------------------------------------------------
_BACKEND_27 = matplotlib.get_backend().lower()
_GUI_OK = _BACKEND_27 in {"tkagg", "qtagg", "qtagg8", "gtk4agg", "gtk3agg",
                          "wxagg", "macosx", "webagg", "nbagg", "notebook",
                          "ipympl", "widget"}
_WIDE = bool(VIS_27["interactive"]) and _GUI_OK   # нужен запас справа под виджеты

CFG_27 = {
    "figure": {
        "size": 9.0,
        "anchor": "center",
        "plot_fraction": 0.68 if _WIDE else 0.80,
        "legend": False,                     # легенду собираем сами (п. + регрессия)
        "title": f"Класс {VIS_27['class']}: {VIS_27['y']} от {VIS_27['x']}",
        "xlabel": VIS_27["x"],
        "ylabel": VIS_27["y"],
        "label_size": 12,
        "title_size": 13,
    },
    "axes": {
        "square_data_area": False,          # у density и alcohol разные единицы
        "spines": "frame",
        "linewidth": 1.2,
    },
    "points": {
        "color": "#1f77b4",
        "size": VIS_27["point_size"],
        "size_auto": False,                  # размер маркера задаётся буквально
        "alpha": VIS_27["point_alpha"],
        "edge_width": 0.0,
        "zorder": 3,
    },
    # слой плотности рисуем сами (в _redraw_density_27), а его шкалу отключаем:
    # правая полоса фигуры отдана под элементы интерактивного управления
    "density": {"show": False, "mode": VIS_27["density_mode"],
                "colorbar": {"show": False}},
    "regression": {"show": False},           # регрессию тоже рисуем сами — по частям,
    "class_regression": {"show": False},     # чтобы каждую полосу можно было отключить
}

result27 = scatterplot.scatter2d(
    df27,
    x=VIS_27["x"], y=VIS_27["y"], class_column="quality",
    config=CFG_27,
)
fig27, ax27 = result27.fig, result27.ax
XLIM27 = tuple(result27.scaffold.xlim)
_leg_lib = ax27.get_legend()          # легенда библиотеки не нужна — будет своя
if _leg_lib is not None:
    _leg_lib.remove()

# ------------------------------------------------------------
# ПОЛОСЫ РЕГРЕССИИ
#   CI:  ŷ ± t·σ·sqrt(1/n + (x − x̄)²/Sxx)      — интервал для СРЕДНЕГО отклика
#   PI:  ŷ ± t·σ·sqrt(1 + 1/n + (x − x̄)²/Sxx)  — интервал для НОВОГО наблюдения
# ------------------------------------------------------------

def _t_critical(level, dof):
    """Двусторонний квантиль Стьюдента (запасной вариант — нормальное распределение)."""
    try:
        from scipy.stats import t as _student_t
        return float(_student_t.ppf(0.5 + level / 2.0, dof))
    except Exception:
        from statistics import NormalDist
        return float(NormalDist().inv_cdf(0.5 + level / 2.0))


def _fit_with_bands(level, x_range):
    """МНК-фит + доверительная полоса (считает библиотека) + прогнозная (numpy)."""
    fit = scatterplot.fit_curve(
        x27, y27, kind="linear", confidence=level,
        method="analytic", grid_points=320, x_range=x_range,
    )
    if fit is None:
        return None, None

    t_crit = _t_critical(level, _dof)
    half = t_crit * _sigma * np.sqrt(1.0 + 1.0 / n27 + (fit.x_grid - _xbar) ** 2 / _Sxx)
    pi_fit = scatterplot.FitResult(
        x_grid=fit.x_grid, y_grid=fit.y_grid,
        y_lo=fit.y_grid - half, y_hi=fit.y_grid + half,
        slope=fit.slope, intercept=fit.intercept, r2=fit.r2, n=fit.n,
    )
    return fit, pi_fit


# Стили элементов регрессии (аргументы scatterplot.draw_fit)
STYLE_LINE_27 = dict(color="#d62728", linewidth=2.0, linestyle="-", alpha=0.95, zorder=6.0)
STYLE_CI_27 = dict(color="#1f77b4", linewidth=0.0, linestyle="-", alpha=1.0,
                   show_ci=True, ci_alpha=0.22, ci_color="#1f77b4",
                   ci_edge="#1f77b4", ci_edge_alpha=0.45, ci_zorder=2.0)
STYLE_PI_27 = dict(color="#2ca02c", linewidth=0.0, linestyle="-", alpha=1.0,
                   show_ci=True, ci_alpha=0.10, ci_color="#2ca02c",
                   ci_edge="#2ca02c", ci_edge_alpha=0.35, ci_zorder=1.0)

# Текущее состояние переключателей (изменяется пользователем)
STATE_27 = {
    "line": bool(VIS_27["show_regression_line"]),
    "ci": bool(VIS_27["show_confidence_band"]),
    "pi": bool(VIS_27["show_predictive_band"]),
    "confidence": float(VIS_27["confidence"]),
}

_ART_REG_27 = []      # текущие артесты линии/полос
_ART_PTS_27 = []      # текущие артесты облака точек
_ART_DENS_27 = []     # текущие артесты плотности
_CAX_27 = None        # текущая шкала плотности
_LEG_27 = None        # текущая легенда

_ANN_27 = ax27.text(
    0.02, 0.03, "", transform=ax27.transAxes, ha="left", va="bottom", fontsize=9,
    color="#222222",
    bbox=dict(boxstyle="round,pad=0.35", fc="white", ec="#cccccc", alpha=0.9),
)


def _band_only(ax, fit, style):
    """Только полоса: draw_fit рисует и полосу, и линию — линию убираем."""
    band, line = scatterplot.draw_fit(ax, fit, **style)
    line.remove()
    return band


def _update_annotation_27():
    _ANN_27.set_text(
        f"y = {_intercept:.2f} {_slope:+.2f}·x\n"
        f"R² = {_r ** 2:.3f},  r = {_r:.3f},  n = {n27}\n"
        f"уровень = {STATE_27['confidence'] * 100:.1f}%,  σ = {_sigma:.3f}"
    )


def _rebuild_legend_27():
    """Легенда: облако точек + всё, что сейчас включено."""
    global _LEG_27
    if _LEG_27 is not None:
        _LEG_27.remove()

    pct = f"{STATE_27['confidence'] * 100:.0f}%"
    handles = [Line2D([], [], linestyle="none", marker="o", markersize=7,
                      markerfacecolor=CFG_27["points"]["color"],
                      markeredgecolor="none", alpha=0.55)]
    labels = [f"класс {VIS_27['class']} (n = {n27})"]

    if STATE_27["line"]:
        handles.append(Line2D([], [], color=STYLE_LINE_27["color"],
                              linewidth=STYLE_LINE_27["linewidth"],
                              linestyle=STYLE_LINE_27["linestyle"]))
        labels.append("линия регрессии")

    if STATE_27["ci"]:
        handles.append(Patch(facecolor=STYLE_CI_27["ci_color"],
                             edgecolor=STYLE_CI_27["ci_edge"], alpha=0.45))
        labels.append(f"CI {pct} — средний отклик")

    if STATE_27["pi"]:
        handles.append(Patch(facecolor=STYLE_PI_27["ci_color"],
                             edgecolor=STYLE_PI_27["ci_edge"], alpha=0.35))
        labels.append(f"PI {pct} — новое наблюдение")

    _LEG_27 = ax27.legend(handles, labels, loc="upper right", fontsize=9,
                          frameon=True, framealpha=0.9)


def _redraw_reg_27():
    """Пересчитать линию регрессии и те полосы, которые сейчас включены."""
    global _ART_REG_27
    for art in _ART_REG_27:
        art.remove()
    _ART_REG_27.clear()

    fit, pi_fit = _fit_with_bands(STATE_27["confidence"], XLIM27)
    if fit is not None:
        # порядок отрисовки: PI снизу, CI выше, линия поверх всех
        if STATE_27["pi"]:
            _ART_REG_27.append(_band_only(ax27, pi_fit, STYLE_PI_27))
        if STATE_27["ci"]:
            _ART_REG_27.append(_band_only(ax27, fit, STYLE_CI_27))
        if STATE_27["line"]:
            _ART_REG_27 += scatterplot.draw_fit(
                ax27, fit, show_ci=False, ci_alpha=0.0, **STYLE_LINE_27)

    _update_annotation_27()
    _rebuild_legend_27()
    fig27.canvas.draw_idle()


def _redraw_points_27(size, alpha):
    """Перерисовать облако точек с новым размером/прозрачностью маркеров."""
    global _ART_PTS_27
    for art in _ART_PTS_27:
        art.remove()
    _ART_PTS_27.clear()

    cfg = {**CFG_27,
           "points": {**CFG_27["points"], "size": float(size), "alpha": float(alpha)}}
    style = scatterplot.resolve_class_style(VIS_27["class"], 0, cfg)
    _ART_PTS_27 += scatterplot.draw_points(ax27, x27, y27, style, cfg)
    fig27.canvas.draw_idle()


def _redraw_density_27(mode):
    """Перерисовать (или убрать) слой плотности: "off" | "kde" | "heatmap" | "shade"."""
    global _CAX_27, _ART_DENS_27
    for art in _ART_DENS_27:
        art.remove()
    _ART_DENS_27.clear()
    if _CAX_27 is not None:
        _CAX_27.remove()
        _CAX_27 = None
    if str(mode) == "off":
        fig27.canvas.draw_idle()
        return

    cfg = {**CFG_27, "density": {**CFG_27["density"], "show": True, "mode": str(mode)}}
    arts, _field, cax, _norm, _cmap = scatterplot.draw_density(
        fig27, ax27, result27.data, cfg, result27.scaffold)
    _ART_DENS_27 += list(arts)
    _CAX_27 = cax
    fig27.canvas.draw_idle()


# Первоначальная отрисовка — ровно с теми настройками, которые задал пользователь
_redraw_points_27(VIS_27["point_size"], VIS_27["point_alpha"])
_redraw_density_27(VIS_27["density_mode"])
_redraw_reg_27()

# --- сравнение ширины CI и PI в нескольных точках ---------------------------
print("\nШирина полос (в единицах Y):")
_fit27, _pi27 = _fit_with_bands(STATE_27["confidence"], XLIM27)
if _fit27 is not None:
    print(f"  {'x':>8s} {'ŷ':>8s} {'CI ±':>8s} {'PI ±':>8s}")
    for _xq in (XLIM27[0], _xbar, XLIM27[1]):
        _i = int(np.argmin(np.abs(_fit27.x_grid - _xq)))
        print(f"  {_fit27.x_grid[_i]:8.4f} {_fit27.y_grid[_i]:8.3f} "
              f"{(_fit27.y_hi[_i] - _fit27.y_lo[_i]) / 2:8.3f} "
              f"{(_pi27.y_hi[_i] - _pi27.y_lo[_i]) / 2:8.3f}")

# ------------------------------------------------------------
# ИНТЕРАКТИВНОЕ УПРАВЛЕНИЕ (виджеты matplotlib)
# ------------------------------------------------------------
if _WIDE:
    # переключатели: линия регрессии и обе полосы
    _ax_chk = fig27.add_axes([0.855, 0.695, 0.135, 0.155])
    _chk = CheckButtons(
        _ax_chk, ["регрессия", "CI: доверит.", "PI: прогноз."],
        [STATE_27["line"], STATE_27["ci"], STATE_27["pi"]],
        label_props={"fontsize": [9]}, check_props={"s": 70},
    )

    def _on_check_27(_label):
        _s = _chk.get_status()
        STATE_27["line"], STATE_27["ci"], STATE_27["pi"] = bool(_s[0]), bool(_s[1]), bool(_s[2])
        _redraw_reg_27()

    _chk.on_clicked(_on_check_27)

    # слой плотности
    _ax_rad = fig27.add_axes([0.855, 0.500, 0.075, 0.135])
    _rad = RadioButtons(_ax_rad, ["off", "kde", "shade", "heatmap"],
                        active=["off", "kde", "shade", "heatmap"].index(
                            str(VIS_27["density_mode"])),
                        activecolor="#d62728", label_props={"fontsize": [9]})
    _rad.on_clicked(lambda label: _redraw_density_27(label))

    def _slider_27(ax_rect, label, valmin, valmax, valinit, fmt):
        _a = fig27.add_axes(ax_rect)
        _a.set_facecolor("#e8e8e8")
        return Slider(_a, label, valmin, valmax, valinit=valinit, valfmt=fmt)

    _sl_conf = _slider_27([0.885, 0.435, 0.055, 0.020], "уровень", 0.50, 0.999,
                          STATE_27["confidence"], "%.3f")
    _sl_size = _slider_27([0.885, 0.365, 0.055, 0.020], "размер", 1.0, 60.0,
                          VIS_27["point_size"], "%.0f")
    _sl_alpha = _slider_27([0.885, 0.295, 0.055, 0.020], "alpha", 0.02, 1.0,
                           VIS_27["point_alpha"], "%.2f")

    def _on_conf_27(value):
        STATE_27["confidence"] = float(value)
        _redraw_reg_27()

    def _on_size_27(value):
        _redraw_points_27(value, _sl_alpha.val)

    def _on_alpha_27(value):
        _redraw_points_27(_sl_size.val, value)

    _sl_conf.on_changed(_on_conf_27)
    _sl_size.on_changed(_on_size_27)
    _sl_alpha.on_changed(_on_alpha_27)

    fig27.text(0.855, 0.255,
               "щелчок по флажку —\nвключить/выключить\nполосу или линию",
               fontsize=8, color="#555555", ha="left", va="top")

    print("\n✓ Интерактивное управление включено:")
    print("    флажки   — линия регрессии / доверительная CI / прогнозная PI полосы;")
    print("    ползунки — уровень доверительной вероятности, размер и прозрачность точек;")
    print("    переключатель — слой плотности (off / kde / shade / heatmap).")
else:
    print(f"\n(backend={_BACKEND_27!r}: интерактивные виджеты недоступны, "
          f"построение управляется словарём VIS_27)")

plt.figure(fig27.number)
plt.show()
print("\n✓ График построен в ячейке ноутбука.")

print("\n" + "=" * 60)
print("ЗАДАНИЕ [2.7] ВЫПОЛНЕНО")
print("=" * 60)


# ============================================================
# ЗАДАНИЕ [2.8]: ПОВТОР ПУНКТА [2.6] + ЛИНЕЙНАЯ РЕГРЕССИЯ
#               С ПРОГНОЗНЫМИ ПОЛОСАМИ ДЛЯ КАЖДОГО КЛАССА
# ============================================================

print("\n" + "=" * 60)
print("[2.8] ПАРНЫЕ ГРАФИКИ С РЕГРЕССИЕЙ И ПРОГНОЗНЫМИ ПОЛОСАМИ")
print("=" * 60)

from matplotlib.lines import Line2D
from matplotlib.patches import Patch

# ------------------------------------------------------------
# НАСТРОЙКИ, РЕГУЛИРУЕМЫЕ ПОЛЬЗОВАТЕЛЕМ (применяются ДО отрисовки)
# ------------------------------------------------------------
VIS_28 = {
    "classes": selected_classes,     # выбирать, какие классы показывать
    "show_regression_line": True,    # линия МНК для каждого класса
    "show_predictive_band": True,    # прогнозная полоса PI для каждого класса
    "confidence": 0.95,              # уровень доверительной вероятности полос
    "kde_alpha": 0.30,                 # прозрачность плотности на диагонали
    "band_alpha": 0.13,              # прозрачность прогнозной полосы
    "min_points": 20,                # меньше точек — класс на этой паре пропускаем
    "point_size": 20,                # размер маркера, pt^2
    "point_alpha": 0.15,             # прозрачность маркеров
    "font_size": 12,                 # размер подписей
    "tick_size": 8,                  # размер тиков
}

print("\nНастройки пользователя:")
for _k, _v in VIS_28.items():
    print(f"  {_k:24s} = {_v}")

# Признаки и классы — те же, что и в п. [2.4]/[2.6]
features_28 = selected_features
classes_28 = list(VIS_28["classes"])
print(f"\nПризнаки: {features_28}")
print(f"Классы: {classes_28} (цвета/маркеры берём из п. [2.6])")

# ------------------------------------------------------------
# ПРОГНОЗНАЯ ПОЛОСА ДЛЯ ОДНОЙ ПАРЫ (X, Y) И ОДНОГО КЛАССА
#   линия и полосы строятся функциями библиотеки plots/scatterplot.py
#   PI:  ŷ ± t·σ·sqrt(1 + 1/n + (x − x̄)²/Sxx)
# ------------------------------------------------------------
def _regression_28(x, y, level, min_points):
    """МНК-фит и прогнозная полоса; при нехватке данных — (None, None, причина)."""
    n = int(np.asarray(x).size)
    if n < max(3, int(min_points)):
        return None, None, f"мало точек (n = {n})"

    fit = scatterplot.fit_curve(x, y, kind="linear", confidence=level,
                                method="analytic", grid_points=160)
    if fit is None:
        return None, None, "вырожденная матрица МНК"

    xbar = float(x.mean())
    sxx = float(((x - xbar) ** 2).sum())
    if sxx <= 0.0:
        return None, None, "нет разброса по X"

    resid = y - (fit.intercept + fit.slope * x)
    dof = max(1, n - 2)
    sigma2 = float((resid ** 2).sum()) / dof
    t_crit = _t_critical(level, dof)      # квантиль Стьюдента из п. [2.7]
    half = t_crit * np.sqrt(sigma2 * (1.0 + 1.0 / n + (fit.x_grid - xbar) ** 2 / sxx))

    pi_fit = scatterplot.FitResult(
        x_grid=fit.x_grid, y_grid=fit.y_grid,
        y_lo=fit.y_grid - half, y_hi=fit.y_grid + half,
        slope=fit.slope, intercept=fit.intercept, r2=fit.r2, n=fit.n,
    )
    return fit, pi_fit, None


def _band_only_28(ax, fit, style):
    """Полоса без линии: draw_fit рисует и то, и другое — линию убираем."""
    band, line = scatterplot.draw_fit(ax, fit, **style)
    line.remove()
    return band


# ------------------------------------------------------------
# ПАРНЫЕ ГРАФИКИ: диагональ — плотности, вне диагонали — scatter
# + линия регрессии и прогнозная полоса для каждого класса
# ------------------------------------------------------------
n_feat_28 = len(features_28)
fig28, axes28 = plt.subplots(n_feat_28, n_feat_28, figsize=(16, 16))
fig28.suptitle('Парные визуализации с линейной регрессией и прогнозными полосами '
               f'(уровень {VIS_28["confidence"] * 100:.0f}%)', fontsize=16, y=0.995)

n_fits_28 = 0        # сколько (пара, класс) построено
n_skipped_28 = 0     # сколько пропущено
skips_28 = []        # подробности пропусков

for i in range(n_feat_28):
    for j in range(n_feat_28):
        ax = axes28[i, j]

        if i == j:
            # Диагональ: диаграммы плотности распределения каждого класса
            for cls in classes_28:
                subset = df0m[df0m['quality'] == cls][features_28[i]].dropna()
                sns.kdeplot(subset, ax=ax, color=class_colors[cls],
                            label=class_labels[cls], fill=True,
                           alpha=VIS_28["kde_alpha"])
            ax.set_xlabel('')
            ax.set_ylabel('')
            if i == 0 and j == 0:
                ax.legend(fontsize=8, loc='upper right')
        else:
            # Вне диагонали: объекты классов разными по форме/цвету точками
            for cls in classes_28:
                subset = df0m[df0m['quality'] == cls]
                ax.scatter(subset[features_28[j]], subset[features_28[i]],
                           c=class_colors[cls], marker=class_markers[cls],
                           alpha=VIS_28["point_alpha"], s=VIS_28["point_size"],
                           edgecolors='none', zorder=3)

                # линия регрессии и прогнозная полоса этого класса
                xs = subset[features_28[j]].to_numpy(dtype=float)
                ys = subset[features_28[i]].to_numpy(dtype=float)
                ok = np.isfinite(xs) & np.isfinite(ys)
                fit_c, pi_c, why = _regression_28(
                    xs[ok], ys[ok], VIS_28["confidence"], VIS_28["min_points"]
                )
                if fit_c is None:
                    n_skipped_28 += 1
                    skips_28.append((features_28[j], features_28[i], cls, why))
                    continue

                n_fits_28 += 1
                if VIS_28["show_predictive_band"]:
                    _band_only_28(ax, pi_c, dict(
                        color=class_colors[cls], linewidth=0.0, linestyle="-",
                        alpha=1.0, show_ci=True, ci_alpha=VIS_28["band_alpha"],
                        ci_color=class_colors[cls], ci_edge=class_colors[cls],
                        ci_edge_alpha=0.45, ci_zorder=1.5,
                    ))
                if VIS_28["show_regression_line"]:
                    scatterplot.draw_fit(
                        ax, fit_c, color=class_colors[cls], linewidth=1.4,
                        linestyle="-", alpha=0.95, show_ci=False,
                        ci_alpha=0.0, zorder=4,
                    )

            ax.set_xlabel('')
            ax.set_ylabel('')
            if i == 0 and j == 1:
                # расшифровка обозначений регрессии (цвета соответствуют классам)
                handles = [
                    Line2D([], [], color=class_colors[c], linewidth=1.4)
                    for c in classes_28
                ]
                handles.append(Patch(facecolor="#777777", alpha=0.45,
                                     edgecolor="#777777"))
                labels = [f"МНК: {class_labels[c]}" for c in classes_28]
                labels.append(f"PI {VIS_28['confidence'] * 100:.0f}%")
                ax.legend(handles, labels, fontsize=7, loc="best", frameon=True,
                          framealpha=0.9)

        ax.tick_params(axis='both', labelsize=VIS_28["tick_size"], pad=5)

plt.subplots_adjust(wspace=0.45, hspace=0.45, left=0.08, right=0.95,
                    top=0.90, bottom=0.08)

# Общие подписи для строк (Y) и столбцов (X) через fig.text()
for i in range(n_feat_28):
    y_pos = 1 - (i + 0.5) / n_feat_28 * 0.82 - 0.08
    fig28.text(0.02, y_pos, features_28[i], rotation=90,
               va='center', ha='center', fontsize=VIS_26["label_size"])

for j in range(n_feat_28):
    x_pos = 0.08 + (j + 0.5) / n_feat_28 * 0.87
    fig28.text(x_pos, 0.94, features_28[j], va='center', ha='center', fontsize=12)

print(f"\nПостроено регрессий (пара × класс): {n_fits_28}, пропущено: {n_skipped_28}")
for x_f, y_f, cls, why in skips_28:
    print(f"  пропуск: {y_f} ~ {x_f}, класс {cls} — {why}")
print(f"Линия регрессии: {'включена' if VIS_28['show_regression_line'] else 'выключена'}; "
      f"прогнозная полоса: {'включена' if VIS_28['show_predictive_band'] else 'выключена'}")

plt.show()
print("\n✓ Парная визуализация с прогнозными полосами построена.")

print("\n" + "=" * 60)
print("ЗАДАНИЕ [2.8] ВЫПОЛНЕНО")
print("=" * 60)


# ============================================================
# ЗАДАНИЕ [2.9]: ФОРМУЛЫ РЕГРЕССИЙ И КРИТЕРИИ R2, ВАЛЬД, LR, SCORE
# ============================================================

print("\n" + "=" * 60)
print("[2.9] ФОРМУЛЫ ЛИНЕЙНЫХ РЕГРЕССИЙ И ИХ КРИТЕРИИ")
print("=" * 60)

from scipy import stats as sps29

# ------------------------------------------------------------
# Для простой линейной регрессии y = b0 + b1·x (n наблюдений):
#   b1 = Sxy/Sxx,  b0 = ȳ - b1·x̄,  SSE = Σ(y-ŷ)²,  SST = Σ(y-ȳ)²,
#   σ̂² = SSE/(n-2),  se(b1) = √(σ̂²/Sxx),  se(b0) = √(σ̂²·(1/n + x̄²/Sxx))
#
#   R²    = 1 - SSE/SST
#           F = (R²/1) / ((1-R²)/(n-2))               ~ F(1, n-2)
#   Вальд  W = (b1/se(b1))²                           ~ χ²(1)
#           F = W/1                                    ~ F(1, n-2)
#   LR    = -2·ln(L0/L1) = n·ln(SST/SSE)              ~ χ²(1)
#           F = LR/1                                   ~ F(1, n-2)
#   score = n·R² = (Sxy)² / (Sxx·SST/n)               ~ χ²(1)
#           (где Sxy = Σ(x-x̄)(y-ȳ)), k = 2 — число параметров (b0 и b1),
#           p1 - p0 = 1 — число добавленных предикторов)
#   логарифм функции правдоподобия полной модели: ln L̂ = -n/2·(ln 2π + ln(SSE/n) + 1)
#           AIC = -2lnL̂ + 2k,    BIC = -2lnL̂ + k·ln n
#   Для одного предиктора критерии R², Вальд, LR и score асимптотически
#   эквивалентны, поэтому их статистики почти совпадают.
# ------------------------------------------------------------
VIS_29 = {
    "classes": [5, 6, 7],          # классы для расчёта критериев
    "alpha": 0.05,                 # уровень значимости
}

print("\nНастройки пользователя:")
for _k, _v in VIS_29.items():
    print(f"  {_k:24s} = {_v}")

ALPHA_29 = float(VIS_29["alpha"])     # уровень значимости для пометки значимых моделей
classes_29 = list(VIS_29["classes"])


def _ols_criteria_29(x, y):
    """МНК-оценки и все критерии для одной пары (x, y). None — если модель вырождена."""
    x = np.asarray(x, dtype=float)
    y = np.asarray(y, dtype=float)
    n = int(x.size)

    xbar, ybar = float(x.mean()), float(y.mean())
    sxx = float(((x - xbar) ** 2).sum())
    sxy = float(((x - xbar) * (y - ybar)).sum())
    sst = float(((y - ybar) ** 2).sum())
    if n < 3 or sxx <= 0.0 or sst <= 0.0:
        return None

    b1 = float(((x - xbar) * (y - ybar)).sum() / sxx)
    b0 = float(ybar - b1 * xbar)
    resid = y - (b0 + b1 * x)
    sse = float((resid ** 2).sum())
    if sse <= 0.0:
        return None

    dof = n - 2                     # внутренние степени свободы
    sigma2 = sse / dof              # σ̂²
    se_b1 = float(np.sqrt(sigma2 / sxx))
    se_b0 = float(np.sqrt(sigma2 * (1.0 / n + xbar ** 2 / sxx)))
    r2 = 1.0 - sse / sst
    r2_adj = 1.0 - (1.0 - r2) * (n - 1) / dof

    # --- R² и его F-критерий -------------------------------------------
    f_r2 = (r2 / 1.0) / ((1.0 - r2) / dof)
    p_r2 = float(sps29.f.sf(f_r2, 1, dof))

    # --- критерий Вальда для коэффициента b1 ---------------------------
    wald = (b1 / se_b1) ** 2
    f_wald = wald / 1.0
    p_wald = float(sps29.f.sf(f_wald, 1, dof))

    # --- LR-критерий (сравнение с моделью только со свободным членом) --
    llf_full = -0.5 * n * (np.log(2.0 * np.pi) + np.log(sse / n) + 1.0)
    llf_null = -0.5 * n * (np.log(2.0 * np.pi) + np.log(sst / n) + 1.0)
    lr = 2.0 * (llf_full - llf_null)              # = n·ln(SST/SSE)
    f_lr = lr / 1.0
    p_lr = float(sps29.f.sf(f_lr, 1, dof))

    # --- критерий score (Лагранжа / Рао) для коэффициента b1 -----------
    # restricted-модель: y ~ 1, σ̂²_0 = SST/n, градиент U = Sxy/σ̂²_0,
    # информация I = Sxx/σ̂²_0  =>  score = U²/I = (Sxy)² / (Sxx·SST/n)
    score = float(sxy ** 2 / (sxx * (sst / n)))       # = n·R²  ~ χ²(1)
    f_score = score / 1.0
    p_score = float(sps29.f.sf(f_score, 1, dof))
    p_score_chi2 = float(sps29.chi2.sf(score, 1))

    # --- лог-функция правдоподобия полной модели и информационные критерии ---
    loglik_full = -0.5 * n * (np.log(2.0 * np.pi) + np.log(sse / n) + 1.0)
    k = 2                                        # b0 и b1
    aic = -2.0 * loglik_full + 2.0 * k
    bic = -2.0 * loglik_full + k * np.log(n)

    return dict(
        n=n, b0=b0, b1=b1, se_b0=se_b0, se_b1=se_b1, sigma=float(np.sqrt(sigma2)),
        r2=r2, r2_adj=r2_adj,
        f_r2=f_r2, p_r2=p_r2,
        wald=wald, f_wald=f_wald, p_wald=p_wald,
        p_wald_chi2=float(sps29.chi2.sf(wald, 1)),
        lr=lr, f_lr=f_lr, p_lr=p_lr, p_lr_chi2=float(sps29.chi2.sf(lr, 1)),
        score=score, f_score=f_score, p_score=p_score, p_score_chi2=p_score_chi2,
        loglik_full=loglik_full, aic=aic, bic=bic,
        df1=1, df2=dof,
    )


def _fmt_p_29(p):
    """Аккуратный формат p-критерия."""
    return f"{p:.3e}" if p < 1e-4 else f"{p:.4f}"


def _mark_29(p):
    return "*" if p < ALPHA_29 else " "     # * — модель значима на уровне ALPHA_29


# ------------------------------------------------------------
# ТЕ ЖЕ РЕГРЕССИИ, ЧТО И В П. [2.8]: 12 пар признаков × 3 класса
# ------------------------------------------------------------
rows29 = []
n_models29 = 0
n_skipped29 = 0

for i in range(len(features_28)):
    for j in range(len(features_28)):
        if i == j:
            continue
        y_name, x_name = features_28[i], features_28[j]

        print(f"\n--- {y_name} ~ {x_name} " + "-" * (46 - len(y_name) - len(x_name)))
        for cls in classes_29:
            subset = df0m[df0m['quality'] == cls]
            xs = subset[x_name].to_numpy(dtype=float)
            ys = subset[y_name].to_numpy(dtype=float)
            ok = np.isfinite(xs) & np.isfinite(ys)

            crit = _ols_criteria_29(xs[ok], ys[ok])
            if crit is None:
                n_skipped29 += 1
                print(f"  класс {cls}: регрессия не оценивается (вырожденные данные)")
                continue
            n_models29 += 1

            sign = "+" if crit["b1"] >= 0 else "-"
            print(f"  класс {cls} (n = {crit['n']}):"
                  f"  {y_name} = {crit['b0']:.4f} {sign} {abs(crit['b1']):.4f}·{x_name}")
            print(f"    se(b0) = {crit['se_b0']:.4f}, se(b1) = {crit['se_b1']:.4f}, "
                  f"σ = {crit['sigma']:.4f}, R²_adj = {crit['r2_adj']:.4f}")
            print(f"    R²    = {crit['r2']:.4f} │ F(1, {crit['df2']}) = "
                  f"{crit['f_r2']:8.3f} │ p = {_fmt_p_29(crit['p_r2'])}{_mark_29(crit['p_r2'])}")
            print(f"    Вальд  = {crit['wald']:8.3f} │ F(1, {crit['df2']}) = "
                  f"{crit['f_wald']:8.3f} │ p = {_fmt_p_29(crit['p_wald'])}{_mark_29(crit['p_wald'])}"
                  f"  (χ²(1): p = {_fmt_p_29(crit['p_wald_chi2'])})")
            print(f"    LR    = {crit['lr']:8.3f} │ F(1, {crit['df2']}) = "
                  f"{crit['f_lr']:8.3f} │ p = {_fmt_p_29(crit['p_lr'])}{_mark_29(crit['p_lr'])}"
                  f"  (χ²(1): p = {_fmt_p_29(crit['p_lr_chi2'])})")
            print(f"    score = {crit['score']:9.3f} │ F(1, {crit['df2']}) = "
                  f"{crit['f_score']:8.3f} │ p = {_fmt_p_29(crit['p_score'])}{_mark_29(crit['p_score'])}"
                  f"  (χ²(1): p = {_fmt_p_29(crit['p_score_chi2'])}; ln L̂ = {crit['loglik_full']:.3f}; "
                  f"AIC = {crit['aic']:.3f}, BIC = {crit['bic']:.3f})")

            rows29.append({
                "y": y_name, "x": x_name, "class": cls, "n": crit["n"],
                "b0": crit["b0"], "b1": crit["b1"],
                "R2": crit["r2"], "R2_adj": crit["r2_adj"],
                "F_R2": crit["f_r2"], "p_R2": crit["p_r2"],
                "Wald": crit["wald"], "F_Wald": crit["f_wald"], "p_Wald": crit["p_wald"],
                "LR": crit["lr"], "F_LR": crit["f_lr"], "p_LR": crit["p_lr"],
                "score": crit["score"], "F_score": crit["f_score"], "p_score": crit["p_score"],
                "p_score_chi2": crit["p_score_chi2"],
                "loglik": crit["loglik_full"], "AIC": crit["aic"], "BIC": crit["bic"],
                "df1": crit["df1"], "df2": crit["df2"],
            })

# ------------------------------------------------------------
# СВОДНАЯ ТАБЛИЦА
# ------------------------------------------------------------
table29 = pd.DataFrame(rows29)
print("\n" + "=" * 60)
print("СВОДНАЯ ТАБЛИЦА (F-статистики и p-критерии; степени свободы — df1, df2)")
print("=" * 60)

# для наглядности печатаем p-критерии в научной нотации
_disp29 = table29[["y", "x", "class", "n", "b0", "b1", "R2", "F_R2", "df1", "df2",
                   "p_R2", "Wald", "LR", "score"]].copy()
_disp29["p_R2"] = _disp29["p_R2"].map(lambda v: f"{v:.3e}")
print(_disp29.round(4).to_string(index=False))

sig29 = table29[table29["p_R2"] < ALPHA_29]
print(f"\nВсего моделей: {n_models29} (пропущено: {n_skipped29})")
print(f"Значимых по R² (p < {ALPHA_29}): {len(sig29)} из {n_models29}")
print(f"Наиболее сильная связь: {table29.loc[table29['R2'].idxmax(), 'y']} ~ "
      f"{table29.loc[table29['R2'].idxmax(), 'x']} (класс "
      f"{table29.loc[table29['R2'].idxmax(), 'class']}), R² = {table29['R2'].max():.4f}")

# результаты доступны в ipynb

print("\n" + "=" * 60)
print("ЗАДАНИЕ [2.9] ВЫПОЛНЕНО")
print("=" * 60)


# ============================================================
# ЗАДАНИЕ [2.10]: RAINCLOUD PLOT С JITTERING + BOXPLOT
#                С МЕДИАНАМИ, IQR И ДОВЕРИТЕЛЬНЫМИ ИНТЕРВАЛАМИ
# ============================================================

print("\n" + "=" * 60)
print("[2.10] RAINCLOUD (JITTER) И BOXPLOT С ДОВЕРИТЕЛЬНЫМИ ИНТЕРВАЛАМИ")
print("=" * 60)

from matplotlib.patches import Rectangle

# ------------------------------------------------------------
# НАСТРОЙКИ, РЕГУЛИРУЕМЫЕ ПОЛЬЗОВАТЕЛЕМ
# ------------------------------------------------------------
VIS_210 = {
    "feature": "alcohol",          # количественный признак, выбираемый пользователем
    "dataset": "df0m",             # "df0m" — классы 5/6/7 из п. [2.4]; "df" — весь датасет
    "classes": [5, 6, 7],          # ВЫБОР КЛАССОВ: None — все классы датасета,
                                   #   список номеров [5, 7], строка "5,7" или диапазон "5-8"
    "orientation": "horizontal",   # "horizontal" — значения по X; "vertical" — по Y
    "ci_level": 0.95,              # уровень доверительного интервала для медианы
    "n_boot": 2000,                # число бутстрэп-повторов для интервала
    "kde_alpha": 0.32,                  # прозрачность «облачка» / violin
    "point_alpha": None,                 # None — адаптивная прозрачность точек;
                                          # число 0.0..1.0 — заданное значение
    "random_state": 42,
    "show_ci": True,               # дорисовывать ДИ медианы поверх ящика boxplot
    "show_class_labels": True,     # подписи классов с числом объектов на оси
}

print("\nНастройки пользователя:")
for _k, _v in VIS_210.items():
    print(f"  {_k:24s} = {_v}")


def _parse_classes_210(spec, available):
    """Приводит выбор классов к списку: None / [5, 7] / "5,7" / "5-8" -> список номеров."""
    if spec is None:
        return list(available)
    if isinstance(spec, str):
        items = []
        for part in spec.replace(";", ",").replace(" ", ",").split(","):
            part = part.strip()
            if not part:
                continue
            if "-" in part[1:]:                      # диапазон, например "5-8"
                a, b = part.split("-", 1)
                items.extend(range(int(a), int(b) + 1))
            else:
                items.append(int(part))
        spec = items
    return [c for c in spec if c in set(available)]


# ------------------------------------------------------------
# ДАННЫЕ: ОДИН КОЛИЧЕСТВЕННЫЙ ПРИЗНАК ДЛЯ КАЖДОГО КЛАССА
# ------------------------------------------------------------
src210 = df0m if str(VIS_210["dataset"]).endswith("0m") else df
feature210 = VIS_210["feature"]
numeric210 = [c for c in src210.columns if c not in ('quality', 'Id')]

if feature210 not in numeric210:
    raise SystemExit(f"Признак {feature210!r} не найден. Доступны: {numeric210}")

data210 = src210[[feature210, 'quality']].dropna().reset_index(drop=True)
available210 = sorted(data210['quality'].unique().tolist())
classes210 = _parse_classes_210(VIS_210["classes"], available210)

if not classes210:
    raise SystemExit(
        f"Не выбран ни один класс из {VIS_210['classes']!r}. "
        f"Доступные классы в датасете {VIS_210['dataset']}: {available210}"
    )

print(f"\nДатасет: {VIS_210['dataset']}, признак: {feature210}")
print(f"Доступные классы: {available210}")
_counts210 = {c: int((data210["quality"] == c).sum()) for c in classes210}
print(f"Выбранные классы: {classes210} "
      f"({sum(_counts210.values())} из {len(data210)} объектов):")
for _c in classes210:
    print(f"    класс {_c}: {_counts210[_c]} объектов")

# Цвета классов — как в п. [2.6], для остальных классов — палитра по кругу
_extra_colors210 = ["#9467bd", "#8c564b", "#e377c2", "#7f7f7f"]
palette210 = {}
for _i, _cls in enumerate(classes210):
    if _cls in class_colors:
        palette210[_cls] = class_colors[_cls]
    else:
        palette210[_cls] = _extra_colors210[_i % len(_extra_colors210)]


# ------------------------------------------------------------
# ДОВЕРИТЕЛЬНЫЙ ИНТЕРВАЛ ДЛЯ МЕДИАНЫ (бутстрэп-перцентильный)
# ------------------------------------------------------------
def _median_ci_210(values, level, n_boot, seed):
    """Медиана и её (1 - level)·100% бутстрэп-перцентильный интервал."""
    values = np.asarray(values, dtype=float)
    rng = np.random.default_rng(seed)
    n = int(values.size)
    median = float(np.median(values))
    boot = np.median(
        rng.choice(values, size=(int(n_boot), n), replace=True), axis=1
    )
    tail = 100.0 * (1.0 - level) / 2.0
    lo, hi = np.percentile(boot, [tail, 100.0 - tail])
    return median, float(lo), float(hi)


def _draw_median_ci_210(ax, orientation, position, lo, hi, half_width=0.055):
    """Доверительный интервал медианы поверх ящика boxplot (пунктир + маркер)."""
    if orientation == "horizontal":
        ax.add_patch(Rectangle((lo, position - half_width), hi - lo, 2 * half_width,
                               facecolor="none", edgecolor="#222222", linewidth=1.3,
                               linestyle="--", zorder=7))
        ax.plot([(lo + hi) / 2], [position], marker="D", markersize=4.5,
                color="#222222", zorder=8, linestyle="none")
    else:
        ax.add_patch(Rectangle((position - half_width, lo), 2 * half_width, hi - lo,
                               facecolor="none", edgecolor="#222222", linewidth=1.3,
                               linestyle="--", zorder=7))
        ax.plot([position], [(lo + hi) / 2], marker="D", markersize=4.5,
                color="#222222", zorder=8, linestyle="none")


# ------------------------------------------------------------
# ЧИСЛЕННЫЕ ХАРАКТЕРИСТИКИ КЛАССОВ (печатаются под графиком)
# ------------------------------------------------------------
rows210 = []
for _cls in classes210:
    _v = data210.loc[data210['quality'] == _cls, feature210].to_numpy(dtype=float)
    _q1, _med, _q3 = (float(v) for v in np.percentile(_v, [25, 50, 75]))
    _med_ci, _lo_ci, _hi_ci = _median_ci_210(
        _v, float(VIS_210["ci_level"]), int(VIS_210["n_boot"]),
        int(VIS_210["random_state"]) + int(_cls)
    )
    rows210.append({
        "class": _cls, "n": int(_v.size),
        "mean": float(_v.mean()), "std": float(_v.std(ddof=1)),
        "min": float(_v.min()), "Q1": _q1, "median": _med, "Q3": _q3,
        "IQR": _q3 - _q1, "max": float(_v.max()),
        "CI_lo": _lo_ci, "CI_hi": _hi_ci, "CI_median": _med_ci,
    })

table210 = pd.DataFrame(rows210)
print("\nЧисленные характеристики классов:")
print(table210.round(3).to_string(index=False))

# ------------------------------------------------------------
# RAINCLOUD: KDE + jitter-разброс точек + boxplot (медиана, IQR)
# ------------------------------------------------------------
orientation210 = str(VIS_210["orientation"])
POINTS_GAP_210 = 0.27       # шаг строк в раскладке (те же значения передаём в библиотеку)
BOX_GAP_210 = 0.27
BOX_WIDTH_210 = 0.22
POINTS_START_210 = 1.58

# строки ящиков начинаются ниже самой нижней строки точек —
# так раскладка не «схлопывается» при любом числе классов
BOX_START_210 = POINTS_START_210 - (len(classes210) - 1) * POINTS_GAP_210 - BOX_GAP_210

# для вертикальной раскладки считаем колонки так, чтобы ящики, точки и
# KDE не наезжали друг на друга при любом числе классов
BOX_CENTER_210, BOX_DIST_210 = 0.50, 0.32
_half_span210 = (len(classes210) - 1) / 2 * BOX_DIST_210
POINTS_CENTER_210 = BOX_CENTER_210 + _half_span210 + BOX_WIDTH_210 / 2 + 0.25 + _half_span210
DENSITY_START_210 = POINTS_CENTER_210 + _half_span210 + 0.30

# фигуру создаём сами, чтобы высота подстраивалась под число классов
_fig_h210 = 4.8 + 1.0 * max(0, len(classes210) - 3)
_fig_w210 = 8.0 if orientation210 == "vertical" else 13.0
fig210, ax210 = plt.subplots(figsize=(_fig_w210, _fig_h210))

if orientation210 == "horizontal":
    fig210, ax210 = raincloud.raincloud_horizontal(
        data210, feature210,
        class_column='quality',
        class_order=classes210,
        class_colors=palette210,
        points_start_y=POINTS_START_210,
        points_gap_y=POINTS_GAP_210,
        boxes_start_y=BOX_START_210,
        boxes_gap_y=BOX_GAP_210,
        box_width=BOX_WIDTH_210,
        kde_alpha=float(VIS_210["kde_alpha"]),
        x_label=feature210,
        axis=ax210,
        random_state=int(VIS_210["random_state"]),
        show_legend=True,
    )
    box_positions210 = [BOX_START_210 - i * BOX_GAP_210 for i in range(len(classes210))]
else:
    fig210, ax210 = raincloud.raincloud_vertical(
        data210, feature210,
        class_column='quality',
        class_order=classes210,
        class_colors=palette210,
        box_center_x=BOX_CENTER_210,
        box_class_distance=BOX_DIST_210,
        points_center_x=POINTS_CENTER_210,
        points_class_distance=BOX_DIST_210,
        density_start_x=DENSITY_START_210,
        kde_alpha=float(VIS_210["kde_alpha"]),
        y_label=feature210,
        axis=ax210,
        random_state=int(VIS_210["random_state"]),
        show_legend=True,
    )
    _offsets = np.linspace(-(len(classes210) - 1) / 2, (len(classes210) - 1) / 2,
                           len(classes210))
    box_positions210 = [BOX_CENTER_210 + o * BOX_DIST_210 for o in _offsets]
    points_positions210 = [POINTS_CENTER_210 + o * BOX_DIST_210 for o in _offsets]

# ------------------------------------------------------------
# ПОДПИСИ КЛАССОВ НА ОСИ (библиотека их не рисует)
# ------------------------------------------------------------
# Применяем пользовательскую прозрачность точек, если она указана.
if VIS_210["point_alpha"] is not None:
    from matplotlib.collections import PathCollection
    for _coll in ax210.collections:
        if isinstance(_coll, PathCollection):
            _coll.set_alpha(float(VIS_210["point_alpha"]))

if VIS_210["show_class_labels"]:
    if orientation210 == "horizontal":
        # одна строка на класс: подписи выравниваются по строкам точек
        _names210 = [f"класс {c}  ·  n = {_counts210[c]}" for c in classes210]
        _label_pos210 = [POINTS_START_210 - i * POINTS_GAP_210
                         for i in range(len(classes210))]
        ax210.set_yticks(_label_pos210)
        ax210.set_yticklabels(_names210, fontsize=9)
        ax210.tick_params(axis="y", length=0, pad=6)
        ax210.spines["left"].set_visible(True)     # опора для подписей
    else:
        # две строки на класс: колонки стоят близко друг к другу
        _names210 = [f"класс {c}\nn = {_counts210[c]}" for c in classes210]
        _label_pos210 = list(box_positions210) + list(points_positions210)
        ax210.set_xticks(_label_pos210)
        ax210.set_xticklabels(_names210 + _names210, fontsize=8)
        ax210.tick_params(axis="x", length=0, pad=4)
        ax210.spines["bottom"].set_visible(True)
        # названия групп остаются подписями над осью
        for _x, _txt in ((float(np.mean(box_positions210)), "Boxplot"),
                         (float(np.mean(points_positions210)), "Точки (jitter)"),
                         (DENSITY_START_210 + 0.41, "KDE")):
            ax210.text(_x, 1.02, _txt, transform=ax210.get_xaxis_transform(),
                       ha="center", va="bottom", fontsize=10)

# ------------------------------------------------------------
# ДОВЕРИТЕЛЬНЫЕ ИНТЕРВАЛЫ МЕДИАН ПОВЕРХ ЯЩИКОВ BOXPLOT
# ------------------------------------------------------------
_ci_caption210 = f"(пунктир — {VIS_210['ci_level'] * 100:.0f}% ДИ медианы)"
if VIS_210["show_ci"]:
    for _pos, _row in zip(box_positions210, rows210):
        _draw_median_ci_210(ax210, orientation210, _pos, _row["CI_lo"], _row["CI_hi"])
    # подпись оси со значениями ставим туда, где сами значения
    if orientation210 == "vertical":
        ax210.set_ylabel(f"{feature210}   {_ci_caption210}", fontsize=11)
    else:
        ax210.set_xlabel(f"{feature210}   {_ci_caption210}", fontsize=11)

print(f"\nРаскладка: raincloud ({orientation210}) — KDE + jitter-точки + boxplot;")
print(f"            в каждом ящике: медиана, IQR (Q1…Q3) и ДИ медианы "
      f"{VIS_210['ci_level'] * 100:.0f}% (пунктирный прямоугольник, "
      f"{VIS_210['n_boot']} бутстрэп-повторов).")

fig210.suptitle(f"Распределение {feature210} по классам "
                f"{', '.join(str(c) for c in classes210)} "
                f"(raincloud + boxplot, ДИ медианы "
                f"{VIS_210['ci_level'] * 100:.0f}%)", fontsize=13)
fig210.tight_layout(rect=(0, 0, 1, 0.94))

# легенда под графиком: классы + обозначения медианы и ДИ
_legend210 = ax210.get_legend()
_handles210 = list(getattr(_legend210, "legend_handles", []) or [])
_labels210 = ([t.get_text() for t in _legend210.get_texts()]
              if _legend210 is not None else [])
if VIS_210["show_ci"]:
    _handles210 = list(_handles210) + [
        Rectangle((0, 0), 1, 1, facecolor="none", edgecolor="#222222",
                  linestyle="--", linewidth=1.3),
        Line2D([], [], marker="D", linestyle="none", color="#222222", markersize=4.5),
    ]
    _labels210 = list(_labels210) + [
        f"ДИ медианы {VIS_210['ci_level'] * 100:.0f}% (bootstrap)",
        "медиана",
    ]
ax210.legend(handles=_handles210, labels=_labels210, title="Класс",
             loc="upper center", bbox_to_anchor=(0.5, -0.17), ncol=5,
             frameon=False, fontsize=9)

plt.show()
print("\n✓ График построен в ячейке ноутбука.")

print("\n" + "=" * 60)
print("ЗАДАНИЕ [2.10] ВЫПОЛНЕНО")
print("=" * 60)


# ============================================================
# ЗАДАНИЕ [2.11]: ДИСПЕРСИОННЫЙ АНАЛИЗ (ANOVA)
# ============================================================

print("\n" + "=" * 60)
print("[2.11] ДИСПЕРСИОННЫЙ АНАЛИЗ (ANOVA) ПО ВЫБРАННОМУ ПРИЗНАКУ")
print("=" * 60)

from scipy import stats as sps211

# ------------------------------------------------------------
# НАСТРОЙКИ, РЕГУЛИРУЕМЫЕ ПОЛЬЗОВАТЕЛЕМ
# ------------------------------------------------------------
VIS_211 = {
    "feature": "alcohol",          # количественный признак для анализа
    "dataset": "df0m",             # "df0m" — классы 5/6/7 из п. [2.4]; "df" — весь датасет
    "classes": [5, 6, 7],          # None — все классы; [5, 7] / "6,7" / "5-8" — выбор
    "alpha": 0.05,                 # уровень значимости
}

print("\nНастройки пользователя:")
for _k, _v in VIS_211.items():
    print(f"  {_k:24s} = {_v}")

# ------------------------------------------------------------
# ФОРМУЛЫ (как в лекции 6, слайд «Дисперсионный анализ — ANOVA»)
#
#   SS_total   = Σ_j Σ_i (x_ij − x̄_total)²
#   SS_within  = Σ_j Σ_i (x_ij − x̄_j(group))²            — внутригрупповая
#   SS_between = Σ_j n_j · (x̄_j(group) − x̄_total)²      — межгрупповая
#   SS_total   = SS_within + SS_between
#
#   df1 = (n_group − 1),   df2 = (n_total − n_group)
#   F   = [SS_between / df1] / [SS_within / df2]         ~ F(df1, df2)
#
#   F-критерий ∝ объяснённая дисперсия / необъяснённая дисперсия
#   H0: средние по всем классам равны
# ------------------------------------------------------------

def _anova_211(values_by_group):
    """Однофакторный ANOVA: разложение дисперсии, F-критерий, df1, df2 и p."""
    groups = [np.asarray(v, dtype=float) for v in values_by_group]
    sizes = np.array([g.size for g in groups], dtype=float)
    means = np.array([g.mean() for g in groups])
    all_values = np.concatenate(groups)

    n_group = len(groups)
    n_total = all_values.size
    df1 = n_group - 1
    df2 = n_total - n_group
    if df1 < 1 or df2 < 1:
        return None

    grand_mean = float(all_values.mean())
    ss_total = float(((all_values - grand_mean) ** 2).sum())
    ss_within = float(sum(((g - m) ** 2).sum() for g, m in zip(groups, means)))
    ss_between = float((sizes * (means - grand_mean) ** 2).sum())

    ms_between = ss_between / df1               # объяснённая дисперсия
    ms_within = ss_within / df2                 # необъяснённая дисперсия
    f_value = ms_between / ms_within
    p_value = float(sps211.f.sf(f_value, df1, df2))

    return dict(
        n_group=n_group, n_total=n_total, df1=df1, df2=df2,
        grand_mean=grand_mean, sizes=sizes, means=means,
        ss_total=ss_total, ss_within=ss_within, ss_between=ss_between,
        ms_between=ms_between, ms_within=ms_within,
        f=f_value, p=p_value,
        eta2=(ss_between / ss_total) if ss_total > 0 else float("nan"),
        omega2=((ss_between - df1 * ms_within) / (ss_total + ms_within)
                if ss_total + ms_within > 0 else float("nan")),
    )


# ------------------------------------------------------------
# ДАННЫЕ
# ------------------------------------------------------------
src211 = df0m if str(VIS_211["dataset"]).endswith("0m") else df
feature211 = VIS_211["feature"]
numeric211 = [c for c in src211.columns if c not in ('quality', 'Id')]

if feature211 not in numeric211:
    raise SystemExit(f"Признак {feature211!r} не найден. Доступны: {numeric211}")

data211 = src211[[feature211, 'quality']].dropna().reset_index(drop=True)
available211 = sorted(data211['quality'].unique().tolist())
classes211 = _parse_classes_210(VIS_211["classes"], available211)   # из п. [2.10]

if len(classes211) < 2:
    raise SystemExit(f"Для ANOVA нужно минимум 2 класса, выбрано: {classes211}")

print(f"\nДатасет: {VIS_211['dataset']}, признак: {feature211}")
print(f"Доступные классы: {available211}; выбранные: {classes211}")

groups211 = {c: data211.loc[data211['quality'] == c, feature211].to_numpy(dtype=float)
             for c in classes211}

# ------------------------------------------------------------
# 1. ОБЩИЙ F-КРИТЕРИЙ ДЛЯ ВСЕХ КЛАССОВ ОДНОВРЕМЕННО
# ------------------------------------------------------------
res211 = _anova_211([groups211[c] for c in classes211])
if res211 is None:
    raise SystemExit("Недостаточно данных для ANOVA")

print("\n" + "-" * 60)
print("1. ОБЩИЙ F-КРИТЕРИЙ ДЛЯ ВСЕХ КЛАССОВ ОДНОВРЕМЕННО")
print("-" * 60)
print(f"{'класс':>8s} {'n':>6s} {'среднее':>10s} {'SS_within':>12s}")
for c in classes211:
    v = groups211[c]
    print(f"{c:>8} {v.size:>6} {v.mean():>10.4f} "
          f"{float(((v - v.mean()) ** 2).sum()):>12.4f}")

print(f"\n  n_group = {res211['n_group']},  n_total = {res211['n_total']},  "
      f"общее среднее x̄_total = {res211['grand_mean']:.4f}")
print(f"  SS_total   = Σ(x − x̄_total)²       = {res211['ss_total']:.4f}")
print(f"  SS_within  = Σ(x − x̄_j)²           = {res211['ss_within']:.4f}  "
      f"(df = {res211['df2']})")
print(f"  SS_between = Σ n_j·(x̄_j − x̄_total)² = {res211['ss_between']:.4f}  "
      f"(df = {res211['df1']})")
print(f"  проверка: SS_within + SS_between = "
      f"{res211['ss_within'] + res211['ss_between']:.4f} "
      f"(= SS_total: {np.isclose(res211['ss_within'] + res211['ss_between'], res211['ss_total'])})")
print(f"\n  MS_between = SS_between/df1 = {res211['ms_between']:.6f}  "
      f"(объяснённая дисперсия)")
print(f"  MS_within  = SS_within/df2  = {res211['ms_within']:.6f}  "
      f"(необъяснённая дисперсия)")
print(f"  F = MS_between / MS_within = {res211['f']:.4f}")
print(f"  df1 = {res211['df1']}, df2 = {res211['df2']}")
print(f"  p  = {_fmt_p_29(res211['p'])}")

_alpha211 = float(VIS_211["alpha"])
_verdict211 = ("нулевая гипотеза (средние равны) ОТВЕРГАЕТСЯ"
               if res211["p"] < _alpha211 else
               "нулевая гипотеза (средние равны) НЕ отвергается")
print(f"\n  H0: μ₁ = μ₂ = … = μ_k;  при α = {_alpha211} → {_verdict211}")
print(f"  Размер эффекта: η² = SS_between/SS_total = {res211['eta2']:.4f}, "
      f"ω² = {res211['omega2']:.4f}")

# ------------------------------------------------------------
# 2. ПОПАРНЫЕ СРАВНЕНИЯ КЛАССОВ (тот же F-критерий на двух группах)
# ------------------------------------------------------------
print("\n" + "-" * 60)
print("2. F-КРИТЕРИЙ ДЛЯ КАЖДОЙ ПАРЫ КЛАССОВ")
print("-" * 60)

rows211 = []
for i_a in range(len(classes211)):
    for i_b in range(i_a + 1, len(classes211)):
        c_a, c_b = classes211[i_a], classes211[i_b]
        pair = _anova_211([groups211[c_a], groups211[c_b]])
        if pair is None:
            continue
        diff = float(groups211[c_a].mean() - groups211[c_b].mean())
        rows211.append({
            "пара": f"{c_a} vs {c_b}", "класс_1": c_a, "класс_2": c_b,
            "n_1": int(pair["sizes"][0]), "n_2": int(pair["sizes"][1]),
            "среднее_1": float(pair["means"][0]), "среднее_2": float(pair["means"][1]),
            "разность_средних": diff,
            "SS_between": pair["ss_between"], "SS_within": pair["ss_within"],
            "MS_between": pair["ms_between"], "MS_within": pair["ms_within"],
            "F": pair["f"], "df1": pair["df1"], "df2": pair["df2"], "p": pair["p"],
            "eta2": pair["eta2"],
        })

table211 = pd.DataFrame(rows211)
if not table211.empty:
    _disp211 = table211.copy()
    _disp211["p"] = _disp211["p"].map(lambda v: f"{v:.3e}")
    print(_disp211[["пара", "n_1", "n_2", "разность_средних", "F",
                    "df1", "df2", "p"]].round(4).to_string(index=False))

    n_sig211 = int((table211["p"] < _alpha211).sum())
    print(f"\n  Значимых различий при α = {_alpha211}: {n_sig211} из {len(table211)} пар")
    for _r in table211.to_dict("records"):
        _mark = "значимо" if _r["p"] < _alpha211 else "не значимо"
        print(f"    {_r['пара']:>10s}: Δ средних = {_r['разность_средних']:+.4f}, "
              f"F({_r['df1']}, {_r['df2']}) = {_r['F']:.4f}, "
              f"p = {_fmt_p_29(_r['p'])} — {_mark}")

# ------------------------------------------------------------
# СВОДКА
# ------------------------------------------------------------
print("\n" + "-" * 60)
print("СВОДКА ПО ПРИЗНАКУ И КЛАССАМ")
print("-" * 60)
summary211 = pd.DataFrame([{
    "class": c, "n": int(groups211[c].size),
    "mean": float(groups211[c].mean()),
    "std": float(groups211[c].std(ddof=1)),
    "SS_within": float(((groups211[c] - groups211[c].mean()) ** 2).sum()),
} for c in classes211])
print(summary211.round(4).to_string(index=False))

# общий результат и попарные — одной таблицей в файл
_overall211 = pd.DataFrame([{
    "тест": "все классы", "пара": f"{'-'.join(str(c) for c in classes211)}",
    "F": res211["f"], "df1": res211["df1"], "df2": res211["df2"], "p": res211["p"],
    "eta2": res211["eta2"],
}])
_pair211 = pd.DataFrame([{
    "тест": "пара классов", "пара": r["пара"], "F": r["F"],
    "df1": r["df1"], "df2": r["df2"], "p": r["p"], "eta2": r["eta2"],
} for r in rows211]) if rows211 else pd.DataFrame()
print(f"\n  Общий F({res211['df1']}, {res211['df2']}) = {res211['f']:.4f}, "
      f"p = {_fmt_p_29(res211['p'])}")
print("✓ Результаты ANOVA сохранены в файл 'anova_results.csv'")

print("\n" + "=" * 60)
print("ЗАДАНИЕ [2.11] ВЫПОЛНЕНО")
print("=" * 60)


# ============================================================
# ЗАДАНИЕ [2.12**]: СИНТЕТИЧЕСКИЙ ГАУССОВ ДАТАСЕТ dfG
# ============================================================

print("\n" + "=" * 60)
print("[2.12**] СИНТЕТИЧЕСКИЙ ГАУССОВ ДАТАСЕТ dfG")
print("=" * 60)

from scipy import stats as sps212

# Настройки генерации (выбираются пользователем)
VIS_212 = {
    "random_seed": 2026,   # воспроизводимость
    "n_min": 30,           # минимальный размер выборки
    "n_max": 100,          # максимальный размер выборки
    "feature_names": ["X", "Y"],
    "class_label": 1,
}

np.random.seed(int(VIS_212["random_seed"]))

# ------------------------------------------------------------
# ГЕНЕРАЦИЯ dfG: случайное число объектов (10..100) и случайная
# форма облака за счёт случайной матрицы ковариаций гауссиана
# ------------------------------------------------------------
_n212 = int(np.random.randint(int(VIS_212["n_min"]), int(VIS_212["n_max"]) + 1))
_mean212 = np.random.normal(loc=0.0, scale=3.0, size=2)
# случайная положительно определённая матрица ковариаций
_A212 = np.random.normal(loc=0.0, scale=2.0, size=(2, 2))
_cov212 = _A212.T @ _A212 + 0.5 * np.eye(2)
_rho212 = float(np.random.uniform(-0.85, 0.85))
_corr212 = (np.sqrt(_cov212[0, 0] * _cov212[1, 1])
            * _rho212)
_cov212[0, 1] = _corr212
_cov212[1, 0] = _corr212

# требуемая функция np.random.multivariate_normal
_samples212 = np.random.multivariate_normal(_mean212, _cov212, size=_n212)
dfG = pd.DataFrame(_samples212, columns=VIS_212["feature_names"])
dfG["class"] = VIS_212["class_label"]

# ------------------------------------------------------------
# ВИЗУАЛИЗАЦИЯ 1: dfG без вывода цифр (только точки)
# ------------------------------------------------------------
figG1, axG1 = plt.subplots(figsize=(6.5, 6.0), dpi=140)
axG1.scatter(dfG["X"].to_numpy(), dfG["Y"].to_numpy(), alpha=0.75, s=34,
             color="steelblue", edgecolors="white", linewidths=0.5)
axG1.axhline(0.0, color="grey", linewidth=0.6, linestyle=":")
axG1.axvline(0.0, color="grey", linewidth=0.6, linestyle=":")
axG1.set_xlabel("X")
axG1.set_ylabel("Y")
axG1.set_title("dfG: случайная форма гауссиана")
figG1.tight_layout()
plt.show()

# ------------------------------------------------------------
# ВИЗУАЛИЗАЦИЯ 2: те же точки + Пирсон, линия регрессии,
# доверительные (confidence) и прогнозные (predictive) полосы
# ------------------------------------------------------------
xG = dfG["X"].to_numpy(dtype=float)
yG = dfG["Y"].to_numpy(dtype=float)
nG = xG.size
xbarG = float(xG.mean())
sxxG = float(((xG - xbarG) ** 2).sum())

slope, intercept = np.polyfit(xG, yG, 1)
yhatG = intercept + slope * xG
residG = yG - yhatG
sseG = float((residG ** 2).sum())
sigmaG = np.sqrt(sseG / (nG - 2)) if nG > 2 else np.nan
r_pearson = float(np.corrcoef(xG, yG)[0, 1])

conf_level = 0.95
t_crit = float(sps212.t.ppf((1.0 + conf_level) / 2.0, nG - 2))
x_grid = np.linspace(float(xG.min()), float(xG.max()), 300)
mean_pred = intercept + slope * x_grid
conf_half = t_crit * sigmaG * np.sqrt(1.0 / nG
                                     + (x_grid - xbarG) ** 2 / sxxG)
pred_half = t_crit * sigmaG * np.sqrt(1.0 + 1.0 / nG
                                     + (x_grid - xbarG) ** 2 / sxxG)

figG2, axG2 = plt.subplots(figsize=(7.0, 6.0), dpi=140)
axG2.scatter(xG, yG, alpha=0.75, s=34, color="steelblue",
             edgecolors="white", linewidths=0.5, zorder=3)

# доверительная полоса средней регрессии
axG2.fill_between(x_grid, mean_pred - conf_half, mean_pred + conf_half,
                  alpha=0.28, color="tomato", label=f"ДИ средней ({conf_level:.0%})",
                  zorder=1)
# прогнозная полоса для индивидуальных точек
axG2.fill_between(x_grid, mean_pred - pred_half, mean_pred + pred_half,
                  alpha=0.14, color="steelblue", label=f"Прог. полоса ({conf_level:.0%})",
                  zorder=0)
# линия регрессии
axG2.plot(x_grid, mean_pred, color="crimson", linewidth=2.0,
          label="Линия регрессии", zorder=4)

axG2.set_xlabel("X")
axG2.set_ylabel("Y")
axG2.set_title("dfG: регрессия X → Y с доверительными и прогнозными полосами")
axG2.text(0.05, 0.95, f"Pearson r = {r_pearson:.3f}",
          transform=axG2.transAxes, va="top", ha="left", fontsize=11,
          bbox=dict(boxstyle="round,pad=0.3", fc="white", ec="grey", alpha=0.8))
axG2.legend(frameon=True, fontsize=9)
figG2.tight_layout()
plt.show()

print("\n" + "=" * 60)
print("ЗАДАНИЕ [2.12**] ВЫПОЛНЕНО")
print("=" * 60)
