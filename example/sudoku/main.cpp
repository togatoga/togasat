#include <iostream>
#include <cassert>
#include "../../togasat.hpp"
int height,width;
int calculate_index(int y, int x, int value)
{
    assert(value >= 1 && value <= 9);
    int index = 9 * 9 * y + 9 * x + value;
    return index;
}
int main() {
    togasat::Solver solver;
    std::cin >> height >> width;
    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            std::string text;
            std::cin >> text;
        }
    }
}