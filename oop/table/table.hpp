#pragma once
#include <vector>
#include <string>
#include <istream>
#include <iostream>
#include <fstream>

// Парсит и возвращает структуру с данными
class TableParser
{
public:
    struct ParsedData
    {
        std::vector<char> alignments;
        std::vector<std::vector<std::string>> rows;
    };
    // ФУнкция для парсинга
    ParsedData parse(std::istream &input);

private:
    bool parseAlignmentLine(std::istream &input, std::vector<char> &alignments);
    void parseDataLines(std::istream &input, int num_columns,
                        std::vector<std::vector<std::string>> &rows);
    std::vector<std::string> splitLine(const std::string &line, int num_columns);
};

// Класс для форматирования таблицы
class TableFormatter
{
public:
    std::string format(const std::vector<char> &alignments,
                       const std::vector<std::vector<std::string>> &rows);

private:
    std::vector<int> calculateColumnWidths(const std::vector<std::vector<std::string>> &rows);
    std::string createHorizontalLine(const std::vector<int> &widths);
    std::string formatRow(const std::vector<std::string> &row,
                          const std::vector<char> &alignments,
                          const std::vector<int> &widths);
    std::string alignCell(const std::string &content, char alignment, int width);
};

// Основной класс, управляющий работой с таблицей
class TableManager
{
public:
    // Основная функция обработки аблицы
    void process(std::istream &input, std::ostream &output);

private:
    std::vector<char> alignments;
    std::vector<std::vector<std::string>> rows;
};