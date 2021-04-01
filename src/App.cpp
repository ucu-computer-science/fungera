#include "App.h"
#include "Organism.h"
#include "Queue.h"
#include <algorithm>
#include <iostream>

App::App()
{
    Memory *memory = Memory::get_instance();
    memory->init(5000, 5000);
    begin_i_ = 2500;
    begin_j_ = 2500;
    auto size = memory->load_genome("../initial.gen", {begin_i_, begin_j_});
    Organism organism(size, {begin_i_, begin_j_});

    initscr();
    noecho();
    timeout(0);

    start_color();

    init_pair(1, COLOR_BLACK, COLOR_WHITE);

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
                case 'x':
                case 'X':
                    Queue::get_instance()->exec_all();
                default:
                    break;
            }
        draw_mem();
        refresh();
    }
}

void App::run()
{

}

void App::draw_mem()
{
    // This is horrible
    std::vector<std::array<std::size_t, 2>> ips;
    const auto organisms = Queue::get_instance()->get_organisms();
    ips.reserve(organisms.size());
    for (const auto &o : organisms)
        ips.emplace_back(o->get_ip());

    std::size_t i = begin_i_;
    for (int y = 0; y < max_y_; ++y) {
        std::size_t j = begin_j_;
        int x = 0;
        move(y, x);
        for ( ; x < max_x_; ++x) {
            const std::array<std::size_t, 2> coord{i+y, j+x};
            if (std::find(ips.begin(), ips.end(), coord) != ips.end())
                attron(COLOR_PAIR(1));
            addch((*Memory::get_instance())(i+y, j+x));
            if (std::find(ips.begin(), ips.end(), coord) != ips.end())
                attroff(COLOR_PAIR(1));
        }
    }
}
