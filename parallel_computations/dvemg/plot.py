import matplotlib.pyplot as plt
import numpy as np

# Данные из таблицы (исправленные значения)
p = [1, 2, 4, 7, 8, 16, 20, 40]
S_at_20000 = [1.0, 2.23, 3.97, 8.22, 8.67, 17.44, 12.35, 21.47]
S_at_40000 = [1.0, 2.10, 4.20, 7.47, 8.30, 10.64, 7.25, 20.38]

# Создание графика
plt.figure(figsize=(12, 8))

# Построение линий
plt.plot(p, S_at_20000, 'o-', linewidth=2, markersize=8, label='20000×20000', color='blue', markeredgecolor='darkblue')
plt.plot(p, S_at_40000, 's-', linewidth=2, markersize=8, label='40000×40000', color='red', markeredgecolor='darkred')

# Линия идеального ускорения (для сравнения)
ideal = p
plt.plot(p, ideal, '--', linewidth=1.5, label='Идеальное ускорение', color='gray', alpha=0.7)

# Настройка осей
plt.xlabel('Количество потоков (p)', fontsize=12)
plt.ylabel('Ускорение (S)', fontsize=12)
plt.title('Зависимость ускорения от количества потоков\n(умножение матрицы на вектор)', fontsize=14, fontweight='bold')

# Добавление сетки
plt.grid(True, alpha=0.3, linestyle='--')

# Добавление легенды
plt.legend(fontsize=11, loc='upper left')

# Настройка делений на осях
plt.xticks(p)  # Отмечаем все значения p на оси x
plt.yticks(np.arange(0, 26, 2))  # Деления на оси y от 0 до 25 с шагом 2
plt.xlim(0, 45)  # Небольшой отступ по краям
plt.ylim(0, 25)

# Добавление подписей значений для точек
for i, (x, y1) in enumerate(zip(p, S_at_20000)):
    offset_y = 5 if i == 6 else 10  # Специальная корректировка для пиковой точки
    plt.annotate(f'{y1:.2f}', (x, y1), textcoords="offset points", 
                xytext=(0, offset_y), ha='center', fontsize=9, color='blue')

for i, (x, y2) in enumerate(zip(p, S_at_40000)):
    offset_y = -15 if i == 6 else 10  # Специальная корректировка для падающей точки
    plt.annotate(f'{y2:.2f}', (x, y2), textcoords="offset points", 
                xytext=(0, offset_y), ha='center', fontsize=9, color='red')

# Выделение проблемной точки (20 потоков)
plt.axvline(x=20, color='orange', linestyle=':', alpha=0.7, linewidth=1.5)
plt.text(20.5, 5, 'NUMA-эффект', rotation=90, fontsize=10, color='orange', alpha=0.8)

# Добавление аннотации для NUMA-узлов
plt.text(35, 3, '2 NUMA-узла\n(20 ядер на узел)', 
         bbox=dict(boxstyle="round,pad=0.3", facecolor="lightyellow", alpha=0.7),
         fontsize=9, ha='center')

# Отображение графика
plt.tight_layout()
plt.show()

# Вывод статистики для отчета
print("Статистика ускорения:")
print("-" * 50)
print(f"{'Потоки':<10} {'20000×20000':<15} {'40000×40000':<15} {'Эффективность (20000)':<20} {'Эффективность (40000)':<20}")
print("-" * 50)
for i, threads in enumerate(p):
    eff_20k = (S_at_20000[i] / threads) * 100
    eff_40k = (S_at_40000[i] / threads) * 100
    print(f"{threads:<10} {S_at_20000[i]:<15.2f} {S_at_40000[i]:<15.2f} {eff_20k:<20.1f}% {eff_40k:<20.1f}%")