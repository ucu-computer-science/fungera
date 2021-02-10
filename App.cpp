#include "App.h"

void App::run()
{
    draw_mem();
    refresh();

    getch();
    endwin();
}

void App::draw_mem()
{
    std::size_t i = begin_i_;
    for (int y = 0; y < max_y_; ++y) {
        std::size_t j = begin_j_;
        int x = 0;
        move(y, x);
        for ( ; x < max_x_; ++x)
            addch((*Memory::get_instance())(i+y, j+x));
    }
}
