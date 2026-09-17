import math

calls = 0
EPSILON = 0.001

def func(x: float) -> float:
    global calls
    calls += 1
    return math.exp(x) + x * x

def main():
    global calls
    calls = 0

    N = int((4 * 2) / EPSILON)
    delta = 2.0 / N

    res = 1000000000000.0
    f = 0.0
    res_x = 1000.0

    x = 0.0
    while x > -2:
        f = func(x)
        if f < res:
            res = f
            res_x = x
        else:
            break
        x -= delta

    # Формат .6f используется для имитации стандартного вывода %f в C
    print(res, " - значение функции")
    print(res_x," - значение аргумента")
    print(calls, " - вызовов функции")

if __name__ == "__main__":
    main()
