#include "post_generator.hpp"
#include <algorithm>
#include <stdexcept>

std::string SimplePostStrategy::GeneratePost()
{
    static std::vector<std::string> templates = {
        "Сегодня был продуктивный день",
        "Отличная погода сегодня",
        "Интересный день выдался",
        "Новые планы на неделю"};

    return PostGenerationStrategy::GetRandomElement(templates);
}

std::string EmotionalPostStrategy::GeneratePost()
{
    static std::vector<std::string> positive = {
        "Ура! 😊 ", "Вау! 🎉 ", "Отлично! 👍 ", "Прекрасно! 🌟 "};
    static std::vector<std::string> negative = {
        "О нет... 😞 ", "Ужасно... 😔 ", "Разочарован... 😕 ", "Сложный день... 🌧️ "};
    static std::vector<std::string> events = {
        "сегодняшний день", "эта неделя", "погода", "работа", "учеба"};

    if (PostGenerationStrategy::GetRandomElement(std::vector<int>{0, 1}) == 0)
    {
        return PostGenerationStrategy::GetRandomElement(positive) +
               PostGenerationStrategy::GetRandomElement(events);
    }
    else
    {
        return PostGenerationStrategy::GetRandomElement(negative) +
               PostGenerationStrategy::GetRandomElement(events);
    }
}

std::string QuestionPostStrategy::GeneratePost()
{
    static std::vector<std::string> questions = {
        "Что думаете о сегодняшней погоде?",
        "Какие планы на выходные?",
        "Посоветуйте хороший фильм",
        "Как провели день?",
        "Что нового и интересного?"};

    return PostGenerationStrategy::GetRandomElement(questions);
}

std::unique_ptr<PostGenerationStrategy> StrategyFactory::CreateStrategy(StrategyType type)
{
    switch (type)
    {
    case StrategyType::SIMPLE:
        return std::make_unique<SimplePostStrategy>();
    case StrategyType::EMOTIONAL:
        return std::make_unique<EmotionalPostStrategy>();
    case StrategyType::QUESTION:
        return std::make_unique<QuestionPostStrategy>();
    default:
        return std::make_unique<SimplePostStrategy>();
    }
}

PostGenerator::PostGenerator() : gen(rd())
{
    SetStrategy(StrategyType::SIMPLE);
}

void PostGenerator::SetStrategy(std::unique_ptr<PostGenerationStrategy> strategy)
{
    currentStrategy = std::move(strategy);
}

void PostGenerator::SetStrategy(StrategyType type)
{
    currentStrategy = StrategyFactory::CreateStrategy(type);
}

std::string PostGenerator::GeneratePost()
{
    if (!currentStrategy)
    {
        SetStrategy(StrategyType::SIMPLE);
    }
    return currentStrategy->GeneratePost();
}

void PostGenerator::RandomizeStrategy()
{
    if (availableStrategies.empty())
        return;
    std::uniform_int_distribution<size_t> dis(0, availableStrategies.size() - 1);
    StrategyType randomStrategy = availableStrategies[dis(gen)];
    SetStrategy(randomStrategy);
}