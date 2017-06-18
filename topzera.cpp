#include <algorithm>
#include <string>
#include <pwd.h>
#include <stdio.h>
#include <thread>
#include <mutex>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <signal.h>

#include "proc_common.h"
#include "topzera.h"

using namespace std;

mutex mtx;

int row, col, user_only = 0, ascending = 1;
int o_row, o_col, u_row, u_col;
int column_value_len;
char column[MAX_COLUMN_WIDTH + 1], column_value[MAX_COLUMN_WIDTH + 1];

void
cli(void)
{
    char prompt[] = "> ";
    char str[80];
    char *cmd, *cmd_pid, *signal_int;
    while (1)
    {
        mtx.lock();
        memset(str, 0, 80);
        move(LINES - 3, 0);
        clrtoeol();
        mvprintw(LINES - 3, 0, "available commands: q --> Quit | kill PID SIGNAL --> Send SIGNAL to a process (PID)");
        mvprintw(LINES - 2, 0, "                    u --> Toggle list user processes only | o -> toggle ascending/descending sorting by pid");
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
        else if (strcmp(str, "u") == 0)
            user_only = !user_only;
        else if (strcmp(str, "o") == 0)
            ascending = !ascending;
        else
        {
            cmd = strtok(str, " ");
            cmd_pid = strtok(NULL, " ");
            signal_int = strtok(NULL, " ");

            if (strcmp(cmd, "kill") == 0)
            {
                 int ret;
                 ret = kill(( pid_t ) atoi(cmd_pid), atoi(signal_int));
            }
        }

        mtx.lock();
        clrtoeol();
        mtx.unlock();
    }
    SAFE_FREE(cmd); SAFE_FREE(cmd_pid); SAFE_FREE(signal_int);
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
print_p(proc_info p)
{
    passwd *p_passwd = getpwuid(p.uid);
    printw("|");
    print_value(CW_PID, p.pid, CP_YELLOW);
    printw("|");
    print_value(CW_USER, p_passwd->pw_name, CP_BLUE);
    printw("|");
    print_value(CW_PROCNAME, p.comm, CP_MAGENTA);
    printw("|");
    print_value(CW_STATE, p.status, CP_CYAN);
    printw("|\n");
}

bool compare_process_pid(proc_info p1, proc_info p2) {
    if (ascending)
        return (p1.pid < p2.pid);
    return (p1.pid > p2.pid);
}

void
show_process(void)
{
    size_t i = 0, j = 0;
    vector<proc_info> process_list;
    while (1)
    {
        mtx.lock();
        getmaxyx(stdscr, row, col); /* get the number of rows and columns */
        getyx(stdscr, o_row, o_col);
        move(0, 0);

        process_list = get_process_list(); // Defined at proc_common
        sort(process_list.begin(), process_list.end(), compare_process_pid);

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

        i = 0, j = 0;
        for (; i < process_list.size() && j < row - 5; i++)
        {
            if (user_only && process_list[i].uid != getuid())
                continue;
            print_p(process_list[i]);

            j++;
        }

        while (j < row - 5) {
            clrtoeol();
            printw("\n");
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
