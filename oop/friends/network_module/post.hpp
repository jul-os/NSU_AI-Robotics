#pragma once
#include <string>
#include <chrono>
#include <memory>
#include <sstream>
#include "datetime.hpp"

class User;

class Post
{
public:
    Post(const std::chrono::system_clock::time_point &time,
         std::shared_ptr<User> authorPtr,
         const std::string &content)
        : timestamp(time), author(authorPtr), text(content) {}

    ~Post() = default;

    auto getTimestamp() const { return timestamp; }
    std::shared_ptr<User> getAuthor() const { return author; }
    std::string getText() const { return text; }

    std::string toString() const
    {
        std::stringstream ss;
        ss << "[" << timeToString(timestamp) << "] "
           << (author ? author->GetId() : "unknown")
           << ": " << text;
        return ss.str();
    }

private:
    std::chrono::system_clock::time_point timestamp;
    std::shared_ptr<User> author;
    std::string text;
};