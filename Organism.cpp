#include "Organism.h"
#include "Queue.h"
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <iostream>

#define max(x, y) ((x) > (y)) ? (x) : (y)

using operation = void (Organism::*)();
const std::unordered_map<char, operation> Organism::map1_{
        {'.', &Organism::nop},
        {':', &Organism::nop},
        {'a', &Organism::nop},
        {'b', &Organism::nop},
        {'c', &Organism::nop},
        {'d', &Organism::nop},
        {'x', &Organism::nop},
        {'y', &Organism::nop},
        {'^', &Organism::up},
        {'v', &Organism::down},
        {'>', &Organism::right},
        {'<', &Organism::left},
        {'&', &Organism::find_pattern},
        {'?', &Organism::if_non_zero},
        {'1', &Organism::one},
        {'0', &Organism::zero},
        {'-', &Organism::dec},
        {'+', &Organism::inc},
        {'~', &Organism::sub},
        {'L', &Organism::read},
        {'W', &Organism::write},
        {'@', &Organism::alloc_child},
        {'$', &Organism::split_child},
        {'S', &Organism::push},
        {'P', &Organism::pop}
};
const std::unordered_map<char, std::array<unsigned char, 2>> Organism::map2_{
        {':', {0, 1}},
        {'a', {1, 0}},
        {'b', {1, 1}},
        {'c', {1, 2}},
        {'d', {1, 3}},
        {'x', {2, 0}},
        {'y', {2, 1}},
        {'^', {3, 0}},
        {'v', {3, 1}},
        {'>', {3, 2}},
        {'<', {3, 3}},
        {'&', {4, 0}},
        {'?', {5, 0}},
        {'1', {6, 0}},
        {'0', {6, 1}},
        {'-', {6, 2}},
        {'+', {6, 3}},
        {'~', {6, 4}},
        {'L', {7, 0}},
        {'W', {7, 1}},
        {'@', {7, 2}},
        {'$', {7, 3}},
        {'S', {8, 0}},
        {'P', {8, 1}}
};
unsigned Organism::norganisms_ = 1;

Organism::Organism(std::size_t nlines, std::size_t ncols,
                   std::size_t begin_i, std::size_t begin_j):
    nlines_(nlines), ncols_(ncols), ip_({begin_i, begin_j})
{
    Memory::get_instance()->alloc(nlines, ncols, begin_j, begin_j);
    Queue::get_instance()->push_back(this);
    ++norganisms_;
}

void Organism::exec()
{
    try {
        (this->*map1_.at(f(0)))();
    } catch (std::out_of_range &e_obj) {
        ++nerrors_;
    }
    ip_ = get_ip2(1);
}

/* Find the complementary pattern to the given one */
void Organism::find_pattern()
{
    char c1 = f(1);
    // If c1 is none of the registers
    if (c1 != 'a' && c1 != 'b' && c1 != 'c' && c1 != 'd')
        throw std::out_of_range("Organism::find_pattern()");
    std::vector<char> pattern;
    std::size_t i;
    const std::size_t max_side = max(nlines_, ncols_);
    for (i = 2; i < max_side; ++i) {
        char c2;
        if ((c2 = f(i)) == '.' || c2 == ':')
            pattern.push_back((c2 == '.') ? ':' : '.');
        else
            break;
    }
    for (std::size_t j = 0; i < max_side; ++i) {
        if (f(i) == pattern[j])
            ++j;
        else
            j = 0;
        if (j == pattern.size()) {
            regs_.at(c1) = get_ip2(i);
            break;
        }
    }
}

/* The analog of if-else construction in other languages. Warning: after
 * the execution of this operation the IP will advance on one more
 * position */
void Organism::if_non_zero()
{
    char c;
    if ((c = f(1)) == 'x' || c == 'y') {
        if (regs_.at(f(2))[(c == 'x') ? 0 : 1])
            ip_ = get_ip2(3);
        else
            ip_ = get_ip2(2);
    } else
        if (regs_.at(c)[0] && regs_.at(c)[1])
            ip_ = get_ip2(2);
        else
            ip_ = get_ip2(1);
}

void Organism::alloc_child()
{
    std::array<std::size_t, 2> size = regs_[f(1)];
    bool is_free_region_found = false;
    for (std::size_t i = 2; i < 5000; ++i) {
        if (Memory::get_instance()->is_free_region(size[0], size[1], get_ip2(i)[0], get_ip2(i)[1])) {
            std::array<std::size_t, 2> child_begin = get_ip2(i);
            regs_.at(f(2)) = child_begin;
            break;
        }
    }
}

void Organism::split_child()
{

}
