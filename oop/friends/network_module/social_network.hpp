#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include <set>
#include "user.hpp"
#include "post.hpp"

class SocialNetwork
{
public:
    SocialNetwork() = default;
    ~SocialNetwork() = default;

    std::shared_ptr<User> AddUser(const std::string &userId);
    std::shared_ptr<User> GetUser(const std::string &userId);
    void CreateFriendship(const std::string &user1Id, const std::string &user2Id);
    void RemoveFriendship(const std::string &user1Id, const std::string &user2Id);
    std::shared_ptr<Post> CreatePost(const std::chrono::system_clock::time_point &time,
                                     const std::string &authorId, const std::string &content);

    std::vector<std::shared_ptr<Post>> GetFriendsFeed(const std::string &userId);
    std::vector<std::shared_ptr<Post>> GetExtendedFeed(const std::string &userId);
    int CalculateHandshakes(const std::string &user1Id, const std::string &user2Id);

    // Метод для получения всех пользователей
    std::vector<std::shared_ptr<User>> GetAllUsers() const
    {
        std::vector<std::shared_ptr<User>> allUsers;
        for (const auto &pair : users)
        {
            allUsers.push_back(pair.second);
        }
        return allUsers;
    }

    // Метод для получения списка ID пользователей
    std::vector<std::string> GetAllUserIds() const
    {
        std::vector<std::string> userIds;
        for (const auto &pair : users)
        {
            userIds.push_back(pair.first);
        }
        return userIds;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<User>> users;
    // Вспомогательный метод для сбора постов от группы пользователей
    void CollectPostsFromUsers(const std::unordered_set<std::shared_ptr<User>> &usersCollection,
                               std::vector<std::shared_ptr<Post>> &result) const;

    // Вспомогательный метод для удаления дубликатов и сортировки
    void RemoveDuplicatesAndSort(std::vector<std::shared_ptr<Post>> &posts) const;
};