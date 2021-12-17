#ifndef MEMORY_H
#define MEMORY_H

#include "cell.h"
#include "point.h"

#include <string>

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

    Point loadGenome(const std::string &fileName, Point topLeftPos);

    bool isAreaFree(Point topLeftPos, Point size);

    void allocArea(Point topLeftPos, Point size);

    void irradiate();

    Cell &operator()(int row, int col);
    Cell operator()(int row, int col) const;

protected:
    Memory();

private:
    void setAreaFreedom(Point topLeftPos, Point size, bool isFree);

    int _rows = 5000;
    int _cols = 5000;
    Cell *_cells = new Cell[_rows*_cols];
};

#endif // MEMORY_H
