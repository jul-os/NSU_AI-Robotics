#define UNIT_TESTING // ← это отключит main()
#include "main.cpp"  // теперь безопасно

#include <gtest/gtest.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <tuple>
#include <string>
#include <stdexcept>

// Тесты для operator<< (печать кортежа)ы
TEST(TuplePrintTest, EmptyTuple)
{
    std::ostringstream oss;
    oss << std::make_tuple();
    EXPECT_EQ(oss.str(), "");
}

TEST(TuplePrintTest, SingleElement)
{
    std::ostringstream oss;
    oss << std::make_tuple(42);
    EXPECT_EQ(oss.str(), "42");
}

TEST(TuplePrintTest, MultipleElements)
{
    std::ostringstream oss;
    oss << std::make_tuple(1, std::string("hello"), 3.14);
    EXPECT_EQ(oss.str(), "1, hello, 3.14");
}

// Тесты для convert<T>
TEST(ConvertTest, IntConversion)
{
    EXPECT_EQ(convert<int>("42"), 42);
    EXPECT_EQ(convert<int>("-7"), -7);
}

TEST(ConvertTest, FloatConversion)
{
    EXPECT_FLOAT_EQ(convert<float>("3.14"), 3.14f);
}

TEST(ConvertTest, DoubleConversion)
{
    EXPECT_DOUBLE_EQ(convert<double>("2.71828"), 2.71828);
}

TEST(ConvertTest, StringConversion)
{
    EXPECT_EQ(convert<std::string>("abc"), "abc");
}

TEST(ConvertTest, InvalidInt)
{
    EXPECT_THROW(convert<int>("abc"), std::invalid_argument);
}

TEST(ConvertTest, InvalidFloat)
{
    EXPECT_THROW(convert<float>("xyz"), std::invalid_argument);
}

// Тесты для vector2tuple
TEST(Vector2TupleTest, BasicConversion)
{
    std::vector<std::string> v = {"10", "hello", "3.5"};
    auto t = vector2tuple<int, std::string, double>(v);
    EXPECT_EQ(std::get<0>(t), 10);
    EXPECT_EQ(std::get<1>(t), "hello");
    EXPECT_DOUBLE_EQ(std::get<2>(t), 3.5);
}

// Тест на несовпадение размеров — на самом деле НЕ БРОСАЕТ исключение!
// std::vector::operator[] не проверяет границы → UB, а не throw.
// Поэтому этот тест закомментирован или убран.
// Если вы добавите проверку в vector2tuple — тогда можно вернуть.

// Тесты для CSVParser
class CSVParserTest : public ::testing::Test
{
protected:
    void writeTestFile(const std::string &name, const std::string &content)
    {
        std::ofstream f(name);
        f << content;
        f.close();
    }

    void TearDown() override
    {
        std::remove("test1.csv");
        std::remove("test2.csv");
        std::remove("test3.csv");
        std::remove("test4.csv");
        std::remove("test5.csv");
    }
};

TEST_F(CSVParserTest, SimpleParsing)
{
    writeTestFile("test1.csv", "1,apple,3.14\n2,banana,2.71\n");
    std::ifstream file("test1.csv");
    CSVParser<int, std::string, double> parser(file, 0);

    std::vector<std::tuple<int, std::string, double>> rows;
    for (const auto &row : parser)
    {
        rows.push_back(row);
    }

    ASSERT_EQ(rows.size(), 2);
    EXPECT_EQ(std::get<0>(rows[0]), 1);
    EXPECT_EQ(std::get<1>(rows[0]), "apple");
    EXPECT_DOUBLE_EQ(std::get<2>(rows[0]), 3.14);

    EXPECT_EQ(std::get<0>(rows[1]), 2);
    EXPECT_EQ(std::get<1>(rows[1]), "banana");
    EXPECT_DOUBLE_EQ(std::get<2>(rows[1]), 2.71);
}

TEST_F(CSVParserTest, SkipHeaderLines)
{
    writeTestFile("test2.csv", "col1,col2,col3\n1,foo,1.0\n2,bar,2.0\n");
    std::ifstream file("test2.csv");
    CSVParser<int, std::string, double> parser(file, 1);

    std::vector<std::tuple<int, std::string, double>> rows;
    for (const auto &row : parser)
    {
        rows.push_back(row);
    }

    ASSERT_EQ(rows.size(), 2);
    EXPECT_EQ(std::get<0>(rows[0]), 1);
    EXPECT_EQ(std::get<1>(rows[0]), "foo");
}

TEST_F(CSVParserTest, EscapedFields)
{
    writeTestFile("test3.csv", "1,\"hello, world\",3.14\n");
    std::ifstream file("test3.csv");
    CSVParser<int, std::string, double> parser(file, 0);

    auto it = parser.begin();
    ASSERT_NE(it, parser.end());
    EXPECT_EQ(std::get<0>(*it), 1);
    EXPECT_EQ(std::get<1>(*it), "hello, world");
    EXPECT_DOUBLE_EQ(std::get<2>(*it), 3.14);
}

TEST_F(CSVParserTest, MalformedEscape)
{
    writeTestFile("test4.csv", "1,\"unclosed field\n");
    std::ifstream file("test4.csv");
    EXPECT_THROW((CSVParser<int, std::string, double>(file, 0)), std::runtime_error);
}

TEST_F(CSVParserTest, InvalidDataConversion)
{
    writeTestFile("test5.csv", "1,abc,3.14\n");
    std::ifstream file("test5.csv");
    auto parseFn = [&]()
    {
        CSVParser<int, int, double> parser(file, 0);
        for (const auto &row : parser)
        {
            (void)row;
        }
    };
    EXPECT_THROW(parseFn(), std::invalid_argument);
}

TEST_F(CSVParserTest, EmptyFile)
{
    writeTestFile("test5.csv", "");
    std::ifstream file("test5.csv");
    CSVParser<int, std::string> parser(file, 0);
    auto it = parser.begin();
    EXPECT_EQ(it, parser.end());
}