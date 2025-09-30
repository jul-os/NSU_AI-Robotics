#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>

using namespace std;

enum class UserRole
{
    RegularUser,
    Girl,
    CowardGirl,
    Boy,
    RichBoy
};

UserRole StringToRole(const string &role_str)
{
    if (role_str == "User")
        return UserRole::RegularUser;
    if (role_str == "Girl")
        return UserRole::Girl;
    if (role_str == "CowardGirl")
        return UserRole::CowardGirl;
    if (role_str == "Boy")
        return UserRole::Boy;
    if (role_str == "RichBoy")
        return UserRole::RichBoy;
    throw invalid_argument("Unknown role: " + role_str);
}

class User
{
public:
    User(string name)
    {
        _name = name;
    }
    virtual void Speak(ostream &out, vector<string> &chat_history) = 0;

protected:
    ostream &SendMessage(ostream &out, string message, vector<string> &chat_history)
    {
        string full_message = _name + ": " + message;
        out << full_message << "\n";
        chat_history.push_back(full_message);
        return out;
        // таким образом можно будет менять куда именно будем делать вывод: в терминал или файл
        // тк функция просто ждет поток вывода абстрактный
    }
    bool ContainsWord(const vector<string> &history, const string &word) const
    {
        for (const auto &msg : history)
        {
            if (msg.find(word) != string::npos)
            {
                return true;
            }
        }
        return false;
    }

private:
    string _name;
    UserRole _role;
};

class RegularUser : public User
{
public:
    // дочерние классы почему-то автоматически не наследуют конструктор
    RegularUser(string name) : User(name) {}
    void Speak(ostream &out, vector<string> &chat_history) override
    {
        SendMessage(out, "Hey", chat_history);
    }
};

class Girl : public User
{
    //- I want a dress. Will somebody buy it for me?
public:
    Girl(string name) : User(name) {}
    void Speak(ostream &out, vector<string> &chat_history) override
    {
        SendMessage(out, "Hey", chat_history);
        SendMessage(out, "I want a dress. Will somebody buy it for me?", chat_history);
    }
};

class CowardGirl : public Girl
{
    // если в чате есть слово rat или rats, говорит - AAAAaaa! No! No rats here, pls
public:
    CowardGirl(string name) : Girl(name) {}
    void Speak(ostream &out, vector<string> &chat_history) override
    {
        // Сначала вызываем поведение Girl (Hey + про платье)
        Girl::Speak(out, chat_history);

        // Затем добавляем специфическое поведение CowardGirl
        if (ContainsWord(chat_history, "rat") || ContainsWord(chat_history, "rats"))
        {
            SendMessage(out, "AAAAaaa! No! No rats here, pls", chat_history);
        }
    }
};

class Boy : public User
{
    // Если в общем чате есть высказывание I want a dress, говорит U r such a girl! R u afraid of rats?
public:
    Boy(string name) : User(name) {}
    void Speak(ostream &out, vector<string> &chat_history) override
    {
        SendMessage(out, "Hey", chat_history);

        if (ContainsWord(chat_history, "I want a dress"))
        {
            SendMessage(out, "U r such a girl! R u afraid of rats?", chat_history);
        }
    }
};

class RichBoy : public User
{
    /// независимо ни от чего говорит - I have a supercar
    /* Если в общем чате есть высказывание I want a dress. Will somebody buy it for
    me?, говорит Of course, baby. I'm the richest boy in the world !*/
public:
    RichBoy(string name) : User(name) {}
    void Speak(ostream &out, vector<string> &chat_history) override
    {
        SendMessage(out, "Hey", chat_history);
        SendMessage(out, "I have a supercar", chat_history);

        if (ContainsWord(chat_history, "I want a dress"))
        {
            SendMessage(out, "Of course, baby. I'm the richest boy in the world!", chat_history);
        }
    }
};

int main()
{
    ifstream in("input.txt");
    vector<string> chat_history;
    vector<unique_ptr<User>> users;
    string name, role_str;
    while (in >> name >> role_str)
    {
        unique_ptr<User> user;
        UserRole role = StringToRole(role_str);
        switch (role)
        {
        case UserRole::RegularUser:
            user = make_unique<RegularUser>(name);
            break;
        case UserRole::Girl:
            user = make_unique<Girl>(name);
            break;
        case UserRole::CowardGirl:
            user = make_unique<CowardGirl>(name);
            break;
        case UserRole::Boy:
            user = make_unique<Boy>(name);
            break;
        case UserRole::RichBoy:
            user = make_unique<RichBoy>(name);
            break;
        }
        if (user)
        {
            users.push_back(move(user));
        }
    }
    // Симулируем чат
    for (auto &user : users)
    {
        user->Speak(cout, chat_history);
    }
    return 0;
}
