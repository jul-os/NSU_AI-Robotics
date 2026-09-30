import math

def f(x):
    global count
    count += 1
    return x**2 + math.exp(x)

def golden(f, a, b, error):
    global count
    count = 0
    t = (math.sqrt(5) - 1) / 2

    x2 = a + t * (b - a)
    x1 = a + b - x2

    while (b - a) / 2 > error:
        if f(x1) < f(x2):
            b = x2
            x2 = x1
            x1 = a + b - x2
        else:
            a = x1
            x1 = x2
            x2 = a + b - x1

    return (b + a) / 2

error = [10**(-3), 10**(-5)]
for this_error in error:
    x = golden(f, -1, 0, this_error)
    print("С погрешностью ", this_error)
    print("минимум функции находится в точке (", x, ";", f(x), ")")
    print("количество итераций:", count)
