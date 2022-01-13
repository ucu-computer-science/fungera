#include "memory.h"

#include <fstream>
#include <iostream>
#include <random>
#include <vector>

namespace
{
// https://stackoverflow.com/questions/6942273/how-to-get-a-random-element-from-a-c-container
template <typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator &g)
{
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template <typename Iter>
Iter select_randomly(Iter start, Iter end)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}
} // namespace

Memory::Memory() {}

Memory *Memory::getInstance()
{
    static Memory m;
    return &m;
}

int Memory::rows() const { return _rows; }

int Memory::cols() const { return _cols; }

char &Memory::instAt(int row, int col) {
    if (row >= _rows || col >= _cols) {
        std::cerr << "Accessing row / col out of memory map" << std::endl;
        throw "Accessing row / col out of memory map";
    }
    return (*this)(row, col).inst;
}

char Memory::instAt(int row, int col) const {
    if (row >= _rows || col >= _cols) {
        std::cerr << "Accessing row/col out of memory map" << std::endl;
        throw "Accessing row / col out of memory map";
    }
    return (*this)(row, col).inst;
}

using std::string;
Point Memory::loadGenome(const string &fileName, Point topLeftPos)
{
    std::ifstream ifs(fileName);
    string line;
    int row = topLeftPos.x;
    int col = topLeftPos.y;
    while (std::getline(ifs, line))
    {
        col = topLeftPos.y;
        for (char c : line)
        {
            (*this)(row, col).inst = c;
            (*this)(row, col).isFree = false;
            ++col;
        }
        ++row;
    }
    ifs.close();
    return { row - topLeftPos.x, col - topLeftPos.y };
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

void Memory::allocArea(Point topLeftPos, Point size) { setAreaFreedom(topLeftPos, size, false); }

void Memory::freeArea(Point topLeftPos, Point size) { setAreaFreedom(topLeftPos, size, true); }

Cell &Memory::operator()(int row, int col) { return _cells[row * _cols + col]; }

Cell Memory::operator()(int row, int col) const { return _cells[row * _cols + col]; }

using std::vector;
void Memory::irradiate()
{
    static std::mt19937 gen(1 /* set seed here */);
    std::uniform_int_distribution<> distr(0, _rows-1);

    int randRow = distr(gen);
    // Assuming that the number of cols is the same as the number of rows
    int randCol = distr(gen);
    // TODO: make possiblity to chooce possible instructions for mutations
    vector<char> instructions{ '.', ':', 'a', 'b', 'c', 'd', 'x', 'y', '^', 'v', '<', '>', '&',
                               '?', '1', '0', '-', '+', '~', 'L', 'W', '@', '$', 'S', 'P', 'R', 'U', 'J' };
    (*this).instAt(randRow, randCol) = *select_randomly(instructions.begin(), instructions.end());
}

bool Memory::isTimeToKill()
{
    double ratio = (double) std::count_if(_cells, _cells+_rows*_cols, [](const Cell &cell){ return !cell.isFree; })
            / std::count_if(_cells, _cells+_rows*_cols, [](const Cell &cell){ return cell.isFree; });
    constexpr auto memoryFullRatio = 0.9;
    return ratio > memoryFullRatio;
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
