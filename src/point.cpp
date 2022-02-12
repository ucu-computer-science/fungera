// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "point.h"

Point::Point(int _x, int _y) : x(_x), y(_y) {}

QString Point::qstr()
{
    return QString("{%1, %2}").arg(x).arg(y);
}

Point &Point::operator+=(Point rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

Point &Point::operator-=(Point rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

Point &Point::operator++()
{
    ++x;
    ++y;
    return *this;
}

Point &Point::operator--()
{
    --x;
    --y;
    return *this;
}

Point operator*(int coef, Point rhs)
{
    rhs.x *= coef;
    rhs.y *= coef;
    return rhs;
}

Point operator*(Point lhs, int coef)
{
    lhs.x *= coef;
    lhs.y *= coef;
    return lhs;
}

Point operator+(Point lhs, Point rhs)
{
    return lhs += rhs;
}

Point operator-(Point lhs, Point rhs)
{
    return lhs -= rhs;
}

bool operator==(Point lhs, Point rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator<(Point lhs, Point rhs)
{
    if (lhs.x == rhs.x)
        return lhs.y < rhs.y;
    return lhs.x < rhs.x;
}

bool operator!=(Point lhs, Point rhs)
{
    return !(lhs == rhs);
}

std::ostream &operator<<(std::ostream &os, Point p)
{
    return os << '{' << p.x << ", " << p.y << '}';
}
