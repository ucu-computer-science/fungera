#ifndef STATISTICS_H
#define STATISTICS_H

#include "src/organism.h"
#include "src/point.h"
#include <map>
#include <vector>

class Statistics {
    public:
        Statistics() { }
        double entropy(OrganismQueue *oq);
        std::map<Point, int> sizeNOrgs();
        void addRelation(int child_id, int parent_id, size_t cycle_no);
        static unsigned hammingDistance(Organism *org, Organism *an_org);
        static std::vector<Point> locationsOfDifference(Organism *org, Organism *an_org);

        void printAllStatistics();

        std::map<int, std::pair<int, size_t>> evo_tree;     // child_id : (parent_id, split_cycle)


        template<class D>
        static inline long long to_us(const D& d);

        static inline std::chrono::high_resolution_clock::time_point get_current_time_fenced();

};

template<class D>
inline long long Statistics::to_us(const D& d)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}
inline std::chrono::high_resolution_clock::time_point Statistics::get_current_time_fenced()
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

#endif // STATISTICS_H
