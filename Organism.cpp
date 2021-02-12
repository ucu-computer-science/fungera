#include "Organism.h"
#include "Queue.h"
#include <unordered_map>
#include <vector>
#include <stdexcept>

#define min(x, y) ((x) < (y)) ? (x) : (y)
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
        {'?', &Organism::if_zero},
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
        {'.', {0, 0}},
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

Organism::Organism(std::array<std::size_t, 2> size,
                   std::array<std::size_t, 2> begin):
    size_(size), ip_(begin)
{
    Queue::get_instance()->push_back(this);
}

void Organism::find_pattern()
{
    char c1 = f(1);
    // If c1 is none of the registers
    if (c1 != 'a' && c1 != 'b' && c1 != 'c' && c1 != 'd')
        throw std::out_of_range("Organism::find_pattern()");
    std::vector<char> pattern;
    std::size_t i;
    const std::size_t max_side = max(size_[0], size_[1]);
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

// This function is called if_not_zero() in the Python version, which
// doesn't make sense if you look at it as "if condition then this else
// this", therefore it's been renamed. Also take note that the IP
// advances on one additional position after the execution
void Organism::if_zero()
{
    char c;
    if ((c = f(1)) == 'x' || c == 'y') {
        if (regs_.at(f(2))[(c == 'x') ? 0 : 1])
            ip_ = get_ip2(3);
        else
            ip_ = get_ip2(2);
    } else
        if (regs_.at(c)[0] || regs_.at(c)[1])
            ip_ = get_ip2(2);
        else
            ip_ = get_ip2(1);
}

void Organism::alloc_child()
{
    auto child_size = regs_.at(f(1));
    char c2 = f(2);
    if (c2 != 'a' && c2 != 'b' && c2 != 'c' && c2 != 'd')
        throw std::out_of_range("Organism::alloc_child()");
    Memory *memory = Memory::get_instance();
    if (child_size[0] != 0 && child_size[1] != 0) {
        for (std::size_t i = 2; i < min(memory->get_nlines(), memory->get_ncols()); ++i) {
            auto child_begin = get_ip2(i);
            if (memory->is_free_region(child_size, child_begin)) {
                child_size_ = child_size;
                child_begin_ = child_begin;
                regs_.at(c2) = child_begin;
                memory->alloc(child_size, child_begin);
                break;
            }
        }
    }
}

void Organism::split_child()
{
    if (child_size_[0] != 0 && child_size_[1] != 0) {
        new Organism(child_size_, child_begin_);
        child_size_ = {0, 0};
        // Don't set child_begin_ to 0, because no checks for it are
        // made
    }
}
