import math

def f(x):
    global count
    count += 1
    return x**2 + math.exp(x)

def parabol(f, a, b, error):
    global count
    count = 0

    # Другие начальные точки
    x1 = a
    x2 = a + 0.3 * (b - a)
    x3 = a + 0.7 * (b - a)

    f1 = f(x1)
    f2 = f(x2)
    f3 = f(x3)

    while True:
        numerator = (x2 - x1)**2 * (f2 - f3) - (x2 - x3)**2 * (f2 - f1)
        denominator = (x2 - x1) * (f2 - f3) - (x2 - x3) * (f2 - f1)

        if denominator == 0:
            break

        u = x2 - 0.5 * numerator / denominator
        fu = f(u)

        if abs(u - x2) < error:
            break

        if u < x1 or u > x3:
            break

        if fu < f2:
            if u < x2:
                x3 = x2
                f3 = f2
            else:
                x1 = x2
                f1 = f2
            x2 = u
            f2 = fu
        else:
            if u < x2:
                x1 = u
                f1 = fu
            else:
                x3 = u
                f3 = fu

    return x2

error = [10**(-3), 10**(-5)]
for this_error in error:
    x = parabol(f, -1, 0, this_error)
    print("С погрешностью ", this_error)
    print("минимум функции находится в точке (", x, ";", f(x), ")")
    print("количество итераций:", count)
