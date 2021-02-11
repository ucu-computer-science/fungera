#ifndef CPPFUNGERA_ORGANISM_H
#define CPPFUNGERA_ORGANISM_H

#include "Memory.h"
#include <cstddef>
#include <unordered_map>
#include <stack>

class Organism {
public:
    Organism(std::size_t nlines, std::size_t ncols,
             std::size_t begin_i, std::size_t begin_j);

    void execute();

private:
    // Get the location of the instruction pointer in direction vector v
    // multiplied by scalar i
    std::array<std::size_t, 2> get_ip2(std::size_t i) {
        return {ip_[0]+i*v_[0], ip_[1]+i*v_[1]};
    }

    char f(std::size_t i) {
        std::array<std::size_t, 2> ip2 = get_ip2(i);
        return (*Memory::get_instance())(ip2[0], ip2[1]);
    }

    void nop() {}

    void up() { v_ = {0, -1}; }

    void down() { v_ = {0, 1}; }

    void left() { v_ = {-1, 0}; }

    void right() { v_ = {1, 0}; }

    void find_pattern();

    void if_is_non_zero();

    void inc() { }

    void dec() { }

    void zero() { regs_[f(1)] = {0, 0}; }

    void one() { regs_[f(1)] = {1, 1}; }

    void sub() { }

    void read_op() { }

    void write_op() { }

    void push() { stack_.push(regs_[f(1)]); }

    void pop()
    {
        regs_[f(1)] = stack_.top();
        stack_.pop();
    }

    void alloc_child();

    void split_child();

    static unsigned norganisms_;
    unsigned No_;
    std::size_t nlines_, ncols_;
    std::array<std::size_t, 2> ip_;
    std::array<signed char, 2> v_ {1, 0};
    std::unordered_map<char, std::array<std::size_t, 2>> regs_ {
        {'a', {0, 0}},
        {'b', {0, 0}},
        {'c', {0, 0}},
        {'d', {0, 0}}
    };
    std::stack<std::array<std::size_t, 2>> stack_;
};

#endif //CPPFUNGERA_ORGANISM_H
