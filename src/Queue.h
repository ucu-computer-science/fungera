#ifndef CPPFUNGERA_QUEUE_H
#define CPPFUNGERA_QUEUE_H

#include "Organism/Organism.h"
#include <vector>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/array.hpp>

class Queue {
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & next_idx;
        // ar & organisms_;
    }
public:
    Queue(const Queue &other) = delete;

    void operator=(const Queue &) = delete;

    static Queue *get_instance()
    {
        static Queue queue;
        return &queue;
    }

    void push_back(Organism *organism) { organisms_.push_back(organism); }

    void exec_all() { for (Organism *o : organisms_) o->exec(); }

    std::size_t size() const { return organisms_.size(); }

    Organism* next()
    {
        if (next_idx == organisms_.size())
            next_idx = 0;
        return organisms_[next_idx++];
    }

    std::size_t get_next_idx(){
        return next_idx;
    }

protected:
    Queue() = default;

    std::vector<Organism *> organisms_;
    std::size_t next_idx = 0;
};

#endif //CPPFUNGERA_QUEUE_H
