#pragma once
#include <string>
#include <vector>
#include <random>
#include <memory>
#include <stdexcept>

// Паттерн Strategy

// Enum для типов стратегий
enum class StrategyType
{
    SIMPLE,
    EMOTIONAL,
    QUESTION
};

class PostGenerationStrategy
{
public:
    virtual ~PostGenerationStrategy() = default;
    virtual std::string GeneratePost() = 0;

    template <typename T>
    static const T &GetRandomElement(const std::vector<T> &vec)
    {
        if (vec.empty())
        {
            throw std::runtime_error("Cannot get random element from empty vector");
        }
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dis(0, vec.size() - 1);
        return vec[dis(gen)];
    }
};

class SimplePostStrategy : public PostGenerationStrategy
{
public:
    std::string GeneratePost() override;
};

class EmotionalPostStrategy : public PostGenerationStrategy
{
public:
    std::string GeneratePost() override;
};

class QuestionPostStrategy : public PostGenerationStrategy
{
public:
    std::string GeneratePost() override;
};

// Фабрика стратегий
class StrategyFactory
{
public:
    static std::unique_ptr<PostGenerationStrategy> CreateStrategy(StrategyType type);
};

// Основной класс генератора с поддержкой разных стратегий
class PostGenerator
{
public:
    PostGenerator();
    ~PostGenerator() = default;

    void SetStrategy(std::unique_ptr<PostGenerationStrategy> strategy);
    void SetStrategy(StrategyType type);
    std::string GeneratePost();
    void RandomizeStrategy();

private:
    std::unique_ptr<PostGenerationStrategy> currentStrategy;
    std::vector<StrategyType> availableStrategies = {
        StrategyType::SIMPLE,
        StrategyType::EMOTIONAL,
        StrategyType::QUESTION};
    // генераторы случайных чисел
    std::random_device rd;
    std::mt19937 gen;
};