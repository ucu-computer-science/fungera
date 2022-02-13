// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "memory.h"

#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <stdexcept>
#include "organismqueue.h"

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


Memory *Memory::getInstance()
{
    static Memory m;
    return &m;
}

size_t Memory::rows() const { return m_rows; }

size_t Memory::cols() const { return m_cols; }

char &Memory::instAt(size_t row, size_t col) {
    if (row < 0 || row >= m_rows || col < 0 || col >= m_cols) {
        if (OrganismQueue::getInstance()->getLogLevel() == "debug")
            std::cerr << "Accessing row/col out of memory map" << std::endl;
        throw std::range_error{"Accessing row/col out of memory map"};
    }
    return (*this)(row, col).inst;
}

char Memory::instAt(size_t row, size_t col) const {
    if ( row >= m_rows || col >= m_cols) {
        if (OrganismQueue::getInstance()->getLogLevel() == "debug")
            std::cerr << "Accessing row/col out of memory map" << std::endl;
        throw std::range_error{"Accessing row/col out of memory map"};
    }
    return (*this)(row, col).inst;
}

Point Memory::loadGenome(const std::string &fileName, Point topLeftPos)
{
    std::ifstream ifs(fileName);
    std::string line;
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

    if (lastRow >= rows() || lastCol >= cols())
    {
        if (OrganismQueue::getInstance()->getLogLevel() == "debug")
            std::cerr << "Memory area out of memory map bounds" << std::endl;
        throw "Memory area out of memory map bounds";
    }

    for (int row = topLeftPos.x; row < lastRow; ++row)
        for (int col = topLeftPos.y; col < lastCol; ++col)
            if (!(*this)(row, col).isFree)
                return false;
    return true;
}

void Memory::allocArea(Point topLeftPos, Point size) { setAreaFreedom(topLeftPos, size, false); }

void Memory::freeArea(Point topLeftPos, Point size) { setAreaFreedom(topLeftPos, size, true); }

Cell &Memory::operator()(size_t row, size_t col) { return m_cells[row * m_cols + col]; }

Cell Memory::operator()(size_t row, size_t col) const { return m_cells[row * m_cols + col]; }

using std::vector;
void Memory::irradiate()
{
    static std::mt19937 gen(13 /* set seed here */);
    std::uniform_int_distribution<> distr(0, m_rows -1);

    int randRow = distr(gen);
    // Assuming that the number of cols is the same as the number of rows
    int randCol = distr(gen);
    // TODO: make possiblity to chooce possible instructions for mutations
    vector<char> instructions{ '.', ':', 'a', 'b', 'c', 'd', 'x', 'y', '^', 'v', '<', '>', '&',
                               '?', '1', '0', '-', '+', '~', 'L', 'W', '@', '$', 'S', 'P', 'R', 'U', 'J' };
    (*this).instAt(randRow, randCol) = *select_randomly(instructions.begin(), instructions.end(), gen);
}

bool Memory::isTimeToKill(double memoryFullRatio)
{
    auto free_cells = std::count_if(m_cells.begin(), m_cells.end(), [](const Cell &cell){ return cell.isFree; });
    auto nonfree_cells = m_cells.size() - free_cells;
    double ratio = static_cast<double>(nonfree_cells)/free_cells; // TODO: Тут вартує не заняті/вільні, а заняті/всього.
    std::cout << "RATIO: " << ratio << std::endl;
    return ratio > memoryFullRatio;
}

void Memory::setAreaFreedom(Point topLeftPos, Point size, bool isFree)
{
    auto lastRow = std::min<size_t>(topLeftPos.x + size.x, rows());
    auto lastCol = std::min<size_t>(topLeftPos.y + size.y, cols());
    for (size_t row = topLeftPos.x; row < lastRow; ++row)
        for (size_t col = topLeftPos.y; col < lastCol; ++col)
            (*this)(row, col).isFree = isFree;
}

void Memory::setInstAt(size_t row, size_t col, char new_inst)
{
    this->m_cells[row* m_rows +col].inst = new_inst;
}
void Memory::setInstAt(Point pnt, char new_inst)
{
    this->setInstAt(pnt.x, pnt.y, new_inst);
}

void Memory::clear() {
    for (int i = 0; i < m_rows * m_cols; i++) {
        m_cells[i].inst = '.';
        m_cells[i].isFree = true;
    }
}
