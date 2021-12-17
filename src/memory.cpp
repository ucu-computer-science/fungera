#include "memory.h"

#include <fstream>
#include <iostream>

Memory::Memory() {}

Memory *Memory::getInstance()
{
    static Memory m;
    return &m;
}

int Memory::rows() const
{
    return _rows;
}

int Memory::cols() const
{
    return _cols;
}

char &Memory::instAt(int row, int col)
{
    return (*this)(row, col).inst;
}

char Memory::instAt(int row, int col) const
{
    return (*this)(row, col).inst;
}

bool Memory::isAreaFree(Point topLeftPos, Point size)
{
    int lastRow = topLeftPos.x + size.x;
    int lastCol = topLeftPos.y + size.y;
    for (int row = topLeftPos.x; row < lastRow; ++row)
        for (int col = topLeftPos.y; col < lastCol; ++col)
            if (!(*this)(row, col).isFree)
                return false;
    return true;
}

void  Memory::allocArea(Point topLeftPos, Point size)
{
    setAreaFreedom(topLeftPos, size, false);
}

Cell &Memory::operator()(int row, int col)
{
    return _cells[row*_cols+col];
}

Cell Memory::operator()(int row, int col) const
{
    return _cells[row*_cols+col];
}

using std::string;
Point Memory::loadGenome(const string &fileName, Point topLeftPos)
{
    std::ifstream ifs(fileName);
    string line;
    int row = topLeftPos.x;
    int col = topLeftPos.y;
    while (std::getline(ifs, line)) {
        col = topLeftPos.y;
        for (char c : line) {
            (*this)(row, col).inst = c;
            (*this)(row, col).isFree = false;
            ++col;
        }
        ++row;
    }
    ifs.close();
    return { row-topLeftPos.x, col-topLeftPos.y };
}

void Memory::setAreaFreedom(Point topLeftPos, Point size, bool isFree)
{
    int lastRow = topLeftPos.x + size.x;
    int lastCol = topLeftPos.y + size.y;
    for (int row = topLeftPos.x; row < lastRow; ++row)
        for (int col = topLeftPos.y; col < lastCol; ++col)
            (*this)(row, col).isFree = isFree;
}

void Memory::setInstAt(int row, int col, char new_inst)
{
    this->_cells[row*_rows+col].inst = new_inst;
}
void Memory::setInstAt(Point pnt, char new_inst)
{
    this->setInstAt(pnt.x, pnt.y, new_inst);
}
