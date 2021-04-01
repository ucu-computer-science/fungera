#include "Memory.h"
#include <fstream>

std::array<std::size_t, 2> Memory::load_genome(const std::string &file_name,
                                               std::array<std::size_t, 2> begin)
{
    std::ifstream in;
    in.open(file_name);
    std::string line;
    std::size_t i = begin[0];
    std::size_t j;
    while (std::getline(in, line)) {
        j = begin[1];
        for (char c : line) {
            (*this)(i, j) = c;
            ++j;
        }
        ++i;
    }
    in.close();
    std::array<std::size_t, 2> size = {i-begin[0], j-begin[1]};
    alloc(size, begin);
    return size;
}
