#pragma once
#include <cstddef>
#include <vector>
class Matrix final
{
public:
    Matrix(); // конструктор по умолчанию.
    // конструктор для создания вектор (матрицы с одной строкой). cols - размер вектора.
    Matrix(size_t cols);
    // конструктор с параметрами. rows -- количество строк, cols --количество столбцов.
    Matrix(size_t rows, size_t cols);
    ~Matrix(); // деструктор.

    // конструктор копий. Выполняет полное копирование матрицы.
    Matrix(const Matrix &mat);
    // оператор умножения матриц. При невозможности
    // выполнения операции возвращает Matrix().
    Matrix operator*(const Matrix &mat) const;
    // оператор вычитания матриц. При невозможности
    // выполнения операции возвращает Matrix().
    Matrix operator-(const Matrix &mat) const;
    // оператор сложения матриц. При невозможности
    // выполнения операции возвращает Matrix().
    Matrix operator+(const Matrix &mat) const;
    // оператор умножения на число.
    Matrix operator*(double value) const;
    // оператор деления на число.
    Matrix operator/(double value) const;

    // оператор присваивания. Выполняет полное копирование матрицы.
    Matrix &operator=(const Matrix &mat);
    /*совмещенный оператор умножения и присваивания. При
невозможности выполнения операции объект, который вызывал данный оператор
становится невалидным. Т.е. при вызове метода isValid(), он вернет false.*/
    Matrix &operator*=(const Matrix &mat);
    /*совмещенный оператор сложения и присваивания. При
    невозможности выполнения операции объект, который вызывал данный оператор
    становится невалидным. Т.е. при вызове метода isValid(), он вернет false.*/
    Matrix &operator+=(const Matrix &mat);
    /*совмещенный оператор вычитания и присваивания. При
    невозможности выполнения операции объект, который вызывал данный оператор
    становится невалидным. Т.е. при вызове метода isValid(), он вернет false.*/
    Matrix &operator-=(const Matrix &mat);

    // совмещенный оператор умножения на число и присваивания.
    Matrix &operator*=(double value);
    // совмещенный оператор деления на число и присваивания.c
    Matrix &operator/=(double value);

    /*проверка матрицы на валидность. Если матрица невалидна, то метод
    возвращает false, иначе true. Для пустой матрицы: isValid() == false.*/
    bool isValid() const;
    // меняет размер матрицы.
    void resize(size_t rows, size_t cols);

    /*доступ к элементу (i, j). Возвращает
    константную ссылку на элемент матрицы. При выходе за границу матрицы кидаем
    exception: throw std::out_of_range(&quot;message&quot;);.*/
    const double &coeffRef(size_t rowIdx, size_t colIdx) const;
    /*доступ к элементу (i, j). Возвращает ссылку на
элемент матрицы. При выходе за границу матрицы кидаем exception: throw*/
    double &coeffRef(size_t rowIdx, size_t colIdx);

    // возвращает указатель на константные элементы матрицы.
    const double *data() const;
    // возвращает указатель на элементы матрицы.
    double *data();

    // возвращает общее количество строк в матрице.
    size_t rows() const;
    // возвращает общее количество столбцов в матрице.
    size_t cols() const;

    /*делает матрицу единичной. Возвращает ссылку на объект, вызвавший
    этот метод.*/
    Matrix &setIdentity();
    // заполняет матрицу нулями. Возвращает ссылку на объект, вызвавший этот метод.
    Matrix &setZero();
    /*заполняет матрицу константными значениями value.
Возвращает ссылку на объект, вызвавший этот метод.*/
    Matrix &setConstants(double value);

    /*задает новый размер матрице и делает матрицу
единичной. Возвращает ссылку на объект, вызвавший этот метод.*/
    Matrix &setIdentity(size_t rows, size_t cols);
    /*задает новый размер матрице и заполняет матрицу
нулями. Возвращает ссылку на объект, вызвавший этот метод.*/
    Matrix &setZero(size_t rows, size_t cols);
    /*задает новый размер матрице и
заполняет матрицу константными значениями value. Возвращает ссылку на объект,
вызвавший этот метод.*/
    Matrix &setConstants(size_t rows, size_t cols, double value);

    // транспонирует матрицу.
    Matrix transpose() const;
    /*вычисляет обратную матрицу. При невозможности выполнения операции
метод возвращает Matrix().*/
    Matrix inverse() const;
    /*вычисляет определитель матрицы. При невозможности выполнения операции
метод возвращает NAN.*/
    double det() const;

    /*создает новую матрицу и делает матрицу
единичной. Возвращает созданную матрицу.*/
    static Matrix identity(size_t rows, size_t cols);
    /*создает новую матрицу и заполняет матрицу нулями.
Возвращает созданную матрицу.*/
    static Matrix zeros(size_t rows, size_t cols);
    /*создает новую матрицу и заполняет
матрицу константными значениями value. Возвращает созданную матрицу.*/
    static Matrix constants(size_t rows, size_t cols, double value);

    /*оператор умножения числа на матрицу. Т.е.
левым операндом является число, а не матрица.*/
    friend Matrix operator*(double value, const Matrix &mat);

private:
    size_t num_rows;
    size_t num_cols;
    std::vector<std::vector<double>> m_data;
};