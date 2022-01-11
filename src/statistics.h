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

        void printAllStatistics();

        std::map<int, std::pair<int, size_t>> evo_tree;     // child_id : (parent_id, split_cycle)
};

#endif // STATISTICS_H
