// NotificationChannel.h
#ifndef NOTIFICATION_CHANNEL_H
#define NOTIFICATION_CHANNEL_H

#include <string>
#include <iostream>

enum class Priority
{
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3
};

class NotificationChannel
{
public:
    virtual ~NotificationChannel() = default;
    virtual void send(const std::string &message) = 0;
    virtual std::string getName() const = 0;
};

class SMSChannel : public NotificationChannel
{
public:
    void send(const std::string &message) override
    {
        std::cout << "[SMS] " << message << std::endl;
    }
    std::string getName() const override { return "SMS"; }
};

class EmailChannel : public NotificationChannel
{
public:
    void send(const std::string &message) override
    {
        std::cout << "[Email] " << message << std::endl;
    }
    std::string getName() const override { return "Email"; }
};

class SoundChannel : public NotificationChannel
{
public:
    void send(const std::string &message) override
    {
        std::cout << "[SOUND] Beep! " << message << std::endl;
    }
    std::string getName() const override { return "Sound"; }
};

#endif // NOTIFICATION_CHANNEL_H