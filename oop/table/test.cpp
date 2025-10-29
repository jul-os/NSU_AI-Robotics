#include "table.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

using namespace std;

// Тесты для TableParser
class TableParserTest : public ::testing::Test
{
protected:
    TableParser parser;
};

TEST_F(TableParserTest, Parse_ValidTable)
{
    istringstream input("< = >\ndata1 data2 data3\ninfo1 info2 info3\n");
    auto result = parser.parse(input);

    ASSERT_EQ(3, result.alignments.size());
    EXPECT_EQ('<', result.alignments[0]);
    EXPECT_EQ('=', result.alignments[1]);
    EXPECT_EQ('>', result.alignments[2]);

    ASSERT_EQ(2, result.rows.size());
    ASSERT_EQ(3, result.rows[0].size());
    EXPECT_EQ("data1", result.rows[0][0]);
    EXPECT_EQ("data2", result.rows[0][1]);
    EXPECT_EQ("data3", result.rows[0][2]);
    EXPECT_EQ("info1", result.rows[1][0]);
    EXPECT_EQ("info2", result.rows[1][1]);
    EXPECT_EQ("info3", result.rows[1][2]);
}

TEST_F(TableParserTest, Parse_EmptyInput)
{
    istringstream input("");
    auto result = parser.parse(input);

    EXPECT_TRUE(result.alignments.empty());
    EXPECT_TRUE(result.rows.empty());
}

TEST_F(TableParserTest, Parse_OnlyAlignmentLine)
{
    istringstream input("< > =\n");
    auto result = parser.parse(input);

    ASSERT_EQ(3, result.alignments.size());
    EXPECT_EQ('<', result.alignments[0]);
    EXPECT_EQ('>', result.alignments[1]);
    EXPECT_EQ('=', result.alignments[2]);
    EXPECT_TRUE(result.rows.empty());
}

TEST_F(TableParserTest, Parse_InvalidAlignmentLine)
{
    istringstream input("abc xyz\n");
    auto result = parser.parse(input);

    EXPECT_TRUE(result.alignments.empty());
    EXPECT_TRUE(result.rows.empty());
}

TEST_F(TableParserTest, Parse_MixedAlignmentCharacters)
{
    istringstream input("< a = b > c\ndata1 data2 data3\n");
    auto result = parser.parse(input);

    ASSERT_EQ(3, result.alignments.size());
    EXPECT_EQ('<', result.alignments[0]);
    EXPECT_EQ('=', result.alignments[1]);
    EXPECT_EQ('>', result.alignments[2]);
}

TEST_F(TableParserTest, Parse_WithEmptyLines)
{
    istringstream input("<\nrow1_c1 row1_c2\n\nrow2_c1 row2_c2\n\n");
    auto result = parser.parse(input);

    ASSERT_EQ(1, result.alignments.size());
    ASSERT_EQ(2, result.rows.size());
    EXPECT_EQ("row1_c1", result.rows[0][0]);
    EXPECT_EQ("row2_c1", result.rows[1][0]);
}

TEST_F(TableParserTest, Parse_ColumnAdjustmentMoreColumns)
{
    istringstream input("< >\none two three four\nsingle\n");
    auto result = parser.parse(input);

    ASSERT_EQ(2, result.alignments.size());
    ASSERT_EQ(2, result.rows.size());

    // Первая строка должна быть обрезана до 2 столбцов
    ASSERT_EQ(2, result.rows[0].size());
    EXPECT_EQ("one", result.rows[0][0]);
    EXPECT_EQ("two", result.rows[0][1]);

    // Вторая строка должна быть дополнена пустыми строками
    ASSERT_EQ(2, result.rows[1].size());
    EXPECT_EQ("single", result.rows[1][0]);
    EXPECT_EQ("", result.rows[1][1]);
}

TEST_F(TableParserTest, Parse_ColumnAdjustmentLessColumns)
{
    istringstream input("< = >\nshort\n");
    auto result = parser.parse(input);

    ASSERT_EQ(3, result.alignments.size());
    ASSERT_EQ(1, result.rows.size());
    ASSERT_EQ(3, result.rows[0].size());
    EXPECT_EQ("short", result.rows[0][0]);
    EXPECT_EQ("", result.rows[0][1]);
    EXPECT_EQ("", result.rows[0][2]);
}

// Тесты для TableFormatter
class TableFormatterTest : public ::testing::Test
{
protected:
    TableFormatter formatter;
};

