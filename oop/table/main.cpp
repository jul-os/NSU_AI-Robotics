#include "table.hpp"
#include <fstream>
#include <iostream>

int main()
{
    TableManager manager;

    std::ifstream input("input.txt");
    std::ofstream output("output.txt");

    if (!input.is_open())
    {
        std::cerr << "Error: Cannot open input file!" << std::endl;
        return 1;
    }

    if (!output.is_open())
    {
        std::cerr << "Error: Cannot open output file!" << std::endl;
        return 1;
    }

    manager.process(input, output);

    std::cout << "Table processed successfully!" << std::endl;
    return 0;
}