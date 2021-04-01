#ifndef CPPFUNGERA_MEMORY_H
#define CPPFUNGERA_MEMORY_H

#include "Cell.h"
#include <array>
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

    bool is_free_region(std::array<std::size_t, 2> size, std::array<std::size_t, 2> begin)
    {
        if (begin[0] >= nlines_ || begin[1] >= ncols_
            || begin[0]+size[0] >= nlines_ || begin[1]+size[1] >= ncols_)
            throw std::out_of_range("Memory::is_free_region()");
        for (std::size_t i = begin[0]; i < begin[0]+size[0]; ++i)
            for (std::size_t j = begin[1]; j < begin[1]+size[1]; ++j)
                if (!cells_[i*ncols_+j].is_free) return false;
        return true;
    }

    void alloc(std::array<std::size_t, 2> n, std::array<std::size_t, 2> begin)
    {
        f(n, begin, false);
    }

    void free(std::array<std::size_t, 2> n, std::array<std::size_t, 2> begin)
    {
        f(n, begin, true);
    }

    std::array<std::size_t, 2> load_genome(const std::string &file_name,
                                           std::array<std::size_t, 2> begin);

protected:
    // Do not initialize the members
    Memory() {}

    /* Set the freedom of the region of the memory */
    void f(std::array<std::size_t, 2> size, std::array<std::size_t, 2> begin,
           bool is_free)
    {
        // Assuming that is_free_region() has been called
        for (std::size_t i = begin[0]; i < begin[0] + size[0]; ++i)
            for (std::size_t j = begin[1]; j < begin[1] + size[1]; ++j)
                cells_[i*ncols_+j].is_free = is_free;
    }

    std::size_t nlines_;
    std::size_t ncols_;
    Cell *cells_;
};

#endif //CPPFUNGERA_MEMORY_H
