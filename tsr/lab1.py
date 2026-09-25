import numpy as np
import pandas as pd

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
classes = np.random.randint(0, 2, size=n_samples)

# 8. Формируем итоговый датафрейм dfS00
dfS00 = pd.DataFrame(Y, columns=['A', 'B', 'C', 'D'])
dfS00['class'] = classes

# Проверка выборочной корреляционной матрицы
print("Корреляционная матрица выборки dfS00:")
print(dfS00[['A', 'B', 'C', 'D']].corr())