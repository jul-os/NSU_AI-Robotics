import math

def f(x):
    return math.sin(x) + x*(x + 10)/50

def df(x):
    return math.cos(x) + x/25 + 1/5

def ddf(x):
    return -math.sin(x) + 1/25

def newton(df, ddf, x0, error):
    count = 0
    x = x0

    while abs(df(x)) > error:
        if ddf(x) == 0:
            break
        x = x - df(x) / ddf(x)
        count += 1

    return x, count

# Тест с разными начальными точками
start_points = [-8, -1, 4]
error = 10**(-7)

for x0 in start_points:
    x, count = newton(df, ddf, x0, error)
    print(f"Начальная точка x0 = {x0}")
    print(f"Минимум найден в точке x = {x:.6f}")
    print(f"Значение функции f(x) = {f(x):.6f}")
    print(f"Количество итераций: {count}\n")
