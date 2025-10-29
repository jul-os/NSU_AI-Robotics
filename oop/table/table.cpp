#include "table.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

TableParser::ParsedData TableParser::parse(istream &input)
{
    ParsedData result;
    string line;

    if (!parseAlignmentLine(input, result.alignments))
    {
        return result;
    }

    parseDataLines(input, result.alignments.size(), result.rows);
    return result;
}

bool TableParser::parseAlignmentLine(istream &input, vector<char> &alignments)
{
    string line;
    if (!getline(input, line))
        return false;

    for (char ch : line)
    {
        if (ch == '<' || ch == '=' || ch == '>')
        {
            alignments.push_back(ch);
        }
    }
    return !alignments.empty();
}

void TableParser::parseDataLines(istream &input, int num_columns,
                                 vector<vector<string>> &rows)
{
    string line;
    while (getline(input, line))
    {
        if (line.empty())
            continue; // пропускаем пустые

        auto row = splitLine(line, num_columns);
        rows.push_back(row);
    }

    /*
        string line;
        while (getline(input, line))
        {
            if (line.empty())
                continue;

            cout << "DEBUG Line: '" << line << "'" << endl; // ← добавить эту строку

            auto row = splitLine(line, num_columns);

            // Отладочная печать результата разбиения
            cout << "DEBUG Parsed: ";
            for (const auto &cell : row)
            {
                cout << "['" << cell << "'] ";
            }
            cout << endl;

            rows.push_back(row);
        }
        */
}

vector<string> TableParser::splitLine(const string &line, int num_columns)
{
    vector<string> row;
    istringstream iss(line);
    string cell;

    while (iss >> cell)
    {
        row.push_back(cell);
    }

    // Корректируем размер
    if (row.size() < num_columns)
    {
        row.resize(num_columns, ""); //"" заполняет недостающие ячейки пустыми строками
    }
    else if (row.size() > num_columns)
    {
        row.resize(num_columns); // отбрасывает лишние ячейки
    }

    return row;
}

string TableFormatter::format(const vector<char> &alignments,
                              const vector<vector<string>> &rows)
{
    if (alignments.empty() || rows.empty())
    {
        return "";
    }

    vector<int> widths = calculateColumnWidths(rows);
    stringstream result;

    // Верхняя граница
    result << createHorizontalLine(widths) << "\n";

    // Строки данных
    for (const auto &row : rows)
    {
        result << formatRow(row, alignments, widths) << "\n";
        result << createHorizontalLine(widths) << "\n";
    }

    return result.str();
}

vector<int> TableFormatter::calculateColumnWidths(const vector<vector<string>> &rows)
{
    if (rows.empty())
        return {};

    // Находим максимальное количество столбцов во всех строках
    size_t max_columns = 0;
    for (const auto &row : rows)
    {
        max_columns = max(max_columns, row.size());
    }

    vector<int> widths(max_columns, 0);

    for (const auto &row : rows)
    {
        for (size_t i = 0; i < row.size(); ++i)
        {
            widths[i] = max(widths[i], (int)row[i].length());
        }
    }

    // Добавляем отступы (по 1 пробелу с каждой стороны)
    for (auto &width : widths)
    {
        width += 2;
    }

    return widths;
}

string TableFormatter::createHorizontalLine(const vector<int> &widths)
{
    stringstream line;
    line << "+";
    for (int width : widths)
    {
        line << string(width, '-') << "+";
    }
    return line.str();
}

string TableFormatter::formatRow(const vector<string> &row,
                                 const vector<char> &alignments,
                                 const vector<int> &widths)
{
    stringstream formattedRow;
    formattedRow << "|";

    for (size_t i = 0; i < widths.size(); ++i)
    {
        string cell_content = (i < row.size()) ? row[i] : "";
        char alignment = (i < alignments.size()) ? alignments[i] : '<';
        formattedRow << alignCell(cell_content, alignment, widths[i]) << "|";
    }

    return formattedRow.str();
}

string TableFormatter::alignCell(const string &content, char alignment, int width)
{
    int contentLength = content.length();
    int totalSpaces = width - contentLength;

    if (totalSpaces <= 0)
    {
        return " " + content + " "; // Минимальные отступы
    }

    switch (alignment)
    {
    case '<':
    { // Выравнивание по левому краю
        int rightSpaces = totalSpaces - 1;
        return " " + content + string(rightSpaces, ' ');
    }
    case '=':
    { // Выравнивание по центру
        int leftSpaces = totalSpaces / 2;
        int rightSpaces = totalSpaces - leftSpaces;
        return string(leftSpaces, ' ') + content + string(rightSpaces, ' ');
    }
    case '>':
    { // Выравнивание по правому краю
        int leftSpaces = totalSpaces - 1;
        return string(leftSpaces, ' ') + content + " ";
    }
    default:
    { // По умолчанию - как левое
        int rightSpaces = totalSpaces - 1;
        return " " + content + string(rightSpaces, ' ');
    }
    }
}

void TableManager::process(istream &input, ostream &output)
{
    TableParser parser;
    TableFormatter formatter;

    // Парсим в наши внутренние поля
    auto parsed = parser.parse(input);
    alignments = parsed.alignments;
    rows = parsed.rows;

    // Форматируем
    output << formatter.format(alignments, rows);
}