TEST_F(TableFormatterTest, Format_EmptyInput)
{
    vector<char> alignments;
    vector<vector<string>> rows;

    string result = formatter.format(alignments, rows);

    EXPECT_EQ("", result);
}

TEST_F(TableFormatterTest, Format_EmptyAlignments)
{
    vector<char> alignments;
    vector<vector<string>> rows = {{"data"}};

    string result = formatter.format(alignments, rows);

    EXPECT_EQ("", result);
}

TEST_F(TableFormatterTest, Format_EmptyRows)
{
    vector<char> alignments = {'<'};
    vector<vector<string>> rows;

    string result = formatter.format(alignments, rows);

    EXPECT_EQ("", result);
}

TEST_F(TableFormatterTest, Format_SingleCellTable)
{
    vector<char> alignments = {'<'};
    vector<vector<string>> rows = {{"Hello"}};

    string result = formatter.format(alignments, rows);

    // Проверяем базовую структуру таблицы
    EXPECT_TRUE(result.find("Hello") != string::npos);
    EXPECT_TRUE(result.find("+") != string::npos);
    EXPECT_TRUE(result.find("|") != string::npos);
    EXPECT_TRUE(result.find("-") != string::npos);
}

TEST_F(TableFormatterTest, Format_MultiColumnTable)
{
    vector<char> alignments = {'<', '=', '>'};
    vector<vector<string>> rows = {
        {"Name", "Age", "Score"},
        {"John", "25", "100"},
        {"Alice", "30", "95"}};

    string result = formatter.format(alignments, rows);

    // Проверяем наличие всех данных
    EXPECT_TRUE(result.find("Name") != string::npos);
    EXPECT_TRUE(result.find("Age") != string::npos);
    EXPECT_TRUE(result.find("Score") != string::npos);
    EXPECT_TRUE(result.find("John") != string::npos);
    EXPECT_TRUE(result.find("Alice") != string::npos);
    EXPECT_TRUE(result.find("25") != string::npos);
    EXPECT_TRUE(result.find("30") != string::npos);
    EXPECT_TRUE(result.find("100") != string::npos);
    EXPECT_TRUE(result.find("95") != string::npos);

    // Проверяем структуру таблицы
    EXPECT_TRUE(result.find("+") != string::npos); // границы
    EXPECT_TRUE(result.find("|") != string::npos); // разделители
    EXPECT_TRUE(result.find("-") != string::npos); // горизонтальные линии
}

TEST_F(TableFormatterTest, Format_DifferentAlignments)
{
    vector<char> alignments = {'<', '=', '>'};
    vector<vector<string>> rows = {
        {"Left", "Center", "Right"}};

    string result = formatter.format(alignments, rows);

    EXPECT_TRUE(result.find("Left") != string::npos);
    EXPECT_TRUE(result.find("Center") != string::npos);
    EXPECT_TRUE(result.find("Right") != string::npos);
}

TEST_F(TableFormatterTest, Format_VaryingColumnWidths)
{
    vector<char> alignments = {'<', '>'};
    vector<vector<string>> rows = {
        {"Short", "VeryLongHeader"},
        {"VeryLongCellText", "S"}};

    string result = formatter.format(alignments, rows);

    EXPECT_TRUE(result.find("Short") != string::npos);
    EXPECT_TRUE(result.find("VeryLongHeader") != string::npos);
    EXPECT_TRUE(result.find("VeryLongCellText") != string::npos);
    EXPECT_TRUE(result.find("S") != string::npos);
}

// Тесты для TableManager
class TableManagerTest : public ::testing::Test
{
protected:
    TableManager manager;
};

TEST_F(TableManagerTest, Process_ValidTable)
{
    istringstream input("< >\nHello 123\nTest 456\n");
    ostringstream output;

    manager.process(input, output);

    string result = output.str();

    // Проверяем, что вывод содержит ожидаемые данные
    EXPECT_TRUE(result.find("Hello") != string::npos);
    EXPECT_TRUE(result.find("123") != string::npos);
    EXPECT_TRUE(result.find("Test") != string::npos);
    EXPECT_TRUE(result.find("456") != string::npos);
    EXPECT_TRUE(result.find("+") != string::npos);
    EXPECT_TRUE(result.find("|") != string::npos);
    EXPECT_FALSE(result.empty());
}

