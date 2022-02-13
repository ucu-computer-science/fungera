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

    size_t rows() const;
    size_t cols() const;

    char &instAt(size_t row, size_t col);
    char instAt(size_t row, size_t col) const;

    void setInstAt(Point pnt, char new_inst);
    void setInstAt(size_t row, size_t col, char new_inst);

    Point loadGenome(const std::string &fileName, Point topLeftPos);

    bool isAreaFree(Point topLeftPos, Point size);

    void allocArea(Point topLeftPos, Point size);

    void freeArea(Point topLeftPos, Point size);

    void irradiate();

    bool isTimeToKill(double memoryFullRatio = 0.01); // Аргумент по замовчуванню -- тимчасове рішення,
                        // вирішувати, чи вбивати -- не робота пам'яті, вона має лише повідомити частку
                        // зайнятих.

    void clear();

    Cell &operator()(size_t row, size_t col);
    Cell operator()(size_t row, size_t col) const;

protected:
    Memory(size_t r, size_t c): // Interface for future extension
        m_rows(r), m_cols{c}, m_cells{ r*c }
    {}

    Memory(): Memory(5000, 5000) {}

private:
    void setAreaFreedom(Point topLeftPos, Point size, bool isFree);

    size_t m_rows = 0;
    size_t m_cols = 0;
    std::vector<Cell> m_cells;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int ) // version
    {
        ar &m_rows;
        ar &m_cols;
        for(size_t i = 0; i < m_rows * m_cols; i++){
            ar &m_cells[i];
        }
    }
};

#endif // MEMORY_H
