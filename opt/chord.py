import math

def f(x):
    return x**2 + math.exp(x)

def df(x):
    global count
    count += 1
    return 2*x + math.exp(x)

def secant(f, df, a, b, error):
    global count
    count = 0

    x0 = a
    x1 = b

    f0 = df(x0)
    f1 = df(x1)

    while abs(f1) > error:
        if f1 == f0:
            break

        x_new = x1 - f1 * (x1 - x0) / (f1 - f0)

        x0 = x1
        f0 = f1

        x1 = x_new
        f1 = df(x1)

    return x1

error = [10**(-3), 10**(-5)]
for this_error in error:
    x = secant(f, df, -1, 0, this_error)
    print("С погрешностью ", this_error)
    print("минимум функции находится в точке (", x, ";", f(x), ")")
    print("количество обращений к производной:", count)
