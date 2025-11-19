#include <iostream>
#include <memory>
#include "simulation.hpp"

// ./social_network load ../friends.txt ../posts.txt
void PrintUsage()
{
    std::cout << "Usage:\n"
              << "  ./social_network demo                    - Run demonstration\n"
              << "  ./social_network load <friends> <posts>  - Load from files\n"
              << "  ./social_network generate <count>        - Generate random posts\n"
              << "  ./social_network interactive             - Interactive mode\n";
}

void RunDemo()
{
    std::cout << "=== Social Network Demo ===\n"
              << std::endl;

    Simulation sim;
    sim.RunDemo();
}

void RunFileLoading(const std::string &friendsFile, const std::string &postsFile)
{
    std::cout << "=== Loading from files ===\n"
              << std::endl;

    Simulation sim;
    try
    {
        sim.LoadFromFiles(friendsFile, postsFile);
        std::cout << "\nFiles loaded successfully!" << std::endl;

        auto &network = sim.GetNetwork();

        // Получаем всех пользователей из загруженных данных
        auto allUsers = network.GetAllUserIds();

        if (allUsers.empty())
        {
            std::cout << "No users found in the loaded data!" << std::endl;
            return;
        }

        std::cout << "\nLoaded users (" << allUsers.size() << "): ";
        for (size_t i = 0; i < allUsers.size(); ++i)
        {
            std::cout << allUsers[i];
            if (i < allUsers.size() - 1)
                std::cout << ", ";
        }
        std::cout << std::endl;

        // Используем первого пользователя для демонстрации
        std::string demoUser = allUsers[0];
        std::cout << "\n=== Demonstration for user '" << demoUser << "' ===" << std::endl;

        // 1. Показываем ленты
        std::cout << "\n1. Feeds:" << std::endl;

        auto friendsFeed = network.GetFriendsFeed(demoUser);
        std::cout << "Friends feed (" << friendsFeed.size() << " posts):" << std::endl;
        for (const auto &post : friendsFeed)
        {
            std::cout << "  " << post->toString() << std::endl;
        }

        auto extendedFeed = network.GetExtendedFeed(demoUser);
        std::cout << "\nExtended feed (" << extendedFeed.size() << " posts):" << std::endl;
        for (const auto &post : extendedFeed)
        {
            std::cout << "  " << post->toString() << std::endl;
        }

        // 2. Показываем рукопожатия
        std::cout << "\n2. Handshakes:" << std::endl;

        // С другими пользователями
        for (size_t i = 1; i < std::min(allUsers.size(), size_t(4)); ++i)
        {
            std::string otherUser = allUsers[i];
            int handshakes = network.CalculateHandshakes(demoUser, otherUser);
            std::cout << "With " << otherUser << ": " << handshakes << " handshake(s)" << std::endl;
        }

        // С самим собой
        int selfHandshakes = network.CalculateHandshakes(demoUser, demoUser);
        std::cout << "With self: " << selfHandshakes << " handshake(s)" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void RunGeneration(int count)
{
    std::cout << "=== Generating random activity ===\n"
              << std::endl;

    Simulation sim;

    try
    {
        auto &network = sim.GetNetwork();

        // 1. Сначала СОЗДАЕМ пользователей
        std::vector<std::string> users = {"vasya", "kolya", "petya", "sasha", "cool123"};
        for (const auto &userId : users)
        {
            network.AddUser(userId);
        }

        // 2. Потом устанавливаем дружбу
        network.CreateFriendship("vasya", "kolya");
        network.CreateFriendship("vasya", "petya");
        network.CreateFriendship("kolya", "sasha");
        network.CreateFriendship("kolya", "cool123");

        std::cout << "Created social graph:" << std::endl;
        std::cout << "vasya -> kolya, petya" << std::endl;
        std::cout << "kolya -> sasha, cool123" << std::endl;

        // 3. Генерируем случайные посты
        sim.GenerateRandomActivity(count, users);

        // 4. Показываем результаты для всех пользователей
        std::cout << "\n=== Generated feeds ===" << std::endl;

        for (const auto &user : users)
        {
            auto friendsFeed = network.GetFriendsFeed(user);
            std::cout << "\nFriends feed for " << user << " (" << friendsFeed.size() << " posts):" << std::endl;
            for (const auto &post : friendsFeed)
            {
                std::cout << "  " << post->toString() << std::endl;
            }
        }

        // 5. Покажем статистику
        std::cout << "\n=== Statistics ===" << std::endl;
        int totalPosts = 0;
        for (const auto &user : users)
        {
            auto userObj = network.GetUser(user);
            if (userObj)
            {
                int userPosts = userObj->GetPosts().size();
                std::cout << user << ": " << userPosts << " posts" << std::endl;
                totalPosts += userPosts;
            }
        }
        std::cout << "Total posts generated: " << totalPosts << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void InteractiveMode()
{
    std::cout << "=== Interactive Mode ===\n"
              << std::endl;

    Simulation sim;
    std::string command;

    std::cout << "Available commands:\n"
              << "  load <friendsFile> <postsFile>   - Load from files\n"
              << "  generate <count>                   - Generate random posts\n"
              << "  feeds <userId>                    - Show feeds for user\n"
              << "  handshakes <user1> <user2>         - Calculate handshakes\n"
              << "  addUser <userId>                 - Add new user\n"
              << "  addFriendship <user1> <user2>     - Add friendship\n"
              << "  removeFriendship <user1> <user2>  - Remove friendship\n"
              << "  friends <userId>                  - Show user's friends\n"
              << "  addPost <userId> <content>       - Add post (use quotes for content)\n"
              << "  users                              - Show all users\n"
              << "  demo                               - Run demo\n"
              << "  exit                               - Exit\n"
              << std::endl;

    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command == "exit")
        {
            break;
        }
        else if (command == "demo")
        {
            sim.RunDemo();
        }
        else if (command == "users")
        {
            auto &network = sim.GetNetwork();
            auto userIds = network.GetAllUserIds();
            std::cout << "Users (" << userIds.size() << "): ";
            for (size_t i = 0; i < userIds.size(); ++i)
            {
                std::cout << userIds[i];
                if (i < userIds.size() - 1)
                    std::cout << ", ";
            }
            std::cout << std::endl;
        }
        else if (command.find("load ") == 0)
        {
            std::string rest = command.substr(5);
            size_t firstSpace = rest.find(' ');
            if (firstSpace != std::string::npos)
            {
                std::string friendsFile = rest.substr(0, firstSpace);
                std::string postsFile = rest.substr(firstSpace + 1);

                // Убираем возможные кавычки
                if (friendsFile.front() == '"' && friendsFile.back() == '"')
                    friendsFile = friendsFile.substr(1, friendsFile.length() - 2);
                if (postsFile.front() == '"' && postsFile.back() == '"')
                    postsFile = postsFile.substr(1, postsFile.length() - 2);

                try
                {
                    sim.LoadFromFiles(friendsFile, postsFile);
                    std::cout << "Files loaded successfully!" << std::endl;
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
            }
            else
            {
                std::cout << "Usage: load <friendsFile> <postsFile>" << std::endl;
            }
        }
        else if (command.find("generate ") == 0)
        {
            try
            {
                int count = std::stoi(command.substr(9));
                // Получаем всех пользователей из сети
                auto &network = sim.GetNetwork();
                auto users = network.GetAllUserIds();

                if (users.empty())
                {
                    std::cout << "No users found! Please add users first or use 'load' command." << std::endl;
                }
                else
                {
                    sim.GenerateRandomActivity(count, users);
                    std::cout << "Generated " << count << " posts for " << users.size() << " users" << std::endl;
                }
            }
            catch (...)
            {
                std::cout << "Usage: generate <count>" << std::endl;
            }
        }
        else if (command.find("feeds ") == 0)
        {
            std::string userId = command.substr(6);
            auto &network = sim.GetNetwork();

            auto user = network.GetUser(userId);
            if (!user)
            {
                std::cout << "User '" << userId << "' not found!" << std::endl;
                continue;
            }

            auto friendsFeed = network.GetFriendsFeed(userId);
            std::cout << "Friends feed for " << userId << " (" << friendsFeed.size() << " posts):" << std::endl;
            for (const auto &post : friendsFeed)
            {
                std::cout << "  " << post->toString() << std::endl;
            }

            auto extendedFeed = network.GetExtendedFeed(userId);
            std::cout << "\nExtended feed for " << userId << " (" << extendedFeed.size() << " posts):" << std::endl;
            for (const auto &post : extendedFeed)
            {
                std::cout << "  " << post->toString() << std::endl;
            }
        }
        else if (command.find("handshakes ") == 0)
        {
            std::string rest = command.substr(11);
            size_t spacePos = rest.find(' ');
            if (spacePos != std::string::npos)
            {
                std::string user1 = rest.substr(0, spacePos);
                std::string user2 = rest.substr(spacePos + 1);
                auto &network = sim.GetNetwork();
                int handshakes = network.CalculateHandshakes(user1, user2);
                if (handshakes >= 0)
                {
                    std::cout << "Handshakes between " << user1 << " and " << user2 << ": " << handshakes << std::endl;
                }
                else
                {
                    std::cout << "No path found between " << user1 << " and " << user2 << std::endl;
                }
            }
            else
            {
                std::cout << "Usage: handshakes <user1> <user2>" << std::endl;
            }
        }
        else if (command.find("add_user ") == 0)
        {
            std::string userId = command.substr(9);
            auto &network = sim.GetNetwork();
            auto user = network.AddUser(userId);
            std::cout << "User '" << userId << "' added successfully!" << std::endl;
        }
        else if (command.find("add_friendship ") == 0)
        {
            std::string rest = command.substr(15);
            size_t spacePos = rest.find(' ');
            if (spacePos != std::string::npos)
            {
                std::string user1 = rest.substr(0, spacePos);
                std::string user2 = rest.substr(spacePos + 1);
                auto &network = sim.GetNetwork();
                network.CreateFriendship(user1, user2);
                std::cout << "Friendship between " << user1 << " and " << user2 << " created!" << std::endl;
            }
            else
            {
                std::cout << "Usage: add_friendship <user1> <user2>" << std::endl;
            }
        }
        else if (command.find("RemoveFriendship ") == 0)
        {
            std::string rest = command.substr(18);
            size_t spacePos = rest.find(' ');
            if (spacePos != std::string::npos)
            {
                std::string user1 = rest.substr(0, spacePos);
                std::string user2 = rest.substr(spacePos + 1);
                auto &network = sim.GetNetwork();

                auto u1 = network.GetUser(user1);
                auto u2 = network.GetUser(user2);
                if (u1 && u2)
                {
                    u1->RemoveFriend(u2);
                    u2->RemoveFriend(u1);
                    std::cout << "Friendship between " << user1 << " and " << user2 << " removed!" << std::endl;
                }
                else
                {
                    std::cout << "One or both users not found!" << std::endl;
                }
            }
            else
            {
                std::cout << "Usage: RemoveFriendship <user1> <user2>" << std::endl;
            }
        }
        else if (command.find("friends ") == 0)
        {
            std::string userId = command.substr(8);
            auto &network = sim.GetNetwork();
            auto user = network.GetUser(userId);
            if (user)
            {
                auto friends = user->GetFriends();
                std::cout << "Friends of " << userId << " (" << friends.size() << "): ";
                for (const auto &friendUser : friends)
                {
                    std::cout << friendUser->GetId(); // fixme чтобы последняя запятая не печаталась
                    std::cout << ", ";
                }
                std::cout << std::endl;
            }
            else
            {
                std::cout << "User '" << userId << "' not found!" << std::endl;
            }
        }
        else if (command.find("add_post ") == 0)
        {
            std::string rest = command.substr(9);
            size_t spacePos = rest.find(' ');
            if (spacePos != std::string::npos)
            {
                std::string userId = rest.substr(0, spacePos);
                std::string content = rest.substr(spacePos + 1);
                auto &network = sim.GetNetwork();

                auto now = std::chrono::system_clock::now();
                network.CreatePost(now, userId, content);
                std::cout << "Post added for user '" << userId << "'" << std::endl;
            }
            else
            {
                std::cout << "Usage: add_post <userId> <content>" << std::endl;
            }
        }
        else
        {
            std::cout << "Unknown command. Type 'exit' to quit." << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        PrintUsage();
        return 1;
    }

    std::string mode = argv[1];

    try
    {
        if (mode == "demo")
        {
            RunDemo();
        }
        else if (mode == "load" && argc == 4)
        {
            RunFileLoading(argv[2], argv[3]);
        }
        else if (mode == "generate" && argc == 3)
        {
            RunGeneration(std::stoi(argv[2]));
        }
        else if (mode == "interactive")
        {
            InteractiveMode();
        }
        else
        {
            PrintUsage();
            return 1;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}