#ifndef CPPFUNGERA_MEMORY_H
#define CPPFUNGERA_MEMORY_H

#include "Cell.h"
#include <cstddef>
#include <stdexcept>
#include <string>

class Memory {
public:
    Memory(Memory &other) = delete;

    void operator=(const Memory &) = delete;

    static Memory *get_instance()
    {
        static Memory memory;
        return &memory;
    }

    void init(std::size_t nlines, std::size_t ncols)
    {
        nlines_ = nlines;
        ncols_ = ncols;
        cells_ = new Cell[nlines*ncols];
    }

    std::size_t get_nlines() { return nlines_; }

    std::size_t get_ncols() { return ncols_; }

    char &operator()(std::size_t i, std::size_t j)
    {
        if (i >= nlines_ || j >= ncols_)
            throw std::out_of_range("Memory::operator()");
        return cells_[i*ncols_+j].contents;
    }

    char operator()(std::size_t i, std::size_t j) const
    {
        if (i >= nlines_ || j >= ncols_)
            throw std::out_of_range("Memory::operator()");
        return cells_[i*ncols_+j].contents;
    }

    bool is_free_cell(std::size_t i, std::size_t j)
    {
        return cells_[i*ncols_+j].is_free;
    }

    bool is_free_region(std::size_t nlines, std::size_t ncols,
                        std::size_t begin_i, std::size_t begin_j)
    {
        for (std::size_t i = begin_i; i < begin_i+nlines; ++i)
            for (std::size_t j = begin_j; j < begin_j+ncols; ++j)
                if (!cells_[i*ncols_+j].is_free) return false;
        return true;
    }

    void alloc(std::size_t nlines, std::size_t ncols,
               std::size_t begin_i, std::size_t begin_j)
    {
        f(nlines, ncols, begin_i, begin_j, false);
    }

    void free(std::size_t nlines, std::size_t ncols,
              std::size_t begin_i, std::size_t begin_j)
    {
        f(nlines, ncols, begin_i, begin_j, true);
    }

    std::size_t load_genome(const std::string &file_name,
                            std::size_t begin_i, std::size_t begin_j,
                            std::size_t &ncols);

protected:
    // Do not initialize the members
    Memory() {}

    // TODO: Name this fucntion properly
    void f(std::size_t nlines, std::size_t ncols,
           std::size_t begin_i, std::size_t begin_j,
           bool is_free)
    {
        for (std::size_t i = begin_i; i < begin_i+nlines; ++i)
            for (std::size_t j = begin_j; j < begin_j+ncols; ++j)
                cells_[i*ncols_+j].is_free = is_free;
    }

    std::size_t nlines_;
    std::size_t ncols_;
    Cell *cells_;
};

#endif //CPPFUNGERA_MEMORY_H
