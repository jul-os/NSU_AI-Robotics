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
    fig_nan = plt.figure(figsize=(10, 3))
    fig_nan.text(0.5, 0.5, "В датасете нет пропущенных значений",
                 ha='center', va='center', fontsize=16)
    plt.axis('off')
    plt.savefig("upset_nan.png", dpi=150, bbox_inches="tight")
    plt.show()
    print(f"\n✓ Картинка для NaN сохранена в файл 'upset_nan.png'")
else:
    fig_nan, out_nan = build_upset_visualization(
        dataframe=df,
        columns=feature_cols,
        missing_limit=0,
        selection_method="nonzero",
        layout="horizontal",
        dot_size=80,
        set_size_bar_width=0.5,
        set_size_bar_gap=0.5,
        heading="Upset Plot: Пропущенные значения (NaN) по признакам"
    )
    plt.figure(fig_nan.number)
    plt.savefig("upset_nan.png", dpi=150, bbox_inches="tight")
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
    color="#7AEFC2ED",
    dot_size=80,
    set_size_bar_width=0.5,
    set_size_bar_gap=0.5,
    heading="Upset Plot: Повторяющиеся значения по признакам"
)
plt.figure(fig_dup.number)
plt.savefig("upset_duplicates.png", dpi=150, bbox_inches="tight")
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
fig1 = plt.figure(figsize=(14, 12))
sns.heatmap(pearson_df0, annot=True, fmt='.2f', cmap='coolwarm', center=0,
            square=False, linewidths=0.5, vmin=-1, vmax=1,
            annot_kws={'size': 7.5}, xticklabels=True, yticklabels=True)
plt.xticks(fontsize=6.0)
plt.yticks(fontsize=6.0)
plt.title('Корреляция Пирсона: df0', fontsize=11)
plt.subplots_adjust(top=0.95, bottom=0.18, left=0.18, right=0.95)
plt.savefig('pearson_df0.png', dpi=150, bbox_inches='tight')
plt.show()

# Heat-map Пирсона — df0m (отдельное окно)
fig2 = plt.figure(figsize=(14, 12))
sns.heatmap(pearson_df0m, annot=True, fmt='.2f', cmap='coolwarm', center=0,
            square=False, linewidths=0.5, vmin=-1, vmax=1,
            annot_kws={'size': 7.5}, xticklabels=True, yticklabels=True)
plt.xticks(fontsize=6.0)
plt.yticks(fontsize=6.0)
plt.title('Корреляция Пирсона: df0m', fontsize=11)
plt.subplots_adjust(top=0.95, bottom=0.18, left=0.18, right=0.95)
plt.savefig('pearson_df0m.png', dpi=150, bbox_inches='tight')
plt.show()

# Heat-map Спирмана — df0 (отдельное окно)
fig3 = plt.figure(figsize=(14, 12))
sns.heatmap(spearman_df0, annot=True, fmt='.2f', cmap='coolwarm', center=0,
            square=False, linewidths=0.5, vmin=-1, vmax=1,
            annot_kws={'size': 7.5}, xticklabels=True, yticklabels=True)
plt.xticks(fontsize=6.0)
plt.yticks(fontsize=6.0)
plt.title('Корреляция Спирмана: df0', fontsize=11)
plt.subplots_adjust(top=0.95, bottom=0.18, left=0.18, right=0.95)
plt.savefig('spearman_df0.png', dpi=150, bbox_inches='tight')
plt.show()

# Heat-map Спирмана — df0m (отдельное окно)
fig4 = plt.figure(figsize=(14, 12))
sns.heatmap(spearman_df0m, annot=True, fmt='.2f', cmap='coolwarm', center=0,
            square=False, linewidths=0.5, vmin=-1, vmax=1,
            annot_kws={'size': 7.5}, xticklabels=True, yticklabels=True)
plt.xticks(fontsize=6.0)
plt.yticks(fontsize=6.0)
plt.title('Корреляция Спирмана: df0m', fontsize=11)
plt.subplots_adjust(top=0.95, bottom=0.18, left=0.18, right=0.95)
plt.savefig('spearman_df0m.png', dpi=150, bbox_inches='tight')
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

print(f"\nПризнаки: {features_26}")
print(f"Классы: {classes_26}")

# Цвета и маркеры для классов
class_colors = {5: 'blue', 6: 'orange', 7: 'green'}
class_markers = {5: 'o', 6: 's', 7: '^'}
class_labels = {5: 'Класс 5', 6: 'Класс 6', 7: 'Класс 7'}

n_features = len(features_26)

# Создаём сетку n_features x n_features
fig, axes = plt.subplots(n_features, n_features, figsize=(16, 16))
fig.suptitle('Парные визуализации признаков по классам', fontsize=16, y=0.995)

for i in range(n_features):
    for j in range(n_features):
        ax = axes[i, j]

        if i == j:
            # Диагональ: диаграммы плотности (KDE) для каждого класса
            for cls in classes_26:
                subset = df0m[df0m['quality'] == cls][features_26[i]].dropna()
                sns.kdeplot(subset, ax=ax, color=class_colors[cls],
                           label=class_labels[cls], fill=True, alpha=0.3)
            ax.set_xlabel('')
            ax.set_ylabel('')
            if i == 0 and j == 0:
                ax.legend(fontsize=8, loc='upper right')
        else:
            # Недиагональ: scatter plot с разными цветами/маркерами для классов
            for cls in classes_26:
                subset = df0m[df0m['quality'] == cls]
                ax.scatter(subset[features_26[j]], subset[features_26[i]],
                          c=class_colors[cls], marker=class_markers[cls],
                          label=class_labels[cls], alpha=0.35, s=20,
                          edgecolors='none')
            ax.set_xlabel('')
            ax.set_ylabel('')

        # Настройка тиков
        ax.tick_params(axis='both', labelsize=8, pad=5)

plt.subplots_adjust(wspace=0.45, hspace=0.45, left=0.08, right=0.95, top=0.90, bottom=0.08)

# Общие подписи для строк (Y) и столбцов (X) через fig.text()
for i in range(n_features):
    # Подпись Y для строки i
    y_pos = 1 - (i + 0.5) / n_features * 0.82 - 0.08
    fig.text(0.02, y_pos, features_26[i], rotation=90,
             va='center', ha='center', fontsize=12)

for j in range(n_features):
    # Подпись X для столбца j
    x_pos = 0.08 + (j + 0.5) / n_features * 0.87
    fig.text(x_pos, 0.94, features_26[j],
             va='center', ha='center', fontsize=12)
plt.savefig('pairplot_features.png', dpi=150, bbox_inches='tight')
plt.show()
print("\n✓ Парная визуализация сохранена в файл 'pairplot_features.png'")

print("\n" + "=" * 60)
print("ЗАДАНИЕ [2.6] ВЫПОЛНЕНО")
print("=" * 60)