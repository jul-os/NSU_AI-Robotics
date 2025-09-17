#include <iostream>
#include <fstream>

using namespace std;

class User
{
public:
    string name;

    void send_message()
    {
        ///
    }
    // todo everyone sends hey
} User;

class Girl : User
{
    //- I want a dress. Will somebody buy it for me?
};

class CowardGirl : Girl
{
    // если в чате есть слово rat или rats, говорит - AAAAaaa! No! No rats here, pls
};

class Boy : User
{
    // Если в общем чате есть высказывание I want a dress, говорит U r such a girl! R u afraid of rats?
};

class RichBoy : Boy
{
    /// независимо ни от чего говорит - I have a supercar
    /* Если в общем чате есть высказывание I want a dress. Will somebody buy it for
    me?, говорит Of course, baby. I'm the richest boy in the world !*/
};

main()
{
    // todo поток выода можно выбрать
    ifstream in("input.txt");
}
