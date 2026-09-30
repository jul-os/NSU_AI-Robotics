import math

def f(x):
    return math.sin(x) + x*(x + 10)/50

def df(x):
    global count
    count += 1
    return math.cos(x) + (2*x + 10)/50

def ddf(x):
    return -math.sin(x) + 2/50

def broken_lines(f, df, ddf, x0, error):
    global count
    count = 0

    x = x0

    while abs(df(x)) > error:
        ddf_x = ddf(x)

        if ddf_x == 0:
            break

        x_new = x - df(x) / ddf_x

        if abs(x_new - x) < error:
            x = x_new
            break

        x = x_new

    return x

error = [10**(-3), 10**(-5)]
for this_error in error:
    x = broken_lines(f, df, ddf, -5, this_error)
    print("С погрешностью ", this_error)
    print("минимум функции находится в точке (", x, ";", f(x), ")")
    print("количество обращений к первой производной:", count)
