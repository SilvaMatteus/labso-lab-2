#include "proc_common.h"
#include <string>
#include <pwd.h>
#include <stdio.h>
#include <thread>
#include <mutex>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace std;

#define UPDATE_TIME 1

#define CP_MAGENTA 1
#define CP_BLUE 2
#define CP_YELLOW 3
#define CP_CYAN 4

mutex mtx;

int row, col;
int o_row, o_col, u_row, u_col;

void
cli(void)
{
    char prompt[] = "> ";
    char str[80];

    while (1)
    {
        mtx.lock();
        memset(str, 0, 80);
        move(LINES - 1, 0);
        clrtoeol();
        printw("%s", prompt);
        mtx.unlock();

        getnstr(str, 80);
        // scanf("%s\n", str);
        if (strcmp(str, "q") == 0)
        {
            endwin();
            exit(0);
        }

        mtx.lock();
        move(LINES - 2, 0);
        clrtoeol();
        mvprintw(LINES - 2, 0, "You Entered: %s", str);
        mtx.unlock();
    }
}

void
show_process(void)
{
    int i = 0;
    while (1)
    {
        mtx.lock();
        getmaxyx(stdscr, row, col); /* get the number of rows and columns */
        getyx(stdscr, o_row, o_col);
        move(0, 0);

        vector<proc_info> process_list = get_process_list(); // Defined at proc_common

        attron(WA_BOLD);

        attron(COLOR_PAIR(CP_MAGENTA));
        printw("TOP");
        attroff(COLOR_PAIR(CP_MAGENTA));

        attron(COLOR_PAIR(CP_YELLOW));
        printw("ZE");
        attroff(COLOR_PAIR(CP_YELLOW));

        attron(COLOR_PAIR(CP_BLUE));
        printw("RA\n");
        attroff(COLOR_PAIR(CP_BLUE));
        
        printw("| ");
        attron(COLOR_PAIR(CP_YELLOW));
        printw("PID");
        attroff(COLOR_PAIR(CP_YELLOW));
        printw(" | ");
        attron(COLOR_PAIR(CP_BLUE));
        printw("USER");
        attroff(COLOR_PAIR(CP_BLUE));
        printw(" | ");
        attron(COLOR_PAIR(CP_MAGENTA));
        printw("PROCNAME");
        attroff(COLOR_PAIR(CP_MAGENTA));
        printw(" | ");
        attron(COLOR_PAIR(CP_CYAN));
        printw("STATE");
        attroff(COLOR_PAIR(CP_CYAN));
        printw("\n");

        passwd *p_passwd;
        for (size_t i = 0; i < process_list.size() && i < row -4; i++)
        {
            p_passwd = getpwuid(process_list[i].uid);
            printw("| ");
            attron(COLOR_PAIR(CP_YELLOW));
            printw("%d", process_list[i].pid);
            attroff(COLOR_PAIR(CP_YELLOW));
            printw(" | ");
            attron(COLOR_PAIR(CP_BLUE));
            printw("%s", p_passwd->pw_name);
            attroff(COLOR_PAIR(CP_BLUE));
            printw(" | ");
            attron(COLOR_PAIR(CP_MAGENTA));
            printw("%s", process_list[i].comm);
            attroff(COLOR_PAIR(CP_MAGENTA));
            printw(" | ");
            attron(COLOR_PAIR(CP_CYAN));
            printw("%c", process_list[i].status);
            attroff(COLOR_PAIR(CP_CYAN));
            printw("\n");
        }

        attroff(WA_BOLD);

        move(o_row, o_col);
        refresh();
        mtx.unlock();
        sleep(UPDATE_TIME);
    }
}

int
main()
{
    initscr();                  /* start the curses mode */
    
    // setup colors
    start_color();              /* Start color 			*/
    use_default_colors();
    
    init_pair(CP_MAGENTA, COLOR_MAGENTA, -1);
    init_pair(CP_BLUE, COLOR_BLUE, -1);
    init_pair(CP_YELLOW, COLOR_YELLOW, -1);
    init_pair(CP_CYAN, COLOR_CYAN, -1);

    getmaxyx(stdscr, row, col); /* get the number of rows and columns */

    thread p1(cli);
    thread p2(show_process);
    p1.join();
    p2.join();

    return 0;
}
