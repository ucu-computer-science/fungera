#ifndef CPPFUNGERA_CELL_H
#define CPPFUNGERA_CELL_H

// Encapsulates the information about the contents and freedom of the
// memory cell (in the Python version these two were separate).
struct Cell {
    char contents_ = '.';
    bool is_free_ = true;
};

#endif //CPPFUNGERA_CELL_H
