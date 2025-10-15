#include "matrix.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

using namespace std;

// Вспомогательная функция для сравнения double с учётом погрешности
bool almost_equal(double a, double b, double epsilon = 1e-10)
{
    return std::abs(a - b) < epsilon;
}

void test_constructors()
{
    cout << "Testing constructors..." << endl;

    // Конструктор по умолчанию
    Matrix m1;
    assert(!m1.isValid());
    assert(m1.rows() == 0);
    assert(m1.cols() == 0);

    // Конструктор с одним параметром (вектор)
    Matrix m2(3);
    assert(m2.isValid());
    assert(m2.rows() == 1);
    assert(m2.cols() == 3);

    // Конструктор с двумя параметрами
    Matrix m3(2, 4);
    assert(m3.isValid());
    assert(m3.rows() == 2);
    assert(m3.cols() == 4);

    // Конструктор копирования
    Matrix m4 = m3;
    assert(m4.isValid());
    assert(m4.rows() == 2);
    assert(m4.cols() == 4);

    cout << "Constructors test passed!" << endl;
}

void test_accessors()
{
    cout << "Testing accessors..." << endl;

    Matrix m(2, 3);

    // Тест записи и чтения
    m.coeffRef(0, 0) = 1.0;
    m.coeffRef(0, 1) = 2.0;
    m.coeffRef(0, 2) = 3.0;
    m.coeffRef(1, 0) = 4.0;
    m.coeffRef(1, 1) = 5.0;
    m.coeffRef(1, 2) = 6.0;

    assert(almost_equal(m.coeffRef(0, 0), 1.0));
    assert(almost_equal(m.coeffRef(1, 2), 6.0));

    // Тест константного доступа
    const Matrix &cm = m;
    assert(almost_equal(cm.coeffRef(0, 1), 2.0));

    // Тест исключений при выходе за границы
    try
    {
        m.coeffRef(5, 5);
        assert(false); // Не должны сюда попасть
    }
    catch (const std::out_of_range &)
    {
        // Ожидаемое поведение
    }

    cout << "Accessors test passed!" << endl;
}

void test_arithmetic_operations()
{
    cout << "Testing arithmetic operations..." << endl;

    Matrix a(2, 2);
    a.coeffRef(0, 0) = 1;
    a.coeffRef(0, 1) = 2;
    a.coeffRef(1, 0) = 3;
    a.coeffRef(1, 1) = 4;

    Matrix b(2, 2);
    b.coeffRef(0, 0) = 5;
    b.coeffRef(0, 1) = 6;
    b.coeffRef(1, 0) = 7;
    b.coeffRef(1, 1) = 8;

    // Сложение
    Matrix c = a + b;
    assert(c.isValid());
    assert(almost_equal(c.coeffRef(0, 0), 6.0));
    assert(almost_equal(c.coeffRef(1, 1), 12.0));

    // Вычитание
    Matrix d = a - b;
    assert(d.isValid());
    assert(almost_equal(d.coeffRef(0, 0), -4.0));

    // Умножение на скаляр
    Matrix e = a * 2.0;
    assert(e.isValid());
    assert(almost_equal(e.coeffRef(1, 0), 6.0));

    // Умножение скаляра на матрицу
    Matrix f = 3.0 * a;
    assert(f.isValid());
    assert(almost_equal(f.coeffRef(0, 1), 6.0));

    // Деление на скаляр
    Matrix g = a / 2.0;
    assert(g.isValid());
    assert(almost_equal(g.coeffRef(1, 1), 2.0));

    cout << "Arithmetic operations test passed!" << endl;
}

