#ifndef CPPFUNGERA_MEMORY_H
#define CPPFUNGERA_MEMORY_H

#include "Cell.h"
#include <cstddef>

class Memory {
public:
    Memory(std::size_t nlines, std::size_t ncols):
        ncols(ncols), cells(new Cell[nlines*ncols]) { }
    ~Memory() { delete[] cells; }
    char get_cell_contents(std::size_t i, std::size_t j) { return cells[i*ncols+j].contents; }
    void set_cell_contents(std::size_t i, std::size_t j, char contents) { cells[i*ncols+j].contents = contents; }
    bool get_cell_freedom(std::size_t i, std::size_t j) { return cells[i*ncols+j].is_free; }
    void set_cell_freedom(std::size_t i, std::size_t j, bool is_free) { cells[i*ncols+j].is_free = is_free; }
private:
    const std::size_t ncols;
    Cell *cells;
};

#endif //CPPFUNGERA_MEMORY_H
