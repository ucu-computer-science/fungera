#include "Organism.h"
#include "Queue.h"
#include <vector>

#define max(x, y) ((x) > (y)) ? (x) : (y)

unsigned Organism::norganisms_ = 0;

Organism::Organism(std::size_t nlines, std::size_t ncols,
                   std::size_t begin_i, std::size_t begin_j):
    No_(norganisms_), nlines_(nlines), ncols_(ncols), ip_({begin_i, begin_j})
{
    Queue::get_instance()->push_back(this);
    ++norganisms_;
}

void Organism::execute()
{

}

/* Find the complementary pattern to the given one */
void Organism::find_pattern()
{
    char c1 = f(1);
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
            regs_[c1] = get_ip2(i);
            break;
        }
    }
}

void Organism::if_is_non_zero()
{

}

void Organism::alloc_child()
{

}

void Organism::split_child()
{

}
