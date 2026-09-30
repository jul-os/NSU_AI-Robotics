import math

def f(x):
    return math.sin(x) + x*(x + 10)/50

def lomaniy(f, a, b, error):
    L = 6/5  # Константа Липшица

    # Шаг 1: Вычислить x₀ и y₀ по формулам (2.27)
    x0 = (f(a) - f(b) + L*(a + b)) / (2*L)
    y0 = (f(a) + f(b) + L*(a - b)) / 2

    # Храним пары (x, p) - вершины ломаной
    pairs = [(x0, y0)]

    iteration = 0

    while True:
        iteration += 1

        # Шаг 2: Найти пару (x*, p*) с минимальным p
        x_star, p_star = min(pairs, key=lambda pair: pair[1])

        # Вычислить f(x*)
        f_star = f(x_star)

        # Шаг 3: Проверка условия остановки
        delta = (f_star - p_star) / (2*L)
        delta_k = 2 * L * delta

        if delta_k <= error:
            return x_star, f_star, iteration

        # Шаг 4: Определить новые пары
        x_new1 = x_star - delta
        x_new2 = x_star + delta
        p_new = (f_star + p_star) / 2

        # Заменить пару (x*, p*) на две новые
        pairs.remove((x_star, p_star))
        pairs.append((x_new1, p_new))
        pairs.append((x_new2, p_new))

error = [10**(-3), 10**(-5)]
for this_error in error:
    x, fx, iterations = lomaniy(f, -10, 10, this_error)
    print(f"С погрешностью {this_error}")
    print(f"минимум функции находится в точке ({x:.6f}; {fx:.6f})")
    print(f"количество итераций: {iterations}\n")
