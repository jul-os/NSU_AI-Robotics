#include "simulation.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

Simulation::Simulation()
{
    // Конструктор
}

std::vector<std::string> Simulation::SplitString(const std::string &str, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);

    while (std::getline(tokenStream, token, delimiter))
    {
        // Убираем пробелы в начале и конце
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);

        if (!token.empty())
        {
            tokens.push_back(token);
        }
    }

    return tokens;
}

std::chrono::system_clock::time_point Simulation::ParseTime(const std::string &timeStr)
{
    return StringToTime(timeStr);
}

void Simulation::ProcessFriendsLine(const std::string &line)
{
    auto tokens = SplitString(line, ' ');
    if (tokens.empty())
        return;

    std::string userId = tokens[0];
    auto user = network.GetUser(userId);
    if (!user)
    {
        user = network.AddUser(userId);
    }

    // Обрабатываем друзей пользователя
    for (size_t i = 1; i < tokens.size(); ++i)
    {
        std::string friendId = tokens[i];
        auto friendUser = network.GetUser(friendId);
        if (!friendUser)
        {
            friendUser = network.AddUser(friendId);
        }

        network.CreateFriendship(userId, friendId);
    }
}

void Simulation::ProcessPostsLine(const std::string &line)
{
    // Формат: 2014-10-20T08:00:00 vasya Блин, сегодня опять понедельник :(
    // Находим первый и второй пробел
    size_t firstSpace = line.find(' ');
    if (firstSpace == std::string::npos)
        return;

    size_t secondSpace = line.find(' ', firstSpace + 1);
    if (secondSpace == std::string::npos)
        return;

    std::string timeStr = line.substr(0, firstSpace);
    std::string authorId = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    std::string content = line.substr(secondSpace + 1);

    try
    {
        auto timestamp = ParseTime(timeStr);
        network.CreatePost(timestamp, authorId, content);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error parsing post line: " << e.what() << std::endl;
        std::cerr << "Line: " << line << std::endl;
    }
}

void Simulation::LoadFromFiles(const std::string &friendsFile, const std::string &postsFile)
{
    std::cout << "Loading social network data..." << std::endl;

    // Загружаем граф друзей
    std::ifstream friendsStream(friendsFile);
    if (!friendsStream.is_open())
    {
        throw std::runtime_error("Cannot open friends file: " + friendsFile);
    }

    std::string line;
    int friendsCount = 0;
    while (std::getline(friendsStream, line))
    {
        if (!line.empty())
        {
            ProcessFriendsLine(line);
            friendsCount++;
        }
    }
    friendsStream.close();
    std::cout << "Processed " << friendsCount << " friendship lines" << std::endl;

    // Загружаем посты
    std::ifstream postsStream(postsFile);
    if (!postsStream.is_open())
    {
        throw std::runtime_error("Cannot open posts file: " + postsFile);
    }

    int postsCount = 0;
    while (std::getline(postsStream, line))
    {
        if (!line.empty())
        {
            ProcessPostsLine(line);
            postsCount++;
        }
    }
    postsStream.close();
    std::cout << "Processed " << postsCount << " posts" << std::endl;

    std::cout << "Social network loaded successfully!" << std::endl;
}

void Simulation::GenerateRandomActivity(int numPosts)
{
    // Старая версия для обратной совместимости - получаем пользователей из сети
    auto users = network.GetAllUserIds();

    if (users.empty())
    {
        std::cerr << "Error: No users found in the network! Please add users first." << std::endl;
        return;
    }

    GenerateRandomActivity(numPosts, users);
}

