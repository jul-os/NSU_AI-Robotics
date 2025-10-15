#include "matrix.hpp"
#include <iostream>
#include <limits>

using namespace std;
Matrix::Matrix() : num_rows(0), num_cols(0), m_data() {}
Matrix::Matrix(size_t cols) : num_cols(cols), num_rows(1), m_data(1, std::vector<double>(cols, 0.0)) {}
Matrix::Matrix(size_t rows, size_t cols) : num_cols(cols), num_rows(rows), m_data(rows, std::vector<double>(cols, 0.0)) {}
Matrix::~Matrix()
{
    // еструктор не нужен std::vector сам управляет памятью
}

Matrix::Matrix(const Matrix &mat)
    : num_rows(mat.num_rows), num_cols(mat.num_cols),
      m_data(mat.m_data)
{
    // std::vector имеет встроенный конструктор копирования, который создаёт полную глубокую копию всех данных
}

// this  - левый операнд (неявный, текущий объект)
// &mat  - правый операнд (явный параметр)

Matrix Matrix::operator*(const Matrix &mat) const
{
    // Умножение возможно если: cols(this) == rows(mat)
    if (num_cols != mat.num_rows)
    {
        return Matrix(); // возвращаем невалидную матрицу
    }
    // rows(this) × cols(mat)
    Matrix result(num_rows, mat.num_cols);
    // C[i][j] = Σ(A[i][k] * B[k][j])
    for (size_t i = 0; i < num_rows; ++i)
    {
        for (size_t j = 0; j < mat.num_cols; ++j)
        {
            double sum = 0.0;
            for (size_t k = 0; k < num_cols; ++k)
            {
                sum += m_data[i][k] * mat.m_data[k][j];
            }
            result.m_data[i][j] = sum;
        }
    }
    return result;
}

Matrix Matrix::operator-(const Matrix &mat) const
{
    if ((num_cols != mat.num_cols) || (num_rows != mat.num_rows))
    {
        return Matrix();
    }
    Matrix result(num_rows, num_cols);

    for (size_t i = 0; i < num_rows; ++i)
    {
        for (size_t j = 0; j < num_cols; ++j)
        {
            result.m_data[i][j] = m_data[i][j] - mat.m_data[i][j];
        }
    }
    return result;
}

Matrix Matrix::operator+(const Matrix &mat) const
{
    if ((num_cols != mat.num_cols) || (num_rows != mat.num_rows))
    {
        return Matrix();
    }
    Matrix result(num_rows, num_cols);

    for (size_t i = 0; i < num_rows; ++i)
    {
        for (size_t j = 0; j < num_cols; ++j)
        {
            result.m_data[i][j] = m_data[i][j] + mat.m_data[i][j];
        }
    }
    return result;
}

Matrix Matrix::operator*(double value) const
{
    Matrix result(num_rows, num_cols);
    for (size_t i = 0; i < num_rows; ++i)
    {
        for (size_t j = 0; j < num_cols; ++j)
        {
            result.m_data[i][j] = m_data[i][j] * value;
        }
    }
    return result;
}

Matrix Matrix::operator/(double value) const
{
    Matrix result(num_rows, num_cols);
    for (size_t i = 0; i < num_rows; ++i)
    {
        for (size_t j = 0; j < num_cols; ++j)
        {
            result.m_data[i][j] = m_data[i][j] / value;
        }
    }
    return result;
}

Matrix &Matrix::operator=(const Matrix &mat)
{
    // Проверка на самоприсваивание: a = a;
    if (this == &mat)
    {
        return *this;
    }
    // Копируем размеры
    num_rows = mat.num_rows;
    num_cols = mat.num_cols;
    // Копируем данные (вектор автоматически делает глубокое копирование)
    m_data = mat.m_data;
    return *this;
}

Matrix &Matrix::operator*=(const Matrix &mat)
{
    if (num_cols != mat.num_rows)
    {
        // Делаем невалидным
        num_rows = 0;
        num_cols = 0;
        m_data.clear();
        return *this;
    }
    size_t old_rows = num_rows;

    // Изменяем размер текущей матрицы для результата
    num_cols = mat.num_cols;
    std::vector<std::vector<double>> new_data(old_rows, std::vector<double>(num_cols, 0.0));
    for (size_t i = 0; i < old_rows; ++i)
    {
        for (size_t j = 0; j < num_cols; ++j)
        {
            double sum = 0.0;
            for (size_t k = 0; k < num_cols; ++k)
            {
                sum += m_data[i][k] * mat.m_data[k][j];
            }
            new_data[i][j] = sum;
        }
    }
    m_data = std::move(new_data);

    return *this;
}