TEST_F(TableManagerTest, Process_EmptyInput)
{
    istringstream input("");
    ostringstream output;

    manager.process(input, output);

    string result = output.str();
    EXPECT_EQ("", result);
}

TEST_F(TableManagerTest, Process_ComplexTable)
{
    istringstream input("< = >\nFirst Second Third\nA BB CCC\nXX Y ZZZ\n");
    ostringstream output;

    manager.process(input, output);

    string result = output.str();

    // Проверяем наличие всех данных
    EXPECT_TRUE(result.find("First") != string::npos);
    EXPECT_TRUE(result.find("Second") != string::npos);
    EXPECT_TRUE(result.find("Third") != string::npos);
    EXPECT_TRUE(result.find("A") != string::npos);
    EXPECT_TRUE(result.find("BB") != string::npos);
    EXPECT_TRUE(result.find("CCC") != string::npos);
    EXPECT_TRUE(result.find("XX") != string::npos);
    EXPECT_TRUE(result.find("Y") != string::npos);
    EXPECT_TRUE(result.find("ZZZ") != string::npos);
}

TEST_F(TableManagerTest, Process_SingleColumnTable)
{
    istringstream input("<\nRow1\nRow2\nRow3\n");
    ostringstream output;

    manager.process(input, output);

    string result = output.str();

    EXPECT_TRUE(result.find("Row1") != string::npos);
    EXPECT_TRUE(result.find("Row2") != string::npos);
    EXPECT_TRUE(result.find("Row3") != string::npos);
    EXPECT_FALSE(result.empty());
}

TEST_F(TableManagerTest, Process_WithDifferentAlignments)
{
    istringstream input("< = >\nL C R\nLeft Center Right\n");
    ostringstream output;

    manager.process(input, output);

    string result = output.str();

    EXPECT_TRUE(result.find("L") != string::npos);
    EXPECT_TRUE(result.find("C") != string::npos);
    EXPECT_TRUE(result.find("R") != string::npos);
    EXPECT_TRUE(result.find("Left") != string::npos);
    EXPECT_TRUE(result.find("Center") != string::npos);
    EXPECT_TRUE(result.find("Right") != string::npos);
}

// Интеграционный тест - проверяем полный workflow
TEST(IntegrationTest, CompleteWorkflow)
{
    TableManager manager;
    istringstream input("< > =\nHeader1 Header2 Header3\nData1 Data22 Data333\nLast1 Last2 Last3\n");
    ostringstream output;

    manager.process(input, output);

    string result = output.str();

    // Проверяем основные компоненты форматированной таблицы
    EXPECT_FALSE(result.empty());

    // Проверяем заголовки
    EXPECT_TRUE(result.find("Header1") != string::npos);
    EXPECT_TRUE(result.find("Header2") != string::npos);
    EXPECT_TRUE(result.find("Header3") != string::npos);

    // Проверяем данные
    EXPECT_TRUE(result.find("Data1") != string::npos);
    EXPECT_TRUE(result.find("Data22") != string::npos);
    EXPECT_TRUE(result.find("Data333") != string::npos);
    EXPECT_TRUE(result.find("Last1") != string::npos);
    EXPECT_TRUE(result.find("Last2") != string::npos);
    EXPECT_TRUE(result.find("Last3") != string::npos);

    // Проверяем структуру таблицы
    size_t firstLineEnd = result.find('\n');
    string firstLine = result.substr(0, firstLineEnd);
    EXPECT_TRUE(firstLine.starts_with("+"));
    EXPECT_TRUE(firstLine.ends_with("+"));

    // Должны быть горизонтальные линии между всеми строками
    size_t lineCount = 0;
    size_t pos = 0;
    while ((pos = result.find("+", pos)) != string::npos)
    {
        lineCount++;
        pos++;
    }
    EXPECT_GE(lineCount, 6); // Как минимум 6 плюсов (2 линии по 3 колонки)
}

// Тест граничного случая - очень длинные слова
TEST_F(TableManagerTest, Process_VeryLongWords)
{
    istringstream input("<\nSupercalifragilisticexpialidocious\nShort\n");
    ostringstream output;

    manager.process(input, output);

    string result = output.str();

    EXPECT_TRUE(result.find("Supercalifragilisticexpialidocious") != string::npos);
    EXPECT_TRUE(result.find("Short") != string::npos);
    EXPECT_FALSE(result.empty());
}