void test_matrix_multiplication()
{
    cout << "Testing matrix multiplication..." << endl;

    Matrix a(2, 3);
    a.coeffRef(0, 0) = 1;
    a.coeffRef(0, 1) = 2;
    a.coeffRef(0, 2) = 3;
    a.coeffRef(1, 0) = 4;
    a.coeffRef(1, 1) = 5;
    a.coeffRef(1, 2) = 6;

    Matrix b(3, 2);
    b.coeffRef(0, 0) = 7;
    b.coeffRef(0, 1) = 8;
    b.coeffRef(1, 0) = 9;
    b.coeffRef(1, 1) = 10;
    b.coeffRef(2, 0) = 11;
    b.coeffRef(2, 1) = 12;

    // Умножение матриц
    Matrix c = a * b;
    assert(c.isValid());
    assert(c.rows() == 2);
    assert(c.cols() == 2);
    assert(almost_equal(c.coeffRef(0, 0), 58.0));
    assert(almost_equal(c.coeffRef(0, 1), 64.0));
    assert(almost_equal(c.coeffRef(1, 0), 139.0));
    assert(almost_equal(c.coeffRef(1, 1), 154.0));

    // Невозможное умножение
    Matrix d(2, 2);
    Matrix e(3, 3);
    Matrix f = d * e;
    assert(!f.isValid());

    cout << "Matrix multiplication test passed!" << endl;
}

void test_assignment_operators()
{
    cout << "Testing assignment operators..." << endl;

    Matrix a(2, 2);
    a.coeffRef(0, 0) = 1;
    a.coeffRef(0, 1) = 2;
    a.coeffRef(1, 0) = 3;
    a.coeffRef(1, 1) = 4;

    Matrix b(2, 2);
    b.coeffRef(0, 0) = 5;
    b.coeffRef(0, 1) = 6;
    b.coeffRef(1, 0) = 7;
    b.coeffRef(1, 1) = 8;

    // Присваивание
    Matrix c = a;
    assert(c.isValid());
    assert(almost_equal(c.coeffRef(1, 1), 4.0));

    // Совмещённые операторы
    a += b;
    assert(almost_equal(a.coeffRef(0, 0), 6.0));

    a -= b;
    assert(almost_equal(a.coeffRef(0, 0), 1.0));

    a *= 2.0;
    assert(almost_equal(a.coeffRef(1, 1), 8.0));

    a /= 2.0;
    assert(almost_equal(a.coeffRef(1, 1), 4.0));

    cout << "Assignment operators test passed!" << endl;
}

void test_identity_and_zeros()
{
    cout << "Testing identity and zeros..." << endl;

    // Единичная матрица
    Matrix identity = Matrix::identity(3, 3);
    assert(identity.isValid());
    assert(almost_equal(identity.coeffRef(0, 0), 1.0));
    assert(almost_equal(identity.coeffRef(0, 1), 0.0));
    assert(almost_equal(identity.coeffRef(1, 1), 1.0));
    assert(almost_equal(identity.coeffRef(2, 2), 1.0));

    // Нулевая матрица
    Matrix zeros = Matrix::zeros(2, 2);
    assert(zeros.isValid());
    assert(almost_equal(zeros.coeffRef(0, 0), 0.0));
    assert(almost_equal(zeros.coeffRef(1, 1), 0.0));

    // Матрица констант
    Matrix constants = Matrix::constants(2, 3, 5.0);
    assert(constants.isValid());
    assert(almost_equal(constants.coeffRef(0, 0), 5.0));
    assert(almost_equal(constants.coeffRef(1, 2), 5.0));

    cout << "Identity and zeros test passed!" << endl;
}

void test_transpose()
{
    cout << "Testing transpose..." << endl;

    Matrix a(2, 3);
    a.coeffRef(0, 0) = 1;
    a.coeffRef(0, 1) = 2;
    a.coeffRef(0, 2) = 3;
    a.coeffRef(1, 0) = 4;
    a.coeffRef(1, 1) = 5;
    a.coeffRef(1, 2) = 6;

    Matrix b = a.transpose();
    assert(b.isValid());
    assert(b.rows() == 3);
    assert(b.cols() == 2);
    assert(almost_equal(b.coeffRef(0, 0), 1.0));
    assert(almost_equal(b.coeffRef(0, 1), 4.0));
    assert(almost_equal(b.coeffRef(2, 0), 3.0));
    assert(almost_equal(b.coeffRef(2, 1), 6.0));

    cout << "Transpose test passed!" << endl;
}

