#include "user.hpp"
#include "post.hpp"
#include <algorithm>
#include <queue>
#include <unordered_set>

User::User(const std::string &userId) : id(userId) {}

void User::AddFriend(std::shared_ptr<User> friendUser)
{
    if (friendUser.get() != this && friendUser)
    {
        friends.insert(friendUser);
    }
}

void User::AddPost(std::shared_ptr<Post> post)
{
    if (post)
    {
        posts.push_back(post);
    }
}

void User::RemoveFriend(std::shared_ptr<User> friendUser)
{
    if (friendUser)
    {
        friends.erase(friendUser);
    }
}

std::unordered_set<std::shared_ptr<User>> User::GetFriends() const
{
    return friends;
}

std::unordered_set<std::shared_ptr<User>> User::GetFriendsOfFriends() const
{
    std::unordered_set<std::shared_ptr<User>> friendsOfFriends;
    for (const auto &friendUser : friends)
    {
        if (friendUser)
        {
            for (const auto &friendOfFriend : friendUser->friends)
            {
                if (friendOfFriend.get() != this &&
                    friends.find(friendOfFriend) == friends.end())
                {
                    friendsOfFriends.insert(friendOfFriend);
                }
            }
        }
    }
    return friendsOfFriends;
}