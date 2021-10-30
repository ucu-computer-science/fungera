#include "Memory/Memory.h"
#include "Organism/Organism.h"
#include "Queue.h"
#include "curses.h"
#include <algorithm>

#define BOLD_PARENT 1
#define BOLD_CHILD 2
#define BOLD_IP 3
#define PARENT 4
#define CHILD 5
#define IP 6

void draw_mem(WINDOW *mem_win, int nlines, int ncols, int begin_i, int begin_j,
              const status_t &status);
bool find_point(const size_t_arr &begin, const size_t_arr &size, std::size_t x, std::size_t y)
{
    return x >= begin[0] && x < begin[0]+size[0]
           && y >= begin[1] && y < begin[0]+size[1];
}
void draw_status(WINDOW *win,
                 const std::string &sim_name,
                 unsigned cycle_no,
                 std::size_t begin_i, std::size_t begin_j,
                 const status_t &status);
void draw_ctrls(WINDOW *win);
void my_mvwaddstr(WINDOW *win, int y, int x, const std::string &str);

int main(int argc, char **argv)
{
    Memory *memory = Memory::get_instance();
    memory->init(5000, 5000);

    std::size_t begin_i = 2500;
    std::size_t begin_j = 2500;

    auto size = memory->load_genome(*(argv+1), {begin_i, begin_j});

    Organism org(size, {begin_i, begin_j});
    auto *cur_org = &org;

    initscr();
    noecho();
    timeout(0);

    // Initialize the colors
    start_color();

    init_pair(BOLD_PARENT, COLOR_BLACK, COLOR_YELLOW);
    init_pair(BOLD_IP, COLOR_WHITE, COLOR_RED);
    init_pair(BOLD_CHILD, COLOR_WHITE, 128);
    init_pair(PARENT, COLOR_WHITE, 126);
    init_pair(IP, COLOR_WHITE, 160);
    init_pair(CHILD, COLOR_WHITE, 128);

    // Memory
    constexpr int mem_nlines = 24, mem_ncols = 80;
    WINDOW *mem_win = newwin(mem_nlines, mem_ncols, 0, 0);
    refresh();
    box(mem_win, 0, 0);
    mvwprintw(mem_win, 0, 1, " Memory ");

    // Status
    constexpr int status_nlines = 24, status_ncols = 40;
    WINDOW *status_win = newwin(status_nlines, status_ncols, 0, mem_ncols);
    refresh();

    // Controls
    constexpr int ctrls_nlines = 5, ctrls_ncols = 120;
    WINDOW *ctrls_win = newwin(ctrls_nlines, ctrls_ncols, mem_nlines, 0);
    refresh();
    draw_ctrls(ctrls_win);

    bool is_running = false;
    unsigned cycle_no = 0;
    for (;;) {
        int c;
        while ((c = getch()) != ERR) {
            switch (c) {
            case 'w':
            case 'W':
                begin_i--;
                break;
            case 'a':
            case 'A':
                begin_j--;
                break;
            case 's':
            case 'S':
                begin_i++;
                break;
            case 'd':
            case 'D':
                begin_j++;
                break;
            case 'e':
            case 'E':
                if (!is_running) {
                    Queue::get_instance()->exec_all();
                    cycle_no++;
                }
                break;
            case 'n':
            case 'N':
                cur_org = Queue::get_instance()->next();
                break;
            case 'p':
            case 'P':
                is_running = !is_running;
                break;
            default:
                break;
            }
        }
        status_t status = cur_org->get_status();

        draw_mem(mem_win, mem_nlines, mem_ncols, begin_i, begin_j, status);

        draw_status(status_win, "Test", cycle_no, begin_i, begin_j, status);

        refresh();

        if (is_running) {
            Queue::get_instance()->exec_all();
            cycle_no++;
        }
    }

    return 0;
}

void draw_mem(WINDOW *mem_win, int nlines, int ncols, int begin_i, int begin_j,
              const status_t &status)
{
    for (int y = 1; y < nlines-1; ++y) {
        int x = 1;
        wmove(mem_win, y, x);
        for ( ; x < ncols-1; ++x) {
            std::size_t i = begin_i + y - 1;
            std::size_t j = begin_j + x - 1;

            if (i == status.ip[0] && j == status.ip[1])
                wattron(mem_win, COLOR_PAIR(BOLD_IP));
            else if (find_point(status.begin, status.size, i, j))
                wattron(mem_win, COLOR_PAIR(BOLD_PARENT));

            waddch(mem_win, (*Memory::get_instance())(i, j));

            if (std::array<std::size_t, 2> {i, j} == status.ip)
                wattroff(mem_win, COLOR_PAIR(BOLD_IP));
            else if (find_point(status.begin, status.size, i, j))
                wattroff(mem_win, COLOR_PAIR(BOLD_PARENT));
        }
    }
    wnoutrefresh(mem_win);
}

void draw_status(WINDOW *win,
                 const std::string &sim_name,
                 unsigned cycle_no,
                 std::size_t begin_i, std::size_t begin_j,
                 const status_t &status)
{
    werase(win);

    box(win, 0, 0);
    mvwprintw(win, 0, 1, " Status ");

    mvwprintw(win, 1, 2, sim_name.c_str());

    mvwprintw(win, 2, 2, "Cycle no : %u", cycle_no);

    mvwprintw(win, 3, 2, "Begin    : {%4lu, %4lu}", begin_i, begin_j);

    mvwprintw(win, 4, 2, "Orgs     : %lu", Queue::get_instance()->size());

    mvwprintw(win, 5, 2, "Extns    : ");

    mvwprintw(win, 6, 2, "ID       : %u", status.id);

    mvwprintw(win, 7, 2, "Errors   : ");

    mvwprintw(win, 8, 2, "IP       : {%4lu, %4lu}", status.ip[0], status.ip[1]);

    mvwprintw(win, 9, 2, "Velocity : {%4d, %4d}", status.velocity[0], status.velocity[1]);

    int i = 10;
    for (const auto &pair : status.regs)
        mvwprintw(win, i++, 2, "R%c       : {%4lu, %4lu}", pair.first, pair.second[0], pair.second[1]);

    // The rendering of the stack is very expensive.
    // TODO: implement it through a vector
    std::vector<size_t_arr> v;

    auto stack = status.stack;
    while (!stack.empty()) {
        v.push_back(stack.top());
        stack.pop();
    }

    for (auto i = v.size()-1; i != -1; i--)
        mvwprintw(win, i + 14, 2, "stack[%d] : {%4lu, %4lu}", i, v[i][0], v[i][1]);

    wnoutrefresh(win);
}

void draw_ctrls(WINDOW *win)
{
    box(win, 0, 0);
    mvwprintw(win, 0, 1, " Controls ");

    my_mvwaddstr(win, 1, 2, "\016W\017 Up\t\016A\017 Left\t\016S\017 "
        "Down\t\016D\017 Right\t\016P\017 Pause\t\016E\017 Exec all\t\016^C\017 Exit");

    wnoutrefresh(win);
}

void my_mvwaddstr(WINDOW *win, int y, int x, const std::string &str)
{
    wmove(win, y, x);
    for (char c : str) {
        switch (c) {
        case '\016':
            wattron(win, A_STANDOUT);
            break;
        case '\017':
            wattroff(win, A_STANDOUT);
            break;
        case '\t':
            x = getcurx(win);
            wmove(win, y, (x+4) & -4);
        default:
            waddch(win, c);
            break;
        }
    }
}
