#ifndef CPPFUNGERA_APP_H
#define CPPFUNGERA_APP_H

#include "Memory.h"
#include "curses.h"

class App {
public:
    App(App &other) = delete;

    void operator=(const App &) = delete;

    static App *get_instance()
    {
        static App app;
        return &app;
    }

    void init(std::size_t begin_i, std::size_t begin_j)
    {
        begin_i_ = begin_i;
        begin_j_ = begin_j;
    }

    void run();

protected:
    App();

    void draw_mem();

    int max_y_;
    int max_x_;
    std::size_t begin_i_;
    std::size_t begin_j_;
};

#endif //CPPFUNGERA_APP_H
