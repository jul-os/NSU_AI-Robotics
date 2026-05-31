// heat_distribution.cpp
// Решение задачи о стационарном распределении тепла в пластинке
// Пятиточечный шаблон, метод Якоби, OpenACC, boost::program_options

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <chrono>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

// ============================================================================
// Параметры по умолчанию
// ============================================================================
const std::string DEFAULT_OUT_FILE = "result.dat";
const double DEFAULT_EPS = 1e-6;
const int DEFAULT_MAX_ITER = 1000000;
const int DEFAULT_NX = 128;
const int DEFAULT_NY = 128;

// Угловые значения температуры (по часовой стрелке: левый-нижний, правый-нижний, правый-верхний, левый-верхний)
const double CORNER_TL = 10.0; // top-left (0, 0)
const double CORNER_TR = 20.0; // top-right (0, Nx-1)
const double CORNER_BR = 30.0; // bottom-right (Ny-1, Nx-1)
const double CORNER_BL = 20.0; // bottom-left (Ny-1, 0)

// ============================================================================
// Инициализация граничных условий: линейная интерполяция между углами
// ============================================================================
void init_boundaries(std::vector<double> &grid, int Nx, int Ny)
{
    // Верхняя граница (i = 0): интерполяция между CORNER_TL и CORNER_TR
    for (int j = 0; j < Nx; ++j)
    {
        double t = static_cast<double>(j) / (Nx - 1);
        grid[j] = CORNER_TL * (1.0 - t) + CORNER_TR * t;
    }

    // Нижняя граница (i = Ny-1): интерполяция между CORNER_BL и CORNER_BR
    for (int j = 0; j < Nx; ++j)
    {
        double t = static_cast<double>(j) / (Nx - 1);
        grid[(Ny - 1) * Nx + j] = CORNER_BL * (1.0 - t) + CORNER_BR * t;
    }

    // Левая граница (j = 0): интерполяция между CORNER_TL и CORNER_BL
    for (int i = 1; i < Ny - 1; ++i)
    {
        double t = static_cast<double>(i) / (Ny - 1);
        grid[i * Nx] = CORNER_TL * (1.0 - t) + CORNER_BL * t;
    }

    // Правая граница (j = Nx-1): интерполяция между CORNER_TR и CORNER_BR
    for (int i = 1; i < Ny - 1; ++i)
    {
        double t = static_cast<double>(i) / (Ny - 1);
        grid[i * Nx + (Nx - 1)] = CORNER_TR * (1.0 - t) + CORNER_BR * t;
    }
}

// ============================================================================
// Инициализация источников/стоков тепла (вектор b)
// ============================================================================
void init_sources(std::vector<double> &b, int Nx, int Ny)
{
    // Заполняем нулями
    std::fill(b.begin(), b.end(), 0.0);

    // Добавляем несколько произвольных источников/стоков в диапазоне [-50, 50]
    // Для воспроизводимости используем фиксированные позиции
    b[(Ny / 2) * Nx + Nx / 3] = 10.0;            // источник
    b[(Ny * 2 / 3) * Nx + (Nx * 2 / 3)] = -25.0; // сток
    b[Nx + 1] = 15.0;
    b[(Ny - 2) * Nx + (Nx - 2)] = -10.0;
}

// ============================================================================
// Вычисление нормы вектора (L2)
// ============================================================================
double compute_norm(const std::vector<double> &vec, int size)
{
    double sum = 0.0;
#pragma acc parallel loop reduction(+ : sum) if (0)
    for (int i = 0; i < size; ++i)
    {
        sum += vec[i] * vec[i];
    }
    return std::sqrt(sum);
}

// ============================================================================
// Основной итерационный процесс (метод Якоби с пятиточечным шаблоном)
// Возвращает: {число итераций, достигнутая ошибка}
// ============================================================================
std::pair<int, double> solve_jacobi(
    std::vector<double> &grid,
    const std::vector<double> &b,
    int Nx, int Ny,
    double eps,
    int max_iter)
{
    const int N = Nx * Ny;
    std::vector<double> grid_new(N);

    // Копируем начальные значения (внутренние точки = 0, границы уже заданы)
    grid_new = grid;

    double error = 0.0;
    int iter = 0;

// OpenACC: директивы для ускорения на GPU
#pragma acc data copyin(b[0 : N]) copy(grid[0 : N], grid_new[0 : N])
    {
        for (iter = 0; iter < max_iter; ++iter)
        {
            error = 0.0;

// Обновление внутренних точек сетки (пятиточечный шаблон)
#pragma acc parallel loop collapse(2) reduction(max : error)
            for (int i = 1; i < Ny - 1; ++i)
            {
                for (int j = 1; j < Nx - 1; ++j)
                {
                    int idx = i * Nx + j;
                    // Уравнение: 4*T[i][j] - T[i-1][j] - T[i+1][j] - T[i][j-1] - T[i][j+1] = b[i][j]
                    // => T[i][j] = (T[i-1][j] + T[i+1][j] + T[i][j-1] + T[i][j+1] - b[i][j]) / 4
                    grid_new[idx] = 0.25 * (grid[idx - Nx] + // верх
                                            grid[idx + Nx] + // низ
                                            grid[idx - 1] +  // лево
                                            grid[idx + 1] -  // право
                                            b[idx]);

                    // Вычисление ошибки (разница между итерациями)
                    double diff = std::abs(grid_new[idx] - grid[idx]);
                    if (diff > error)
                        error = diff;
                }
            }

// Копирование новых значений обратно (swap)
#pragma acc parallel loop
            for (int i = 1; i < Ny - 1; ++i)
            {
                for (int j = 1; j < Nx - 1; ++j)
                {
                    int idx = i * Nx + j;
                    grid[idx] = grid_new[idx];
                }
            }

            // Проверка сходимости
            if (error < eps)
            {
                ++iter; // учитываем текущую итерацию
                break;
            }
        }
    }

    return {iter, error};
}

