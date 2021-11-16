#ifndef CPPFUNGERA_CELL_H
#define CPPFUNGERA_CELL_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/array.hpp>

// Encapsulates the information about the contents and freedom of the
// memory cell (in the Python version these two were separate).
struct Cell {
    char contents_ = '.';
    bool is_free_ = true;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & contents_;
        ar & is_free_;
    }
};

#endif //CPPFUNGERA_CELL_H
