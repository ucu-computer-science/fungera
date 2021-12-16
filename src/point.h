#ifndef POINT_H
#define POINT_H

#include <QtWidgets>

#include <ostream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

struct Point
{
    Point() = default;
    Point(int _x, int _y);

    QString qstr();

    int x;
    int y;

    Point &operator+=(Point rhs);

    Point &operator-=(Point rhs);

    Point &operator++();

    Point &operator--();

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & x;
        ar & y;
    }
};

Point operator*(int coef, Point rhs);
Point operator*(Point lhs, int coef);

Point operator+(Point lhs, Point rhs);

Point operator-(Point lhs, Point rhs);

bool operator==(Point lhs, Point rhs);

std::ostream &operator<<(std::ostream &os, Point p);

#endif // POINT_H
