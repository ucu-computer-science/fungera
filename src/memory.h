#ifndef MEMORY_H
#define MEMORY_H

#include "cell.h"
#include "point.h"

#include <string>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/array.hpp>

class Memory
{
public:
    Memory(Memory &) = delete;
    Memory *operator=(const Memory &) = delete;

    static Memory *getInstance();

    int rows() const;
    int cols() const;

    char &instAt(int row, int col);
    char instAt(int row, int col) const;

    void setInstAt(Point pnt, char new_inst);
    void setInstAt(int row, int col, char new_inst);

    Point loadGenome(const std::string &fileName, Point topLeftPos);

    bool isAreaFree(Point topLeftPos, Point size);

    void allocArea(Point topLeftPos, Point size);

    void freeArea(Point topLeftPos, Point size);

    void irradiate();

    bool isTimeToKill();

    void clear();

    Cell &operator()(int row, int col);
    Cell operator()(int row, int col) const;

protected:
    Memory();

private:
    void setAreaFreedom(Point topLeftPos, Point size, bool isFree);

    int _rows = 5000;
    int _cols = 5000;
    Cell *_cells = new Cell[_rows*_cols];

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & _rows;
        ar & _cols;
        for(int i = 0; i < _rows*_cols; i++){
            ar & _cells[i];
        }
    }
};

#endif // MEMORY_H
