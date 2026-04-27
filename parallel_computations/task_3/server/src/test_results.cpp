#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

bool almost_equal(double a, double b, double eps = 1e-9)
{
    return std::abs(a - b) < eps * (1.0 + std::abs(a) + std::abs(b));
}

int verify_file(const fs::path &filepath)
{
    std::ifstream in(filepath);
    if (!in)
    {
        std::cerr << "Cannot open: " << filepath << "\n";
        return 1;
    }

    std::string line;
    int errors = 0, total = 0;

    while (std::getline(in, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::istringstream iss(line);
        size_t id;
        double arg1, arg2, expected, actual;
        if (!(iss >> id >> arg1 >> arg2 >> expected >> actual))
            continue;

        ++total;
        if (!almost_equal(expected, actual))
        {
            std::cerr << "FAIL [" << filepath << "] id=" << id
                      << " expected=" << expected << " actual=" << actual << "\n";
            ++errors;
        }
    }

    std::cout << filepath << ": " << (total - errors) << "/" << total << " OK\n";
    return errors;
}

int main(int argc, char *argv[])
{
    fs::path dir = (argc > 1) ? argv[1] : "results";
    if (!fs::exists(dir))
    {
        std::cerr << "Directory not found: " << dir << "\n";
        return 1;
    }

    int total_errors = 0;
    for (const auto &entry : fs::directory_iterator(dir))
    {
        if (entry.path().extension() == ".txt")
        {
            total_errors += verify_file(entry.path());
        }
    }

    if (total_errors == 0)
    {
        std::cout << "All tests PASSED \n";
        return 0;
    }
    else
    {
        std::cerr << "Tests FAILED: " << total_errors << " errors\n";
        return 1;
    }
}