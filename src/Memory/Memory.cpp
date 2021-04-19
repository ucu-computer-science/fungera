#include "Memory.h"
#include <fstream>

size_t_arr Memory::load_genome(const std::string &file_name, size_t_arr begin)
{
    std::ifstream in(file_name);
    if (!in)
        throw std::runtime_error("Couldn't open file for reading.");

    std::string line;
    size_t i = begin[0];
    size_t j;
    while (std::getline(in, line)) {
        j = begin[1];
        for (char c : line) {
            (*this)(i, j) = c;
            ++j;
        }
        ++i;
    }

    in.close();

    size_t_arr size = {i-begin[0], j-begin[1]};
    alloc(size, begin);

    return size;
}
