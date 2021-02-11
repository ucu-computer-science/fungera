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

    void exec();

//private:
    // Get the location of the instruction pointer in direction vector v
    // multiplied by scalar i
    std::array<std::size_t, 2> get_ip2(std::size_t i) {
        return {ip_[0]+i*v_[1], ip_[1]+i*v_[0]};
    }

    // TODO: Name this function properly
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

    void if_non_zero();

    void inc()
    {
        char c;
        if ((c = f(1)) == 'x' || c == 'y')
            ++regs_[f(2)][(c == 'x') ? 0 : 1];
        else {
            ++regs_[c][0];
            ++regs_[c][1];
        }
    }

    void dec()
    {
        char c;
        if ((c = f(1)) == 'x' || c == 'y')
            --regs_[f(2)][(c == 'x') ? 0 : 1];
        else {
            --regs_[c][0];
            --regs_[c][1];
        }
    }

    void zero() { regs_.at(f(1)) = {0, 0}; }

    void one() { regs_.at(f(1)) = {1, 1}; }

    void sub()
    {
        char c1 = f(1), c2 = f(2), c3 = f(3);
        regs_.at(c3) = {regs_.at(c1)[0]-regs_.at(c2)[0],
                        regs_.at(c1)[1]-regs_.at(c2)[0]};
    }

    void read()
    {
        char c1 = f(1), c2 = f(2);
        std::size_t i = regs_[c1][0], j = regs_[c1][1];
        auto opcode = map2_.at((*Memory::get_instance())(i, j));
        regs_[c2] = {opcode[0], opcode[1]};
    }

    void write()
    {
        char c1 = f(1), c2 = f(2);
        for (const auto &pair : map2_)
            if (regs_[c2][0] == pair.second[0] && regs_[c2][1] == pair.second[1]) {
                (*Memory::get_instance())(regs_[c1][0], regs_[c1][1]) = pair.first;
                break;
            }
    }

    void push() { stack_.push(regs_.at(f(1))); }

    void pop()
    {
        regs_.at(f(1)) = stack_.top();
        stack_.pop();
    }

    void alloc_child();

    void split_child();

    using operation = void (Organism::*)();
    static const std::unordered_map<char, operation> map1_;
    static const std::unordered_map<char, std::array<unsigned char, 2>> map2_;
    static unsigned norganisms_;
    const unsigned No_ = norganisms_;
    const std::size_t nlines_, ncols_;
    std::array<std::size_t, 2> ip_;
    std::array<signed char, 2> v_{1, 0};
    std::unordered_map<char, std::array<std::size_t, 2>> regs_{
            {'a', {0, 0}},
            {'b', {0, 0}},
            {'c', {0, 0}},
            {'d', {0, 0}}
    };
    std::stack<std::array<std::size_t, 2>> stack_;
    unsigned nerrors_ = 0;
};

#endif //CPPFUNGERA_ORGANISM_H
