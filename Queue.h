#ifndef CPPFUNGERA_QUEUE_H
#define CPPFUNGERA_QUEUE_H

#include "Organism.h"
#include <vector>

class Queue {
public:
    Queue(const Queue &other) = delete;

    void operator=(const Queue &) = delete;

    static Queue *get_instance()
    {
        static Queue queue;
        return &queue;
    }

    void execute_all() { for (Organism o : organisms) o.execute(); }

protected:
    Queue() = default;

    std::vector<Organism> organisms;
};

#endif //CPPFUNGERA_QUEUE_H
