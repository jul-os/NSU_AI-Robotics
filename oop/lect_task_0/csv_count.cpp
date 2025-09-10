#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <vector>
// можно делать не в ооп стиле пока что
using namespace std;

int main(int argc, char **argv)
{
    ofstream out;
    ifstream in("input.txt");
    if (!in.is_open())
    {
        perror("input file couldn't be opened\n");
        return 0;
    }
    out.open("output.csv");
    if (!out.is_open())
    {
        perror("csv file couldn't be opened\n");
        return 0;
    }
    string iread;
    int totalWords = 0;
    map<const string, int> wordsCount;
    while (getline(in, iread))
    {
        stringstream ss(iread); // создать потоковый объект из строки
        string word;
        while (ss >> word)
        {
            // удалить все знаки препинания
            word.erase(std::remove_if(word.begin(), word.end(), [](unsigned char c)
                                      { return std::ispunct(c); }),
                       word.end());
            // вставка в map
            if (wordsCount.find(word) == wordsCount.end())
            {
                wordsCount.insert({word, 1});
            }
            else
            {
                wordsCount[word] += 1;
            }
            totalWords++;
        }
    }
    // в гугле сказали что просто так отсортировать мапу по значению нельзя надо привлечь вектор
    vector<pair<string, int>> sortedMap(wordsCount.begin(), wordsCount.end());
    sort(sortedMap.begin(), sortedMap.end(), [](const auto &a, const auto &b)
         { return a.second > b.second; });
    // тут запись в csv
    out << "word, count, percentage\n";
    for (auto word : sortedMap)
    {
        out << word.first << "," << word.second << "," << word.second / totalWords << "\n";
    }
    // очищаем мапу, закрываем файлы
    wordsCount.clear();
    out.close();
    in.close();
    return 0;
}