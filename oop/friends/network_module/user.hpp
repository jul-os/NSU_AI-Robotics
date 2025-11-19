#pragma once
#include <string>
#include <unordered_set>
#include <vector>
#include <memory>

class Post;

class User : public std::enable_shared_from_this<User>
{
public:
    User(const std::string &userId);
    ~User() = default;

    void AddFriend(std::shared_ptr<User> friendUser);
    void AddPost(std::shared_ptr<Post> post);
    void RemoveFriend(std::shared_ptr<User> friendUser);

    std::unordered_set<std::shared_ptr<User>> GetFriends() const;
    std::unordered_set<std::shared_ptr<User>> GetFriendsOfFriends() const;
    std::string GetId() const { return id; }
    std::vector<std::shared_ptr<Post>> GetPosts() const { return posts; }

private:
    std::string id;
    std::unordered_set<std::shared_ptr<User>> friends;
    std::vector<std::shared_ptr<Post>> posts;
};