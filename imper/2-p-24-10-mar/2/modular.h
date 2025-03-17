#ifndef MODULAR_H
#define MODULAR_H

#pragma once //so it'll be includede only once in the current compilation

extern int MOD;

int pnorm(int num);
//принимает одно значение типа int, возвращает int. Функция возвра-
//щает остаток от деления переданного аргумента по текущему модулю.

// каждая принимает два параметра типа int, возвра-
// щает int. Они реализуют сложение, вычитание, умножение и деление соответственно
// в поле вычетов по модулю текущего MOD.
int padd(int num1, int num2);

int psub(int num1, int num2);

int pmul(int num1, int num2);

int pdiv(int num1, int num2);

#endif // MODULAR_H