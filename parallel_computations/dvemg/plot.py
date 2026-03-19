import matplotlib.pyplot as plt
import numpy as np

# Ваши данные
threads = np.array([1, 2, 4, 7, 8, 16, 20, 40])
times = np.array([0.461497, 0.244100, 0.130481, 0.079605, 0.070408, 0.040921, 0.034910, 0.030375])
speedups = np.array([1.04, 1.96, 3.67, 6.01, 6.79, 11.69, 13.70, 15.75])
serial_time = 0.478266  # эталонное последовательное время

# Идеальное ускорение (линейное)
ideal_speedup = threads.copy()

# Эффективность
efficiency = (speedups / threads) * 100

# Построение графиков
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))

# График 1: Ускорение
ax1.plot(threads, speedups, 'bo-', linewidth=2, markersize=6, label='Реальное ускорение')
ax1.plot(threads, ideal_speedup, 'r--', linewidth=1.5, alpha=0.7, label='Идеальное (линейное)')
ax1.set_xlabel('Количество потоков', fontsize=11)
ax1.set_ylabel('Ускорение (Speedup)', fontsize=11)
ax1.set_title('Ускорение программы интегрирования', fontsize=12, fontweight='bold')
ax1.grid(True, alpha=0.3)
ax1.legend(fontsize=10)
ax1.set_xticks(threads)
ax1.set_xlim(0, 42)

# График 2: Эффективность
colors = ['#2ecc71' if e >= 50 else '#f39c12' if e >= 30 else '#e74c3c' for e in efficiency]
bars = ax2.bar(threads, efficiency, color=colors, edgecolor='black', alpha=0.8)
ax2.set_xlabel('Количество потоков', fontsize=11)
ax2.set_ylabel('Эффективность, %', fontsize=11)
ax2.set_title('Эффективность параллелизации', fontsize=12, fontweight='bold')
ax2.grid(True, alpha=0.3, axis='y')
ax2.set_xticks(threads)
ax2.set_ylim(0, 110)

# Добавление значений на столбцы
for bar, eff in zip(bars, efficiency):
    ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 2, 
             f'{eff:.1f}%', ha='center', fontsize=9)

plt.tight_layout()
plt.savefig('speedup_report.png', dpi=300, bbox_inches='tight')
plt.show()

# Вывод статистики в консоль
print("\n=== Статистика масштабируемости ===")
print(f"{'Потоки':<8} {'Время (с)':<12} {'Ускорение':<10} {'Эффективность, %':<15}")
print("-" * 45)
for t, time, sp, eff in zip(threads, times, speedups, efficiency):
    print(f"{t:<8} {time:<12.6f} {sp:<10.2f} {eff:<15.2f}")