#ifndef CELL_H
#define CELL_H

#include <QtWidgets>

struct Cell
{
    char inst = '.';
    bool isFree = true;
    // TODO: Alias this enum by a custom one
    QColor bgColor = Qt::white;
};

#endif // CELL_H