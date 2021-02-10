#include <iostream>
#include "App.h"

int main()
{
    // Initialize the memory
    Memory *memory = Memory::get_instance();
    memory->init(5000, 5000);
    std::size_t nlines, ncols;
    nlines = memory->load_genome("initial.gen", 2500, 2500, ncols);

    // Initialize the ancestor

    // Initialize the TUI
    App *app = App::get_instance();
    app->init(2500, 2500);
    app->run();

    return 0;
}