// ============================================================================
// Вывод сетки в терминал (для малых размеров 10x10, 13x13)
// ============================================================================
void print_grid_small(const std::vector<double> &grid, int Nx, int Ny)
{
    std::cout << "\n=== Temperature Grid (" << Ny << "x" << Nx << ") ===" << std::endl;
    std::cout.precision(2);
    std::cout << std::fixed;

    for (int i = 0; i < Ny; ++i)
    {
        for (int j = 0; j < Nx; ++j)
        {
            std::cout << std::setw(6) << grid[i * Nx + j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "=====================================\n"
              << std::endl;
}

// ============================================================================
// Сохранение результата в бинарный файл для gnuplot
// ============================================================================
void save_to_binary(const std::vector<double> &grid, const std::string &filename)
{
    std::ofstream out(filename, std::ios::binary);
    if (!out)
    {
        std::cerr << "Error: cannot open file " << filename << " for writing" << std::endl;
        return;
    }
    out.write(reinterpret_cast<const char *>(grid.data()), grid.size() * sizeof(double));
    out.close();
    std::cout << "Result saved to " << filename << std::endl;
}

// ============================================================================
// Парсинг аргументов командной строки
// ============================================================================
struct Config
{
    int Nx = DEFAULT_NX;
    int Ny = DEFAULT_NY;
    double eps = DEFAULT_EPS;
    int max_iter = DEFAULT_MAX_ITER;
    std::string out_file = DEFAULT_OUT_FILE;
    bool print_small = false;
};

Config parse_args(int argc, char **argv)
{
    Config cfg;

    po::options_description desc("Available options");
    desc.add_options()("help,h", "Show this help message")("nx", po::value<int>(&cfg.Nx)->default_value(DEFAULT_NX), "Grid size along X axis")("ny", po::value<int>(&cfg.Ny)->default_value(DEFAULT_NY), "Grid size along Y axis")("eps", po::value<double>(&cfg.eps)->default_value(DEFAULT_EPS), "Convergence tolerance")("max-iter", po::value<int>(&cfg.max_iter)->default_value(DEFAULT_MAX_ITER), "Maximum iterations")("output,o", po::value<std::string>(&cfg.out_file)->default_value(DEFAULT_OUT_FILE), "Output binary file")("print,p", po::bool_switch(&cfg.print_small), "Print small grids (10x10, 13x13) to terminal");

    po::variables_map vm;
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }
    catch (const po::error &e)
    {
        std::cerr << "Error parsing arguments: " << e.what() << std::endl;
        std::cerr << desc << std::endl;
        exit(1);
    }

    if (vm.count("help"))
    {
        std::cout << "Heat Distribution Solver\n\n"
                  << desc << std::endl;
        std::cout << "Example usage:\n";
        std::cout << "  ./heat_solver --nx=128 --ny=128 --eps=1e-6 --max-iter=1000000\n";
        std::cout << "  ./heat_solver --nx=10 --ny=10 --print\n";
        std::cout << "\nGnuplot command for visualization:\n";
        std::cout << "  gnuplot -e \"plot 'result.dat' binary array=("
                  << cfg.Ny << "," << cfg.Nx << ") format='%lf' with image; pause -1\"\n";
        exit(0);
    }

    // Валидация параметров
    if (cfg.Nx < 3 || cfg.Ny < 3)
    {
        std::cerr << "Error: grid dimensions must be at least 3x3" << std::endl;
        exit(1);
    }

    return cfg;
}

// ============================================================================
// Основная функция
// ============================================================================
int main(int argc, char **argv)
{
    // Парсинг аргументов
    Config cfg = parse_args(argc, argv);
    const int N = cfg.Nx * cfg.Ny;

    std::cout << "Heat Distribution Solver" << std::endl;
    std::cout << "Grid: " << cfg.Ny << "x" << cfg.Nx << " = " << N << " nodes" << std::endl;
    std::cout << "Tolerance: " << cfg.eps << ", Max iterations: " << cfg.max_iter << std::endl;

    // Выделение памяти
    std::vector<double> grid(N, 0.0);    // Температура в узлах
    std::vector<double> sources(N, 0.0); // Источники/стоки (вектор b)

    // Инициализация
    init_sources(sources, cfg.Nx, cfg.Ny);
    init_boundaries(grid, cfg.Nx, cfg.Ny);

    // Замер времени
    auto start_time = std::chrono::high_resolution_clock::now();

    // Решение
    auto [iterations, final_error] = solve_jacobi(
        grid, sources, cfg.Nx, cfg.Ny, cfg.eps, cfg.max_iter);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    // Вывод результатов
    std::cout << "\n=== Results ===" << std::endl;
    std::cout << "Iterations: " << iterations << " / " << cfg.max_iter << std::endl;
    std::cout << "Final error: " << final_error << std::endl;
    std::cout << "Time elapsed: " << elapsed.count() << " seconds" << std::endl;

    // Вывод малых сеток для проверки
    if (cfg.print_small && ((cfg.Nx == 10 && cfg.Ny == 10) || (cfg.Nx == 13 && cfg.Ny == 13)))
    {
        print_grid_small(grid, cfg.Nx, cfg.Ny);
    }

    // Сохранение результата
    save_to_binary(grid, cfg.out_file);

    return 0;
}