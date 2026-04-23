#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

/**
 * @brief Шаблонный сервер задач с пулом потоков
 * @tparam T Тип возвращаемого результата
 */
template <typename T>
class TaskServer {
public:
    using id_type = std::size_t;
    using task_type = std::function<T()>;

    explicit TaskServer(std::size_t worker_count = std::thread::hardware_concurrency())
        : worker_count_(worker_count > 0 ? worker_count : 1), next_id_(1) {}

    ~TaskServer() { stop(); }

    // Нельзя копировать, можно перемещать
    TaskServer(const TaskServer&) = delete;
    TaskServer& operator=(const TaskServer&) = delete;
    TaskServer(TaskServer&&) = default;
    TaskServer& operator=(TaskServer&&) = default;

    /** Запустить сервер (потоки-воркеры) */
    void start() {
        bool expected = false;
        if (!running_.compare_exchange_strong(expected, true)) {
            return; // Уже запущен
        }

        stopping_.store(false);
        workers_.reserve(worker_count_);
        for (std::size_t i = 0; i < worker_count_; ++i) {
            workers_.emplace_back([this] { worker_loop(); });
        }
    }

    /** Остановить сервер и дождаться завершения задач */
    void stop() {
        if (!running_.load()) return;

        stopping_.store(true);
        cv_task_.notify_all(); // Разбудить все воркеры

        for (auto& t : workers_) {
            if (t.joinable()) t.join();
        }
        workers_.clear();
        running_.store(false);
    }

    /**
     * @brief Добавить задачу в очередь
     * @param f Функция без аргументов, возвращающая T
     * @return Уникальный ID задачи
     */
    template <typename F>
    id_type add_task(F&& f) {
        if (!running_.load()) {
            throw std::runtime_error("TaskServer: server not started");
        }

        const id_type id = next_id_.fetch_add(1, std::memory_order_relaxed);

        // packaged_task захватывает исключение внутри future
        std::packaged_task<T()> task([func = std::forward<F>(f)]() mutable -> T {
            return func();
        });
        std::future<T> future = task.get_future();

        {
            std::lock_guard<std::mutex> lock(mutex_);
            task_queue_.emplace_back(id, std::move(task));
            futures_.emplace(id, std::move(future));
        }
        cv_task_.notify_one(); // Будим одного воркера
        return id;
    }

    /**
     * @brief Получить результат по ID (блокирующий)
     * @warning future::get() можно вызвать только один раз!
     */
    T request_result(id_type id) {
        std::future<T> future;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = futures_.find(id);
            if (it == futures_.end()) {
                throw std::out_of_range("TaskServer: result not found for id " + std::to_string(id));
            }
            future = std::move(it->second); // Забираем владение
            futures_.erase(it); // Удаляем из карты сразу
        }
        return future.get(); // Блокируется до готовности
    }

    /** Удалить результат по ID (если нужно освободить память) */
    bool erase_result(id_type id) {
        std::lock_guard<std::mutex> lock(mutex_);
        return futures_.erase(id) > 0;
    }

    /** Количество ожидающих задач */
    size_t pending_tasks() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return task_queue_.size();
    }

private:
    void worker_loop() {
        while (true) {
            std::pair<id_type, std::packaged_task<T()>> task_item;

            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_task_.wait(lock, [this] {
                    return stopping_.load() || !task_queue_.empty();
                });

                if (task_queue_.empty() && stopping_.load()) {
                    return; // Выходим, если остановка и задач нет
                }
                if (task_queue_.empty()) {
                    continue; // Ложное пробуждение
                }

                task_item = std::move(task_queue_.front());
                task_queue_.pop_front();
            }

            // Выполняем задачу ВНЕ блокировки
            try {
                task_item.second(); // Результат уже сохранён внутри packaged_task
            } catch (...) {
                // Исключение перехвачено future, клиент получит его при get()
            }
        }
    }

    const std::size_t worker_count_;
    mutable std::mutex mutex_;
    std::condition_variable cv_task_;

    // Очередь задач: быстро добавляем/удаляем с концов → deque
    std::deque<std::pair<id_type, std::packaged_task<T()>>> task_queue_;

    // Результаты: быстрый поиск/удаление по ID → unordered_map
    std::unordered_map<id_type, std::future<T>> futures_;

    std::vector<std::thread> workers_;
    std::atomic<id_type> next_id_;
    std::atomic<bool> running_{false};
    std::atomic<bool> stopping_{false};
};