#include <iostream>
#include <fstream>
#include <ostream>
#include <stdexcept>
#include <tuple>
#include <string>
#include <type_traits>
#include <iterator>
#include <vector>

// todo тесты написать

// остановка рекурсии
// когда I дошел до конца (до I == sizeof...(Args)) рекурсия стоп
template <std::size_t I = 0, typename... Args, typename Ch, typename Tr>
typename std::enable_if<(I == sizeof...(Args)), void>::type
printTuple(std::basic_ostream<Ch, Tr> &out, std::tuple<Args...> t)
{
    return;
}

// тело рекурсии
// печатает I-ый элемент кортежа, вызывает себя на I+1
// перед каждым элементом кроме первого запятая
template <size_t I = 0, typename... Args, typename Ch, typename Tr>
typename std::enable_if<(I < sizeof...(Args)), void>::type
printTuple(std::basic_ostream<Ch, Tr> &out, std::tuple<Args...> t)
{
    if (I > 0)
        out << ", ";
    out << std::get<I>(t);
    printTuple<I + 1>(out, t);
}

// перегрузка оператора << для std::tuple
template <typename... Args, typename Ch, typename Tr>
std::ostream &operator<<(std::basic_ostream<Ch, Tr> &out, std::tuple<Args...> const &t)
{
    printTuple(out, t);
    return out;
}

// конвертация строки в базовый тип
template <typename T>
T convert(std::string const &x)
{
    try
    {
        if constexpr (std::is_same<T, int>::value)
            return std::stoi(x.c_str());
        if constexpr (std::is_same<T, float>::value)
            return std::stof(x.c_str());
        if constexpr (std::is_same<T, double>::value)
            return std::stod(x.c_str());
        if constexpr (std::is_same<T, std::string>::value)
            return x;
    }
    catch (const std::invalid_argument &e)
    {
        throw;
    }
}
// конвертация колонок
template <class... Ts, size_t... Idxs>
std::tuple<Ts...>
parse(std::vector<std::string> const &values, std::index_sequence<Idxs...>)
{
    return {convert<Ts>(values[Idxs])...};
}
// конвертация списка строк в tuple
template <class... Ts>
std::tuple<Ts...> vector2tuple(std::vector<std::string> const &values)
{
    return parse<Ts...>(values, std::make_index_sequence<sizeof...(Ts)>{});
}

#if !defined(CSVPARSER_DELIM)
#define CSVPARSER_DELIM ','
#endif

#if !defined(CSVPARSER_EOL)
#define CSVPARSER_EOL '\n'
#endif

#if !defined(CSVPARSER_ESCAPE)
#define CSVPARSER_ESCAPE '"'
#endif

template <typename... Args>
class CSVParser
{
private:
    using val = std::tuple<Args...>; // тип одной строки данных. например int, double
    std::ifstream &file_;
    val current_;

    int line_ = 1;
    int column_ = 0;
    bool err_ = false;

    int N;
    char delim = CSVPARSER_DELIM;
    char endl = CSVPARSER_EOL;
    char escap = CSVPARSER_ESCAPE;

public:
    class iterator
    {
        // чтобы использовать CSVParser range-based for
        // or begin()/end()
        CSVParser *x_;

    public:
        using value_type = val;
        using reference = const val &;
        using pointer = const val *;
        using iterator_category = std::input_iterator_tag;

        iterator(CSVParser *x = nullptr) : x_{x} {}
        reference operator*() const { return x_->current_; }
        iterator &operator++()
        {
            increment();
            return *this;
        }
        iterator operator++(int)
        {
            increment();
            return *this;
        }
        bool operator==(iterator rhs) const { return x_ == rhs.x_; }
        bool operator!=(iterator rhs) const { return !(rhs == *this); }

    protected:
        void increment()
        {
            x_->next();
            if (!x_->valid() || x_->err_)
            {
                x_ = nullptr;
                return;
            }
        }
    };
    iterator begin()
    {
        // Если сразу после конструирования у нас ошибка или нет данных — вернуть end()
        if (!valid() || err_)
        {
            return end();
        }
        return iterator{this};
    }
    iterator end() { return iterator{}; }

    CSVParser(std::ifstream &f, int const skip_lines = 0) : file_(f), N(skip_lines)
    {
        // принимает открытй файл, пропускает skip_lines строк, читает первую после
        for (int i = 0; i < N; ++i)
            skip();
        next();
    }
    void setDelimeter(char const d = CSVPARSER_DELIM)
    {
        if (d == '\0')
            return;
        this->delim = d;
    }
    void setEOL(char const eol = CSVPARSER_EOL)
    {
        if (eol == '\0')
            return;
        this->endl = eol;
    }
    void setESCAPE(char const esc = CSVPARSER_ESCAPE)
    {
        if (esc == '\0')
            return;
        this->escap = esc;
    }
    bool valid() const
    {
        return !file_.eof() && !err_;
    }
    void skip()
    {
        char c;
        while (file_.get(c))
        {
            if (c == endl)
                break;
        }
        ++line_;
    }
    void next()
    {
        // читает символы по одному из файла
        // нашел " - флаг escaped, с ним любые символы просто добавляются в строку
        // потом в конуе должен проверить что " закрыто
        // если нашел разделители , \n то завершанет колонку\строку
        // вектор строк в кортеж чрез vector2tuple
        using tmp_store = std::vector<std::string>;
        bool escaped = false;
        char c;
        std::string ss;
        int i = 0;
        int max_i = std::tuple_size<val>{};
        tmp_store tmp;
        while (file_.get(c))
        {
            if (c == escap)
            {
                escaped = !escaped;
                continue;
            }
            if (escaped)
            {
                ss += c;
                continue;
            }
            if (c == delim || c == endl)
            {
                tmp.push_back(std::move(ss));
                ++i;
                ss.clear();
                if (c == endl || i == max_i)
                    break;
                continue;
            }
            ss += c;
        }
        /*if (file_.eof()) {
            err_ = true;
            current_ = val{};
            return;
        }*/
        if (escaped)
        {
            throw std::runtime_error("Parsing error. Escaped simvol was lost.");
        }
        if (line_ > N)
        {
            if (tmp.empty())
            {
                err_ = true;
                current_ = val{};
                return;
            }
            if (i < max_i)
                tmp.push_back(std::move(ss));

            try
            {
                auto t = vector2tuple<Args...>(tmp);
                current_ = std::move(t);
            }
            catch (const std::exception &exc)
            {
                std::cout << "Parsing error. Line: " << line_ << std::endl;
                err_ = true;
                current_ = val{};
                throw;
            }
        }
        ++line_;
    }
};

#ifndef UNIT_TESTING
int main()
{
    std::ifstream file("test.txt");
    if (!file.is_open())
    {
        std::cerr << "Cannot open test.txt\n";
        return 1;
    }
    CSVParser<int, std::string, double> parser{file, 0};
    try
    {
        auto it = parser.begin();
        auto end = parser.end();
        for (; it != end; ++it)
        {
            std::cout << *it << std::endl;
        }
    }
    catch (std::runtime_error &err)
    {
        std::cout << "ERROR:" << err.what() << "\n";
    }
    catch (std::invalid_argument &err)
    {
        std::cout << "ERROR:" << err.what() << "\n";
    }

    return 0;
}
#endif