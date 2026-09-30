import math

def f(x):
    return x**2 + math.exp(x)

def f_der(x):
    global count
    count += 1
    return 2 * x + math.exp(x)

def dichotomy_first_order(f, a, b, error):
    global count
    count = 0
    while (b - a) / 2 > error:
        c = (a + b) / 2
        if f_der(c) > 0:
            b = c
        elif f_der(c) < 0:
            a = c
        elif f_der(c) <= error:
            return c

    return (a + b) / 2


error = [10**(-3), 10**(-5)]
for this_error in error:
    x = dichotomy_first_order(f, -1, 0, this_error)
    print("С погрешностью ", this_error)
    print("минимум функции находится в точке (", x, ";", f(x), ")")
    print("количество итераций:", count)
