#include "App.h"
#include "Organism.h"
#include "Queue.h"

App::App()
{
    Memory *memory = Memory::get_instance();
    memory->init(5000, 5000);
    begin_i_ = 2500;
    begin_j_ = 2500;
    auto size = memory->load_genome("initial.gen", {begin_i_, begin_j_});
    Organism organism(size, {begin_i_, begin_j_});

    initscr();

    noecho();
    cbreak();
    nodelay(stdscr, true);

    getmaxyx(stdscr, max_y_, max_x_);

    for (;;) {
        int c;
        while ((c = getch()) != ERR)
            switch (c) {
            case 'w':
            case 'W':
                up();
                break;
            case 'a':
            case 'A':
                left();
                break;
            case 's':
            case 'S':
                down();
                break;
            case 'd':
            case 'D':
                right();
                break;
            default:
                break;
            }
        Queue::get_instance()->exec_all();
        draw_mem();
    }
}

void App::run()
{

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
    refresh();
}
