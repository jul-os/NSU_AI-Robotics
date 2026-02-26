import matplotlib.pyplot as plt
import numpy as np

# Данные
threads = [1, 2, 4, 7, 8, 16, 20, 40]
speedup_20000 = [1.00, 1.79, 3.09, 6.56, 7.26, 13.85, 15.82, 13.96]
speedup_40000 = [1.00, 1.96, 4.12, 7.15, 8.18, 15.81, 7.30, 13.02]
ideal = threads

plt.figure(figsize=(14, 8))

# Основные графики
plt.plot(threads, speedup_20000, 'o-', linewidth=2.5, markersize=10, 
         label='20000×20000', color='blue', markeredgecolor='darkblue')
plt.plot(threads, speedup_40000, 's-', linewidth=2.5, markersize=10, 
         label='40000×40000', color='red', markeredgecolor='darkred')
plt.plot(threads, ideal, 'k--', linewidth=2, label='Идеальное ускорение', alpha=0.7)

# Выделение аномалии на 20 потоках для 40000x40000
plt.plot(20, 7.30, 'ro', markersize=15, markeredgecolor='darkred', 
         markeredgewidth=2, fillstyle='none', label='Аномалия (20 потоков)')

plt.xlabel('Количество потоков', fontsize=14)
plt.ylabel('Ускорение S = T(1)/T(p)', fontsize=14)
plt.title('Зависимость ускорения от количества потоков\nдля матрично-векторного умножения', 
          fontsize=16, fontweight='bold')
plt.legend(fontsize=12, loc='upper left')
plt.grid(True, alpha=0.3, linestyle='--')
plt.xticks(threads, fontsize=12)
plt.yticks(fontsize=12)
plt.xlim(0, 42)
plt.ylim(0, 18)

# Добавим горизонтальную линию на уровне 16 (макс ускорение)
plt.axhline(y=16, color='gray', linestyle=':', alpha=0.5)
plt.text(42, 16, ' max = 16', fontsize=10, va='center')

# Аннотации для ключевых точек
plt.annotate(f'15.82x', xy=(20, 15.82), xytext=(22, 16.5),
            arrowprops=dict(arrowstyle='->', color='blue'),
            fontsize=10, color='blue', fontweight='bold')

plt.annotate(f'7.30x', xy=(20, 7.30), xytext=(22, 6.0),
            arrowprops=dict(arrowstyle='->', color='red'),
            fontsize=10, color='red', fontweight='bold')

plt.tight_layout()
plt.savefig('speedup_analysis.pdf', format='pdf', dpi=300, bbox_inches='tight')
plt.show()