Matrix &Matrix::operator+=(const Matrix &mat)
{
    if (num_cols != mat.num_cols || num_rows != mat.num_rows)
    {
        // Делаем невалидным
        num_rows = 0;
        num_cols = 0;
        m_data.clear();
        return *this;
    }
    std::vector<std::vector<double>> new_data(num_rows, std::vector<double>(num_cols, 0.0));
    for (size_t i = 0; i < num_rows; ++i)
    {
        for (size_t j = 0; j < num_cols; ++j)
        {
            new_data[i][j] = m_data[i][j] + mat.m_data[i][j];
        }
    }
    m_data = std::move(new_data);

    return *this;
}

Matrix &Matrix::operator-=(const Matrix &mat)
{
    if (num_cols != mat.num_cols || num_rows != mat.num_rows)
    {
        // Делаем невалидным
        num_rows = 0;
        num_cols = 0;
        m_data.clear();
        return *this;
    }
    std::vector<std::vector<double>> new_data(num_rows, std::vector<double>(num_cols, 0.0));
    for (size_t i = 0; i < num_rows; ++i)
    {
        for (size_t j = 0; j < num_cols; ++j)
        {
            new_data[i][j] = m_data[i][j] - mat.m_data[i][j];
        }
    }
    // todo что такое move и почему он тут используется
    m_data = std::move(new_data);

    return *this;
}

Matrix &Matrix::operator*=(double value)
{
    for (size_t i = 0; i < num_rows; ++i)
    {
        for (size_t j = 0; j < num_cols; ++j)
        {
            m_data[i][j] = m_data[i][j] * value;
        }
    }
    return *this;
}

Matrix &Matrix::operator/=(double value)
{
    for (size_t i = 0; i < num_rows; ++i)
    {
        for (size_t j = 0; j < num_cols; ++j)
        {
            m_data[i][j] = m_data[i][j] / value;
        }
    }
    return *this;
}

bool Matrix::isValid() const
{
    return (num_rows > 0 && num_cols > 0 && !m_data.empty());
}

void Matrix::resize(size_t rows, size_t cols)
{
    std::vector<std::vector<double>> new_data(rows, std::vector<double>(cols, 0.0));

    // Копируем старые данные (сколько влезет)
    size_t min_rows = std::min(rows, num_rows);
    size_t min_cols = std::min(cols, num_cols);

    for (size_t i = 0; i < min_rows; ++i)
    {
        for (size_t j = 0; j < min_cols; ++j)
        {
            new_data[i][j] = m_data[i][j];
        }
    }
    // Заменяем данные и обновляем размеры
    m_data = std::move(new_data);
    num_rows = rows;
    num_cols = cols;
}

const double &Matrix::coeffRef(size_t rowIdx, size_t colIdx) const
{
    if (rowIdx >= num_rows || colIdx >= num_cols)
    {
        throw std::out_of_range("Matrix indices out of range");
    }
    // Возвращает константную ссылку - элемент нельзя изменить
    return m_data[rowIdx][colIdx];
}

double &Matrix::coeffRef(size_t rowIdx, size_t colIdx)
{
    if (rowIdx >= num_rows || colIdx >= num_cols)
    {
        throw std::out_of_range("Matrix indices out of range");
    }
    // Возвращает обычную ссылку - элемент можно изменять
    return m_data[rowIdx][colIdx];
}

// ask это вообще имеется ввиду?
const double *Matrix::data() const
{
    if (m_data.empty() || m_data[0].empty())
    {
        return nullptr;
    }
    // Возвращаем указатель на первый элемент первой строки
    return m_data[0].data();
}

double *Matrix::data()
{
    if (m_data.empty() || m_data[0].empty())
    {
        return nullptr;
    }
    // Возвращаем указатель на первый элемент первой строки
    return m_data[0].data();
}

size_t Matrix::rows() const
{
    return num_rows;
}

size_t Matrix::cols() const
{
    return num_cols;
}

Matrix &Matrix::setIdentity()
{
    for (int i = 0; i < num_rows; ++i)
    {
        for (int j = 0; j < num_cols; ++j)
        {
            if (i == j)
                m_data[i][j] = 1;
            else
                m_data[i][j] = 0;
        }
    }
    return *this;
}

Matrix &Matrix::setZero()
{
    for (int i = 0; i < num_rows; ++i)
    {
        for (int j = 0; j < num_cols; ++j)
        {
            m_data[i][j] = 0;
        }
    }
    return *this;
}

Matrix &Matrix ::setConstants(double value)
{
    // todo stdfill
    for (int i = 0; i < num_rows; ++i)
    {
        for (int j = 0; j < num_cols; ++j)
        {
            m_data[i][j] = value;
        }
    }
    return *this;
}