void test_determinant()
{
    // todo 10*10
    cout << "Testing determinant..." << endl;

    // Матрица 1x1
    Matrix a(1, 1);
    a.coeffRef(0, 0) = 5.0;
    assert(almost_equal(a.det(), 5.0));

    // Матрица 2x2
    Matrix b(2, 2);
    b.coeffRef(0, 0) = 1;
    b.coeffRef(0, 1) = 2;
    b.coeffRef(1, 0) = 3;
    b.coeffRef(1, 1) = 4;
    assert(almost_equal(b.det(), -2.0));

    // Матрица 3x3
    Matrix c(3, 3);
    c.coeffRef(0, 0) = 2;
    c.coeffRef(0, 1) = 1;
    c.coeffRef(0, 2) = 1;
    c.coeffRef(1, 0) = 1;
    c.coeffRef(1, 1) = 2;
    c.coeffRef(1, 2) = 1;
    c.coeffRef(2, 0) = 1;
    c.coeffRef(2, 1) = 1;
    c.coeffRef(2, 2) = 2;
    assert(almost_equal(c.det(), 4.0));

    // Не квадратная матрица
    Matrix d(2, 3);
    assert(std::isnan(d.det()));

    cout << "Determinant test passed!" << endl;
}

void test_inverse()
{
    cout << "Testing inverse..." << endl;

    // Матрица 2x2
    Matrix a(2, 2);
    a.coeffRef(0, 0) = 4;
    a.coeffRef(0, 1) = 7;
    a.coeffRef(1, 0) = 2;
    a.coeffRef(1, 1) = 6;

    Matrix inv = a.inverse();
    assert(inv.isValid());
    // todo 7*7

    // Проверяем, что A * A^(-1) = I
    Matrix identity_check = a * inv;
    identity_check.setIdentity(); // Округляем для сравнения

    assert(almost_equal(identity_check.coeffRef(0, 0), 1.0));
    assert(almost_equal(identity_check.coeffRef(1, 1), 1.0));

    // Вырожденная матрица
    Matrix singular(2, 2);
    singular.coeffRef(0, 0) = 1;
    singular.coeffRef(0, 1) = 2;
    singular.coeffRef(1, 0) = 2;
    singular.coeffRef(1, 1) = 4;
    Matrix inv_singular = singular.inverse();
    assert(!inv_singular.isValid());

    cout << "Inverse test passed!" << endl;
}

void test_resize()
{
    cout << "Testing resize..." << endl;

    Matrix m(2, 2);
    m.coeffRef(0, 0) = 1;
    m.coeffRef(0, 1) = 2;
    m.coeffRef(1, 0) = 3;
    m.coeffRef(1, 1) = 4;

    m.resize(3, 3);
    assert(m.rows() == 3);
    assert(m.cols() == 3);
    assert(almost_equal(m.coeffRef(0, 0), 1.0)); // Старые данные сохранились
    assert(almost_equal(m.coeffRef(2, 2), 0.0)); // Новые элементы = 0

    cout << "Resize test passed!" << endl;
}

int main()
{
    try
    {
        test_constructors();
        test_accessors();
        test_arithmetic_operations();
        test_matrix_multiplication();
        test_assignment_operators();
        test_identity_and_zeros();
        test_transpose();
        test_determinant();
        test_inverse();
        test_resize();

        cout << "\nAll tests passed successfully!" << endl;
        return 0;
    }
    catch (const exception &e)
    {
        cerr << "Test failed: " << e.what() << endl;
        return 1;
    }
}