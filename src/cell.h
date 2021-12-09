#ifndef CELL_H
#define CELL_H

#include <QtWidgets>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/array.hpp>

struct Cell
{
    char inst = '.';
    bool isFree = true;
    // TODO: Alias this enum by a custom one
    QColor bgColor = Qt::white;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & inst;
        ar & isFree;
    }
};

#endif // CELL_H
