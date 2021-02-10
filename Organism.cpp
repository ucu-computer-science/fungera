#include "Organism.h"

#include <vector>
#include <algorithm>

unsigned Organism::norganisms = 0;

void Organism::find_pattern()
{
    std::vector<char> pattern;
    std::size_t i;
    const std::size_t maxn = *std::max_element(size_.begin(), size_.end());
    for (i = 2; i < maxn; ++i) {
        char c2;
        if ((c2 = get_next_ith_op(i)) == '.' || c2 == ':')
            pattern.push_back((c2 == '.') ? ':' : '.');
        else
            break;
    }
    for (std::size_t j = 0; i < maxn; ++i) {
        if (get_next_ith_op(i) == pattern[j])
            ++j;
        else
            j = 0;
        if (j == pattern.size()) {
            registers_[get_next_ith_op(1)] = {ip_[0]+i*delta_[0],ip_[1]+i*delta_[1]};
            break;
        }
    }
}
