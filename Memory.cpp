#include <iostream>
#include "Memory.h"

std::size_t Memory::load_genome(const std::string &file_name,
                                std::size_t begin_i, std::size_t begin_j,
                                std::size_t &ncols)
{
    std::ifstream in;
    in.open(file_name);
    std::string line;
    std::size_t i = begin_i;
    std::size_t j;
    while (std::getline(in, line)) {
        j = begin_j;
        for (char c : line) {
            (*this)(i, j) = c;
            ++j;
        }
        ++i;
    }
    ncols = j - begin_j;
    in.close();
    return i - begin_i;
}
