#ifndef CPPFUNGERA_ORGANISM_H
#define CPPFUNGERA_ORGANISM_H

#include "Memory.h"
#include <array>
#include <cstddef>
#include <unordered_map>

class Organism {
public:
    Organism(std::array<std::size_t, 2> size, std::array<std::size_t, 2> address):
        No_(norganisms), size_(size), address_(address) { ++norganisms; }

    char get_next_ith_op(std::size_t i) { return Memory::get_instance()->get_cell_contents(ip_[0]+i*delta_[0], ip_[1]+i*delta_[1]); }

    void nop() {}

    void up() { delta_ = {-1, 0}; }

    void down() { delta_ = {1, 0}; }

    void left() { delta_ = {0, -1}; }

    void right() { delta_ = {0, 1}; }

    void if_not_zero();

    void find_pattern();

    void zero() { registers_[get_next_ith_op(1)] = {0, 0}; }

    void one() { registers_[get_next_ith_op(1)] = {1, 1}; }

    void sub()
    {
        std::array<std::size_t, 2> lhs = registers_[get_next_ith_op(1)];
        std::array<std::size_t, 2> rhs = registers_[get_next_ith_op(2)];
        registers_[get_next_ith_op(3)] = {lhs[0]-rhs[0], lhs[1]-rhs[1]};
    }

    void execute();

private:
    static unsigned norganisms;
    unsigned No_;
    const std::array<std::size_t, 2> size_;
    std::array<std::size_t, 2> address_;
    std::array<std::size_t, 2> ip_ = address_;
    std::array<signed char, 2> delta_ = {0, 1};
    std::unordered_map<char, std::array<std::size_t, 2>> registers_;
};

#endif //CPPFUNGERA_ORGANISM_H
