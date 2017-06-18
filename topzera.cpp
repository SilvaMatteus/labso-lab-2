#include <algorithm>
#include <string>
#include <pwd.h>
#include <stdio.h>
#include <thread>
#include <mutex>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "proc_common.h"
#include "topzera.h"

using namespace std;

mutex mtx;

int row, col, user_only;
int o_row, o_col, u_row, u_col;
int column_value_len;
char column[MAX_COLUMN_WIDTH + 1], column_value[MAX_COLUMN_WIDTH + 1];

void
cli(void)
{
    char prompt[] = "> ";
    char str[80];

    while (1)
    {
        mtx.lock();
        memset(str, 0, 80);
        move(LINES - 2, 0);
        clrtoeol();
        mvprintw(LINES - 2, 0, "available commands: (help goes here!)");
        move(LINES - 1, 0);
        clrtoeol();
        printw("%s", prompt);
        mtx.unlock();

        getnstr(str, 80);
        if (strcmp(str, "q") == 0)
        {
            endwin();
            exit(0);
        }

        mtx.lock();
        clrtoeol();
        mtx.unlock();
    }
}

void
print_header(int column_width, const string value, int color_pair)
{
    attron(COLOR_PAIR(color_pair));
    memset(column, '\0', MAX_COLUMN_WIDTH + 1);
    memset(column, ' ', column_width);
    memcpy(&column[(column_width - strlen(value.c_str())) / 2], value.c_str(), strlen(value.c_str()));
    printw("%s", column);
    attroff(COLOR_PAIR(color_pair));
}

void
print_column_value(int column_width, char *column_value, int color_pair)
{
    column_value_len = min(column_width, min(MAX_COLUMN_WIDTH, (int) strlen(column_value)));
    memset(column, '\0', MAX_COLUMN_WIDTH + 1);
    memset(column, ' ', column_width);
    memcpy(&column[(column_width - column_value_len) / 2], column_value, column_value_len);
    printw("%s", column);
    attroff(COLOR_PAIR(color_pair));
}

void
print_value(int column_width, int value, int color_pair)
{
    attron(COLOR_PAIR(color_pair));
    sprintf(column_value, "%d", value);
    print_column_value(column_width, column_value, color_pair);
}

void
print_value(int column_width, char *value, int color_pair)
{
    attron(COLOR_PAIR(color_pair));
    sprintf(column_value, "%s", value);
    print_column_value(column_width, column_value, color_pair);
}

void
print_value(int column_width, char value, int color_pair)
{
    attron(COLOR_PAIR(color_pair));
    sprintf(column_value, "%c", value);
    print_column_value(column_width, column_value, color_pair);
}

void
show_process(void)
{
    int i = 0;
    vector<proc_info> process_list;
    while (1)
    {
        mtx.lock();
        getmaxyx(stdscr, row, col); /* get the number of rows and columns */
        getyx(stdscr, o_row, o_col);
        move(0, 0);

        process_list = get_process_list(); // Defined at proc_common

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
        
        printw("|");
        print_header(CW_PID, "PID", CP_YELLOW);
        printw("|");
        print_header(CW_USER, "USER", CP_BLUE);
        printw("|");
        print_header(CW_PROCNAME, "PROCNAME", CP_MAGENTA);
        printw("|");
        print_header(CW_STATE, "STATE", CP_CYAN);
        printw("|\n");

        passwd *p_passwd;
        for (size_t i = 0, j = 0; i < process_list.size() && j < row -4; i++)
        {
            p_passwd = getpwuid(process_list[i].uid);
            printw("|");
            print_value(CW_PID, process_list[i].pid, CP_YELLOW);
            printw("|");
            print_value(CW_USER, p_passwd->pw_name, CP_BLUE);
            printw("|");
            print_value(CW_PROCNAME, process_list[i].comm, CP_MAGENTA);
            printw("|");
            print_value(CW_STATE, process_list[i].status, CP_CYAN);
            printw("|\n");

            j++;
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
