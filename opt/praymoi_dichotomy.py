import math

def f(x):
    global count
    count += 1
    return x**2 + math.exp(x)

def dichotomy(f, a, b, error):
    global count
    count = 0
#   delta is between 0 and 2*error
    delta = error / 2
    while (b - a)/2 > error:
        x1 = (a + b) / 2 - delta
        x2 = (a + b) / 2 + delta

        if (f(x1) < f(x2)):
            b = x2
        else:
            a = x1
    return (a + b) / 2

error = [10**(-3), 10**(-5)]
for this_error in error:
    x = dichotomy(f, -1, 0, this_error)
    print("С погрешностью ", this_error)
    print("минимум функции находится в точке (", x, ";", f(x), ")")
    print("количество итераций:", count)
