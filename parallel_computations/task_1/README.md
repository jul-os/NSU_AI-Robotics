По умолчанию создается массив типа double.
Чтобы создать массив типа float при сборке введите:

```console
cmake -B build_float -DUSE_FLOAT=ON
cmake --build build_float
```

запустить:

```console
./build_float/sine_sum
```

Чтобы использовать тип double при сборке введите:

```console
cmake -B build
cmake --build build
```

запустить:

```console
./build/sine_sum
```

Результаты:<br>
float Sum: 0.0001428930<br>
double Sum: -0.0000000000
