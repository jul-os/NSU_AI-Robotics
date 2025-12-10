#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include "serialize.h"

using namespace std;

int main()
{
    // === 1. Старые тесты (POD) ===
    {
        ofstream ofs("test.ser", ofstream::out | ofstream::binary);
        char hello[7] = "hello!";
        int boo[3] = {1, 2, 3};

        serialize(hello, ofs);
        serialize(boo, ofs);
        ofs.close();

        char hello2[7];
        int boo2[3];

        ifstream ifs("test.ser", ifstream::in | ifstream::binary);
        ifs >> noskipws;

        deserialize(hello2, ifs);
        deserialize(boo2, ifs);
        ifs.close();

        if (equal(hello, hello + 7, hello2))
            cout << "POD char array OK!" << endl;
        else
            cout << "POD char array FAILED!" << endl;

        if (equal(boo, boo + 3, boo2))
            cout << "POD int array OK!" << endl;
        else
            cout << "POD int array FAILED!" << endl;
    }

    // === 2. Тест std::string ===
    {
        ofstream ofs("test.ser", ofstream::out | ofstream::binary);
        string s1 = "Привет, мир! 🌍";
        serialize(s1, ofs);
        ofs.close();

        string s2;
        ifstream ifs("test.ser", ifstream::in | ifstream::binary);
        ifs >> noskipws;
        deserialize(s2, ifs);
        ifs.close();

        if (s1 == s2)
            cout << "std::string OK!" << endl;
        else
            cout << "std::string FAILED!" << endl;
    }

    // === 3. Тест std::vector<int> ===
    {
        ofstream ofs("test.ser", ofstream::out | ofstream::binary);
        vector<int> v1 = {10, -5, 0, 1000000};
        serialize(v1, ofs);
        ofs.close();

        vector<int> v2;
        ifstream ifs("test.ser", ifstream::in | ifstream::binary);
        ifs >> noskipws;
        deserialize(v2, ifs);
        ifs.close();

        if (v1 == v2)
            cout << "std::vector<int> OK!" << endl;
        else
            cout << "std::vector<int> FAILED!" << endl;
    }

    // === 4. Тест std::vector<std::string> (вложенность) ===
    {
        ofstream ofs("test.ser", ofstream::out | ofstream::binary);
        vector<string> vs1 = {"one", "two", "three"};
        serialize(vs1, ofs);
        ofs.close();

        vector<string> vs2;
        ifstream ifs("test.ser", ifstream::in | ifstream::binary);
        ifs >> noskipws;
        deserialize(vs2, ifs);
        ifs.close();

        if (vs1 == vs2)
            cout << "std::vector<std::string> OK!" << endl;
        else
            cout << "std::vector<std::string> FAILED!" << endl;
    }

    // === 5. Тест std::map<string, int> ===
    {
        ofstream ofs("test.ser", ofstream::out | ofstream::binary);
        map<string, int> m1 = {{"apple", 5}, {"banana", 3}, {"cherry", 10}};
        serialize(m1, ofs);
        ofs.close();

        map<string, int> m2;
        ifstream ifs("test.ser", ifstream::in | ifstream::binary);
        ifs >> noskipws;
        deserialize(m2, ifs);
        ifs.close();

        if (m1 == m2)
            cout << "std::map<string, int> OK!" << endl;
        else
            cout << "std::map<string, int> FAILED!" << endl;
    }

    // === 6. Тест пустых контейнеров ===
    {
        ofstream ofs("test.ser", ofstream::out | ofstream::binary);
        string empty_str;
        vector<double> empty_vec;
        map<int, string> empty_map;

        serialize(empty_str, ofs);
        serialize(empty_vec, ofs);
        serialize(empty_map, ofs);
        ofs.close();

        string empty_str2;
        vector<double> empty_vec2;
        map<int, string> empty_map2;

        ifstream ifs("test.ser", ifstream::in | ifstream::binary);
        ifs >> noskipws;
        deserialize(empty_str2, ifs);
        deserialize(empty_vec2, ifs);
        deserialize(empty_map2, ifs);
        ifs.close();

        bool ok = (empty_str2.empty() && empty_vec2.empty() && empty_map2.empty());
        if (ok)
            cout << "Empty containers OK!" << endl;
        else
            cout << "Empty containers FAILED!" << endl;
    }

    return 0;
}