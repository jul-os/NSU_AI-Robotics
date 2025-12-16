// NotificationManager.h
#ifndef NOTIFICATION_MANAGER_H
#define NOTIFICATION_MANAGER_H

#include "NotificationChannel.h"
#include <vector>
#include <memory>
#include <algorithm>

struct Subscriber
{
    std::shared_ptr<NotificationChannel> channel;
    Priority minPriority;
};

class NotificationManager
{
public:
    void subscribe(std::shared_ptr<NotificationChannel> channel, Priority minPriority)
    {
        subscribers.push_back({channel, minPriority});
    }

    void unsubscribe(const std::string &channelName)
    {
        subscribers.erase(
            std::remove_if(subscribers.begin(), subscribers.end(),
                           [&](const Subscriber &s)
                           {
                               return s.channel->getName() == channelName;
                           }),
            subscribers.end());
    }

    void notify(const std::string &message, Priority priority)
    {
        for (const auto &sub : subscribers)
        {
            if (static_cast<int>(priority) >= static_cast<int>(sub.minPriority))
            {
                sub.channel->send(message);
            }
        }
    }

private:
    std::vector<Subscriber> subscribers;
};

#endif // NOTIFICATION_MANAGER_H
