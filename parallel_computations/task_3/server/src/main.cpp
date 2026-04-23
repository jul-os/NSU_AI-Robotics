#include "task_server.hpp"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace fs = std::filesystem;

// ========== Функции-задачи (шаблоны из условия) ==========
template <typename T>
T fun_sin(T arg) { return std::sin(arg); }

template <typename T>
T fun_sqrt(T arg) { return std::sqrt(arg); }

template <typename T>
T fun_pow(T base, T exp) { return std::pow(base, exp); }

// ========== Конфигурация клиента ==========
struct ClientConfig {
    std::string name;
    uint32_t seed;
    // Генерация аргументов + вычисление ожидаемого результата + создание задачи
    std::function<void(std::mt19937&, double&, double&, double&, std::function<double()>&)> make_task;
};

// Клиент 1: sin(arg)
ClientConfig make_sin_client() {
    return {
        "sin", 0x51A1u,
        [](std::mt19937& rng, double& a, double& b, double& expected, std::function<double()>& task) {
            std::uniform_real_distribution<double> dist(-1000.0, 1000.0);
            a = dist(rng); b = 0.0;
            expected = fun_sin(a);
            task = [a]() { return fun_sin(a); };
        }
    };
}

// Клиент 2: sqrt(arg)
ClientConfig make_sqrt_client() {
    return {
        "sqrt", 0x52B2u,
        [](std::mt19937& rng, double& a, double& b, double& expected, std::function<double()>& task) {
            std::uniform_real_distribution<double> dist(0.0, 1e12);
            a = dist(rng); b = 0.0;
            expected = fun_sqrt(a);
            task = [a]() { return fun_sqrt(a); };
        }
    };
}

// Клиент 3: pow(base, exp)
ClientConfig make_pow_client() {
    return {
        "pow", 0x53C3u,
        [](std::mt19937& rng, double& a, double& b, double& expected, std::function<double()>& task) {
            std::uniform_real_distribution<double> base_dist(0.0, 1000.0);
            std::uniform_real_distribution<double> exp_dist(-10.0, 10.0);
            a = base_dist(rng); b = exp_dist(rng);
            expected = fun_pow(a, b);
            task = [a, b]() { return fun_pow(a, b); };
        }
    };
}

// ========== Запуск одного клиента ==========
void run_client(TaskServer<double>& server, size_t N, const fs::path& out_dir, const ClientConfig& cfg) {
    std::mt19937 rng(cfg.seed);
    fs::path filepath = out_dir / ("client_" + cfg.name + ".txt");

    std::ofstream out(filepath);
    if (!out) throw std::runtime_error("Cannot open: " + filepath.string());

    out << "# Client: " << cfg.name << "\n"
        << "# Tasks: " << N << "\n"
        << "# Format: id arg1 arg2 expected actual\n"
        << std::scientific << std::setprecision(15);

    // Храним метаданные задач ДО отправки, чтобы потом сопоставить результат
    struct TaskMeta {
        size_t id;
        double arg1, arg2, expected;
    };
    std::vector<TaskMeta> metas;
    metas.reserve(N);

    // Этап 1: генерация + отправка задач (запоминаем ID!)
    for (size_t i = 0; i < N; ++i) {
        double a, b, expected;
        std::function<double()> task;
        
        cfg.make_task(rng, a, b, expected, task);
        
        size_t id = server.add_task(std::move(task)); // ← ОДИН вызов add_task
        metas.push_back({id, a, b, expected});
    }

    // Этап 2: получение результатов + запись в файл
    for (const auto& m : metas) {
        double actual = server.request_result(m.id); // блокирует, пока не готово
        out << m.id << " " << m.arg1 << " " << m.arg2 << " " << m.expected << " " << actual << "\n";
        server.erase_result(m.id); // освобождаем память
    }
}

// ========== MAIN ==========
int main(int argc, char* argv[]) {
    size_t N = 100; // 5 < N < 10000
    if (argc > 1) {
        N = std::stoul(argv[1]);
        if (N <= 5 || N >= 10000) {
            std::cerr << "Error: N must be in (5, 10000), got " << N << "\n";
            return 1;
        }
    }

    fs::path out_dir = "results";
    fs::create_directories(out_dir);

    // Сервер с пулом потоков (по умолчанию = число ядер)
    TaskServer<double> server;
    server.start();

    // Три клиента из условия
    std::vector<ClientConfig> configs = {
        make_sin_client(),
        make_sqrt_client(),
        make_pow_client()
    };

    // Запускаем клиентов в отдельных потоках
    std::vector<std::thread> client_threads;
    for (const auto& cfg : configs) {
        client_threads.emplace_back([&server, N, &out_dir, cfg]() {
            run_client(server, N, out_dir, cfg);
        });
    }

    for (auto& t : client_threads) t.join();
    server.stop();

    std::cout << "✓ Done. Results: " << fs::absolute(out_dir) << "\n";
    return 0;
}