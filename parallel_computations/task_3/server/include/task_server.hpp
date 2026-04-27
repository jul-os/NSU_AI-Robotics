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
 *  Шаблонный сервер задач с пулом потоков
 *  T - Тип возвращаемого результата
 */
template <typename T>
class TaskServer
{
public:
    using id_type = std::size_t;          // ID задачи
    using task_type = std::function<T()>; // сгнатура задачи - функция без аргументов, возвращающая T

    // создается столько потоков сколько ядер, но не меньше 1
    //  ID начинаются с 1, 0 зарезервирован для ошибок
    explicit TaskServer(std::size_t worker_count = std::thread::hardware_concurrency())
        : worker_count_(worker_count > 0 ? worker_count : 1), next_id_(1) {}

    ~TaskServer() { stop(); }

    // Нельзя копировать, можно перемещать
    TaskServer(const TaskServer &) = delete;
    TaskServer &operator=(const TaskServer &) = delete;
    TaskServer(TaskServer &&) = default;
    TaskServer &operator=(TaskServer &&) = default;

    /** Запустить сервер (потоки-воркеры) */
    void start()
    {
        bool expected = false;
        if (!running_.compare_exchange_strong(expected, true))
        {
            return; // Уже запущен
        }

        stopping_.store(false); // сброс флага остановки
        workers_.reserve(worker_count_);
        for (std::size_t i = 0; i < worker_count_; ++i)
        {
            // создаём поток, который сразу запускает приватный метод worker_loop()
            workers_.emplace_back([this]
                                  { worker_loop(); });
        }
    }

    /** Остановить сервер и дождаться завершения задач */
    void stop()
    {
        if (!running_.load())
            return;

        stopping_.store(true);
        // Разбудить все воркеры, которые ждут на condition_variable
        // Иначе они могли бы спать вечно
        cv_task_.notify_all();

        for (auto &t : workers_)
        {
            if (t.joinable())
                t.join(); // Ждём завершения всех воркеров
        }
        workers_.clear();
        running_.store(false);
    }

    /**
     * Добавляет задачу в очередь
     * f - Функция без аргументов, возвращающая T
     * возвращает Уникальный ID задачи
     */
    template <typename F>
    id_type add_task(F &&f)
    {
        // нелья добавлять задачи, если сервер не запущен
        if (!running_.load())
        {
            throw std::runtime_error("TaskServer: server not started");
        }

        // увелчиваем счетчик, получаем новый ID
        const id_type id = next_id_.fetch_add(1, std::memory_order_relaxed);
        // memory_order_relaxed гарантирует только атомарность самой операции, но не накладывает ограничений на порядок выполнения других операций вокруг неё

        // packaged_task захватывает исключение внутри future
        std::packaged_task<T()> task([func = std::forward<F>(f)]() mutable -> T
                                     { return func(); });
        std::future<T> future = task.get_future();

        {
            // критическая секция для доступа к общим данным
            std::lock_guard<std::mutex> lock(mutex_);
            task_queue_.emplace_back(id, std::move(task));
            futures_.emplace(id, std::move(future));
        }
        cv_task_.notify_one(); // Будим одного воркера
        return id;
    }

    /**
     * Получить результат по ID (блокирующий)
     * future::get() можно вызвать только один раз!
     */
    T request_result(id_type id)
    {
        std::future<T> future;
        {
            // Блокируем мьютекс на время поиска в futures_
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = futures_.find(id);
            if (it == futures_.end())
            {
                throw std::out_of_range("TaskServer: result not found for id " + std::to_string(id));
            }
            future = std::move(it->second); // Забираем владение
            futures_.erase(it);             // Удаляем из карты сразу
        }
        return future.get(); // Блокируется до готовности
    }

    /** Удалить результат по ID  */
    bool erase_result(id_type id)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return futures_.erase(id) > 0;
    }

    /** показывает количество ожидающих задач */
    size_t pending_tasks() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return task_queue_.size();
    }

private:
    // цикл рабочего потока: ждем задачи, выполняем её, сохраняем результат
    void worker_loop()
    {
        while (true)
        {
            std::pair<id_type, std::packaged_task<T()>> task_item;

            {
                std::unique_lock<std::mutex> lock(mutex_);
                // Ждем, пока не будет задач или не придет сигнал остановки
                cv_task_.wait(lock, [this]
                              { return stopping_.load() || !task_queue_.empty(); });

                if (task_queue_.empty() && stopping_.load())
                {
                    return; // Выходим, если остановка и задач нет
                }
                if (task_queue_.empty())
                {
                    continue; // Ложное пробуждение
                }
                // если задачи и правда есть, то забираем её и удаляем из очереди
                task_item = std::move(task_queue_.front());
                task_queue_.pop_front();
            }

            // Выполняем задачу ВНЕ блокировки
            // задача выполняется без захвата мьютекса, чтобы не блокировать другие потоки на время вычислений
            try
            {
                task_item.second(); // Результат уже сохранён внутри packaged_task
            }
            catch (...)
            {
                // Исключение перехвачено future, клиент получит его при get()
            }
        }
    }

    const std::size_t worker_count_;
    mutable std::mutex mutex_;
    std::condition_variable cv_task_;

    // Очередь задач: быстро добавляем/удаляем с концов  - deque
    // Контейнер для очереди задач packaged_task, который
    // Выполняет пользовательскую функцию.
    // Сохраняет результат или исключение во внутреннем состоянии
    // Предоставляет std::future<R> для асинхронного получения результата
    std::deque<std::pair<id_type, std::packaged_task<T()>>> task_queue_;

    // Результаты: быстрый поиск/удаление по ID - unordered_map
    // Контейнер для хранения результатов по ID
    std::unordered_map<id_type, std::future<T>> futures_;

    std::vector<std::thread> workers_;
    std::atomic<id_type> next_id_;
    std::atomic<bool> running_{false};
    std::atomic<bool> stopping_{false};
};