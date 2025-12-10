#include "serialize.h"
#include <sstream>
#include <cassert>
#include <iostream>

void test_pod()
{
    std::cout << "Testing POD types...\n";
    {
        int x = 42;
        std::stringstream ss;
        serialize(x, ss);
        int y = 0;
        deserialize(y, ss);
        assert(x == y);
    }
    {
        double d = 3.14159;
        std::stringstream ss;
        serialize(d, ss);
        double d2 = 0.0;
        deserialize(d2, ss);
        assert(d == d2);
    }
    std::cout << "OK\n\n";
}

void test_string()
{
    std::cout << "Testing std::string...\n";
    std::string s1 = "Hello, C++20 Concepts!";
    std::stringstream ss;
    serialize(s1, ss);
    std::string s2;
    deserialize(s2, ss);
    assert(s1 == s2);
    std::cout << "OK\n\n";
}

void test_vector_int()
{
    std::cout << "Testing std::vector<int>...\n";
    std::vector<int> v1 = {1, 2, 3, 42, -100};
    std::stringstream ss;
    serialize(v1, ss);
    std::vector<int> v2;
    deserialize(v2, ss);
    assert(v1 == v2);
    std::cout << "OK\n\n";
}

void test_vector_string()
{
    std::cout << "Testing std::vector<std::string>...\n";
    std::vector<std::string> v1 = {"apple", "banana", "cherry"};
    std::stringstream ss;
    serialize(v1, ss);
    std::vector<std::string> v2;
    deserialize(v2, ss);
    assert(v1 == v2);
    std::cout << "OK\n\n";
}

void test_map()
{
    std::cout << "Testing std::map<std::string, int>...\n";
    std::map<std::string, int> m1 = {
        {"one", 1},
        {"two", 2},
        {"forty_two", 42}};
    std::stringstream ss;
    serialize(m1, ss);
    std::map<std::string, int> m2;
    deserialize(m2, ss);
    assert(m1 == m2);
    std::cout << "OK\n\n";
}

// Попытка скомпилировать несериализуемый тип — должна дать ошибку!
// struct NonSerializable { std::string* ptr; };
// void test_bad() {
//     NonSerializable x;
//     std::stringstream ss;
//     serialize(x, ss); // ← должно быть ОШИБКОЙ КОМПИЛЯЦИИ
// }

int main()
{
    test_pod();
    test_string();
    test_vector_int();
    test_vector_string();
    test_map();

    std::cout << "🎉 All tests passed!\n";
    return 0;
}