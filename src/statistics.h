#ifndef STATISTICS_H
#define STATISTICS_H

#include "src/organismqueue.h"
#include "src/point.h"
#include <map>
#include <vector>

class Statistics {
    public:
        double entropy(OrganismQueue *oq);
        std::map<Point, int> sizeNOrgs();
};

#endif // STATISTICS_H
