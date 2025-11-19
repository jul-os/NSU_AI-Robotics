#include "social_network.hpp"
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <sstream>
#include <fstream>
#include <iostream>
#include <set>

std::shared_ptr<User> SocialNetwork::AddUser(const std::string &userId)
{
    auto user = std::make_shared<User>(userId);
    users[userId] = user;
    return user;
}

std::shared_ptr<User> SocialNetwork::GetUser(const std::string &userId)
{
    auto it = users.find(userId);
    if (it != users.end())
    {
        return it->second;
    }
    return nullptr;
}

void SocialNetwork::CreateFriendship(const std::string &user1Id, const std::string &user2Id)
{
    auto user1 = GetUser(user1Id);
    auto user2 = GetUser(user2Id);

    if (user1 && user2)
    {
        user1->AddFriend(user2);
        user2->AddFriend(user1);
    }
}

void SocialNetwork::RemoveFriendship(const std::string &user1Id, const std::string &user2Id)
{
    auto user1 = GetUser(user1Id);
    auto user2 = GetUser(user2Id);
    if (user1 && user2)
    {
        user1->RemoveFriend(user2);
        user2->RemoveFriend(user1);
    }
}

std::shared_ptr<Post> SocialNetwork::CreatePost(const std::chrono::system_clock::time_point &time,
                                                const std::string &authorId, const std::string &content)
{
    auto author = GetUser(authorId);
    if (!author)
    {
        author = AddUser(authorId);
    }

    auto post = std::make_shared<Post>(time, author, content);
    author->AddPost(post);

    return post;
}

void SocialNetwork::CollectPostsFromUsers(const std::unordered_set<std::shared_ptr<User>> &usersCollection,
                                          std::vector<std::shared_ptr<Post>> &result) const
{
    for (const auto &user : usersCollection)
    {
        if (user)
        {
            auto user_posts = user->GetPosts();
            result.insert(result.end(), user_posts.begin(), user_posts.end());
        }
    }
}

void SocialNetwork::RemoveDuplicatesAndSort(std::vector<std::shared_ptr<Post>> &posts) const
{
    if (posts.empty())
        return;

    std::unordered_set<std::shared_ptr<Post>> uniquePosts(posts.begin(), posts.end()); // ask
    posts.assign(uniquePosts.begin(), uniquePosts.end());

    // Сортируем по времени (самые свежие первыми)
    std::sort(posts.begin(), posts.end(),
              [](const std::shared_ptr<Post> &a, const std::shared_ptr<Post> &b)
              {
                  return a->getTimestamp() > b->getTimestamp();
              });
}

std::vector<std::shared_ptr<Post>> SocialNetwork::GetFriendsFeed(const std::string &userId)
{
    auto user = GetUser(userId);
    if (!user)
        return {};

    std::vector<std::shared_ptr<Post>> feed;
    auto friends = user->GetFriends();

    CollectPostsFromUsers(friends, feed);
    RemoveDuplicatesAndSort(feed);

    return feed;
}

std::vector<std::shared_ptr<Post>> SocialNetwork::GetExtendedFeed(const std::string &userId)
{
    auto user = GetUser(userId);
    if (!user)
        return {};

    std::vector<std::shared_ptr<Post>> feed;

    auto directFriends = user->GetFriends();
    CollectPostsFromUsers(directFriends, feed);

    auto friendsOfFriends = user->GetFriendsOfFriends();
    CollectPostsFromUsers(friendsOfFriends, feed);

    RemoveDuplicatesAndSort(feed);

    return feed;
}

int SocialNetwork::CalculateHandshakes(const std::string &user1Id, const std::string &user2Id)
{
    auto user1 = GetUser(user1Id);
    auto user2 = GetUser(user2Id);

    if (!user1 || !user2)
        return -1;
    if (user1 == user2)
        return 0;

    // BFS для поиска кратчайшего пути
    std::queue<std::shared_ptr<User>> queue;
    std::unordered_map<std::string, int> distances;
    std::unordered_set<std::string> visited;

    queue.push(user1);
    distances[user1->GetId()] = 0;
    visited.insert(user1->GetId());

    while (!queue.empty())
    {
        auto current = queue.front();
        queue.pop();

        if (current == user2)
        {
            return distances[current->GetId()];
        }

        for (const auto &friend_user : current->GetFriends())
        {
            if (friend_user && visited.find(friend_user->GetId()) == visited.end())
            {
                visited.insert(friend_user->GetId());
                distances[friend_user->GetId()] = distances[current->GetId()] + 1;
                queue.push(friend_user);
            }
        }
    }

    return -1; // путь не найден
}