void Simulation::GenerateRandomActivity(int numPosts, const std::vector<std::string> &users)
{
    if (users.empty())
    {
        std::cerr << "Error: Cannot generate posts for empty user list!" << std::endl;
        return;
    }

    std::cout << "Generating " << numPosts << " random posts for " << users.size() << " users..." << std::endl;

    // Генератор случайных чисел для временных меток
    std::random_device rd;
    std::mt19937 gen(rd());

    // Генерируем посты за последние 7 дней
    auto now = std::chrono::system_clock::now();
    auto weekAgo = now - std::chrono::hours(24 * 7);
    std::uniform_int_distribution<int> timeDist(0, 24 * 7 * 3600); // секунды за неделю

    for (int i = 0; i < numPosts; ++i)
    {
        // Случайный пользователь из переданного списка
        std::uniform_int_distribution<size_t> userDist(0, users.size() - 1);
        std::string author = users[userDist(gen)];

        // Случайное время в пределах недели
        auto randomTime = weekAgo + std::chrono::seconds(timeDist(gen));

        // Генерируем случайный пост
        std::string content = postGenerator.GeneratePost();

        // Создаем пост в сети
        network.CreatePost(randomTime, author, content);

        // Случайно меняем стратегию для разнообразия
        if (i % 5 == 0)
        {
            postGenerator.RandomizeStrategy();
        }
    }

    std::cout << "Generated " << numPosts << " random posts for users: ";
    for (size_t i = 0; i < users.size(); ++i)
    {
        std::cout << users[i];
        if (i < users.size() - 1)
            std::cout << ", ";
    }
    std::cout << std::endl;
}

void Simulation::RunDemo()
{
    std::cout << "\n=== Social Network Simulation Demo ===" << std::endl;

    // Демонстрация загрузки данных
    std::cout << "\n1. Loading sample data..." << std::endl;
    try
    {
        // Создаем тестовые файлы на лету для демонстрации
        std::ofstream friendsFile("demo_friends.txt");
        friendsFile << "vasya kolya petya sasha\n";
        friendsFile << "kolya vasya cool123\n";
        friendsFile << "sasha vasya petya\n";
        friendsFile << "cool123 kolya\n";
        friendsFile.close();

        std::ofstream postsFile("demo_posts.txt");
        postsFile << "2014-10-20T08:00:00 vasya Привет всем!\n";
        postsFile << "2014-10-20T08:05:00 kolya Доброе утро!\n";
        postsFile << "2014-10-20T09:00:00 sasha Как ваши дела?\n";
        postsFile << "2014-10-20T09:00:01 cool123 Блин, сегодня опять понедельник :(\n";
        postsFile.close();

        LoadFromFiles("demo_friends.txt", "demo_posts.txt");
    }
    catch (const std::exception &e)
    {
        std::cout << "Error loading demo data: " << e.what() << std::endl;
    }

    // Демонстрация лент
    std::cout << "\n2. Demo feeds for user 'vasya':" << std::endl;

    auto friendsFeed = network.GetFriendsFeed("vasya");
    std::cout << "Friends feed (" << friendsFeed.size() << " posts):" << std::endl;
    for (const auto &post : friendsFeed)
    {
        std::cout << "  " << post->toString() << std::endl;
    }

    auto extendedFeed = network.GetExtendedFeed("vasya");
    std::cout << "Extended feed (" << extendedFeed.size() << " posts):" << std::endl;
    for (const auto &post : extendedFeed)
    {
        std::cout << "  " << post->toString() << std::endl;
    }

    // Демонстрация рукопожатий
    std::cout << "\n3. Handshakes calculation:" << std::endl;
    int handshakes = network.CalculateHandshakes("vasya", "cool123");
    std::cout << "Handshakes between vasya and cool123: " << handshakes << std::endl;

    handshakes = network.CalculateHandshakes("vasya", "petya");
    std::cout << "Handshakes between vasya and petya: " << handshakes << std::endl;

    // Демонстрация генерации случайных постов
    std::cout << "\n4. Generating random activity..." << std::endl;
    GenerateRandomActivity(3);

    // Показываем обновленные ленты
    friendsFeed = network.GetFriendsFeed("vasya");
    std::cout << "Updated friends feed (" << friendsFeed.size() << " posts):" << std::endl;
    for (const auto &post : friendsFeed)
    {
        std::cout << "  " << post->toString() << std::endl;
    }

    std::cout << "\n=== Demo completed ===" << std::endl;
}