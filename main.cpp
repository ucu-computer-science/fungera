#include <iostream>
#include "App.h"

int main()
{
    App *app = App::get_instance();
    app->run();

    return 0;
}
