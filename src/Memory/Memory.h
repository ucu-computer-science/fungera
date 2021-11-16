#ifndef CPPFUNGERA_MEMORY_H
#define CPPFUNGERA_MEMORY_H

#include "../arr.h"
#include "../Cell.h"
#include <array>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <iostream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/array.hpp>

class Memory {

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & nlines_;
        ar & ncols_;
        for(size_t i = 0; i < nlines_*ncols_; i++){
            ar & cells_[i];
        }
    }
public:
    Memory(Memory &other) = delete;

    void operator=(const Memory &) = delete;

    static Memory *get_instance()
    {
        static Memory memory;
        return &memory;
    }

    void init(size_t nlines, size_t ncols)
    {
        nlines_ = nlines;
        ncols_ = ncols;
        cells_ = new Cell[nlines*ncols];
    }

    Cell* get_cells() const { return cells_; }

    size_t get_nlines() const { return nlines_; }

    size_t get_ncols() const { return ncols_; }

    char &operator()(size_t i, size_t j)
    {
        if (i >= nlines_ || j >= ncols_) {
            std::cerr << i << ' ' << j << '\n';
            throw std::out_of_range("Memory::operator()");
        }

        return cells_[i*ncols_+j].contents_;
    }

    char operator()(size_t i, size_t j) const
    {
        if (i >= nlines_ || j >= ncols_)
            throw std::out_of_range("Memory::operator()");

        return cells_[i*ncols_+j].contents_;
    }

    bool is_free_cell(std::size_t i, std::size_t j) { return cells_[i*ncols_+j].is_free_; }

    bool is_free_region(size_t_arr size, size_t_arr begin)
    {
        if (begin[0] >= nlines_ || begin[1] >= ncols_
            || begin[0]+size[0] >= nlines_ || begin[1]+size[1] >= ncols_)
            throw std::out_of_range("Memory::is_free_region()");

        for (std::size_t i = begin[0]; i < begin[0]+size[0]; ++i)
            for (std::size_t j = begin[1]; j < begin[1]+size[1]; ++j)
                if (!cells_[i*ncols_+j].is_free_) return false;
        return true;
    }

    void alloc(size_t_arr size, size_t_arr begin)
    {
        f(size, begin, false);
    }

    void free(size_t_arr size, size_t_arr begin)
    {
        f(size, begin, true);
    }

    size_t_arr load_genome(const std::string &file_name, size_t_arr begin);

protected:
    // Do not initialize the members
    Memory() {}

    // Set the freedom of the region of the memory
    void f(size_t_arr size, size_t_arr begin, bool is_free)
    {
        // Assuming that is_free_region() has been called
        for (std::size_t i = begin[0]; i < begin[0]+size[0]; ++i)
            for (std::size_t j = begin[1]; j < begin[1]+size[1]; ++j)
                cells_[i*ncols_+j].is_free_ = is_free;
    }

    std::size_t nlines_;
    std::size_t ncols_;
    Cell *cells_;
};

#endif //CPPFUNGERA_MEMORY_H
