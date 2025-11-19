#pragma once
#include <string>
#include <memory>
#include "social_network.hpp"
#include "post_generator.hpp"

class Simulation
{
public:
    Simulation();
    ~Simulation() = default;

    void LoadFromFiles(const std::string &friendsFile, const std::string &postsFile);
    void GenerateRandomActivity(int numPosts);
    void GenerateRandomActivity(int numPosts, const std::vector<std::string> &users);
    void RunDemo();

    SocialNetwork &GetNetwork() { return network; }

private:
    SocialNetwork network;
    PostGenerator postGenerator;

    std::chrono::system_clock::time_point ParseTime(const std::string &timeStr);
    void ProcessFriendsLine(const std::string &line);
    void ProcessPostsLine(const std::string &line);

    // Вспомогательные методы для парсинга
    std::vector<std::string> SplitString(const std::string &str, char delimiter);
};