Matrix &Matrix::setIdentity(size_t rows, size_t cols)
{
    this->resize(rows, cols);
    this->setIdentity();
    return *this;
}
Matrix &Matrix::setZero(size_t rows, size_t cols)
{
    this->resize(rows, cols);
    this->setZero();
    return *this;
}
Matrix &Matrix::setConstants(size_t rows, size_t cols, double value)
{
    this->resize(rows, cols);
    this->setConstants(value);
    return *this;
}

Matrix Matrix::identity(size_t rows, size_t cols)
{
    Matrix result(rows, cols);
    result.setIdentity();
    return result;
}
Matrix Matrix::zeros(size_t rows, size_t cols)
{
    Matrix result(rows, cols);
    result.setZero();
    return result;
}
Matrix Matrix::constants(size_t rows, size_t cols, double value)
{
    Matrix result(rows, cols);
    result.setConstants(value);
    return result;
}

Matrix Matrix::transpose() const
{
    if (!isValid())
    {
        return Matrix();
    }
    Matrix result(num_cols, num_rows);
    for (size_t i = 0; i < num_rows; ++i)
    {
        for (size_t j = 0; j < num_cols; ++j)
        {
            result.m_data[j][i] = m_data[i][j];
        }
    }
    return result;
}

double Matrix::det() const
{
    if (!isValid() || num_rows != num_cols)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
    if (num_rows == 1)
        return m_data[0][0];
    if (num_rows == 2)
        return m_data[0][0] * m_data[1][1] - m_data[0][1] * m_data[1][0];
    double determinant = 0.0;
    int sign = 1;

    for (size_t j = 0; j < num_cols; ++j)
    {
        Matrix minor(num_rows - 1, num_cols - 1);
        for (size_t minor_i = 0; minor_i < num_rows - 1; ++minor_i)
        {
            for (size_t minor_j = 0; minor_j < num_cols - 1; ++minor_j)
            {
                size_t source_j = (minor_j < j) ? minor_j : minor_j + 1;
                minor.m_data[minor_i][minor_j] = m_data[minor_i + 1][source_j];
            }
        }
        determinant += sign * m_data[0][j] * minor.det();
        sign = -sign;
    }
    return determinant;
}

Matrix Matrix::inverse() const
{
    // Проверяем, что матрица квадратная
    if (!isValid() || num_rows != num_cols)
    {
        return Matrix();
    }

    // Создаём расширенную матрицу [A|I]
    size_t n = num_rows;
    std::vector<std::vector<double>> augmented(n, std::vector<double>(2 * n, 0.0));

    // Заполняем левую часть исходной матрицей A
    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            augmented[i][j] = m_data[i][j];
        }
    }

    // Заполняем правую часть единичной матрицей I
    for (size_t i = 0; i < n; ++i)
    {
        augmented[i][n + i] = 1.0;
    }

    // Прямой ход метода Гаусса
    for (size_t pivot = 0; pivot < n; ++pivot)
    {
        // Ищем максимальный элемент в столбце для устойчивости
        size_t max_row = pivot;
        double max_val = std::abs(augmented[pivot][pivot]);

        for (size_t i = pivot + 1; i < n; ++i)
        {
            if (std::abs(augmented[i][pivot]) > max_val)
            {
                max_val = std::abs(augmented[i][pivot]);
                max_row = i;
            }
        }

        // Если все элементы в столбце нулевые - матрица вырождена
        if (max_val < 1e-10)
        {
            return Matrix();
        }

        // Меняем строки местами, если нужно
        if (max_row != pivot)
        {
            std::swap(augmented[pivot], augmented[max_row]);
        }

        // Нормализуем pivot-строку
        double pivot_val = augmented[pivot][pivot];
        for (size_t j = pivot; j < 2 * n; ++j)
        {
            augmented[pivot][j] /= pivot_val;
        }

        // Обнуляем столбец ниже и выше pivot-элемента
        for (size_t i = 0; i < n; ++i)
        {
            if (i != pivot)
            {
                double factor = augmented[i][pivot];
                for (size_t j = pivot; j < 2 * n; ++j)
                {
                    augmented[i][j] -= factor * augmented[pivot][j];
                }
            }
        }
    }

    // Извлекаем обратную матрицу из правой части
    Matrix result(n, n);
    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            result.m_data[i][j] = augmented[i][n + j];
        }
    }
    return result;
}

// реализация вне класса
// Можно реализовать через уже существующий operator*
Matrix operator*(double value, const Matrix &mat)
{
    if (!mat.isValid())
    {
        return Matrix();
    }
    return mat * value; // использует Matrix::operator*(double)
}