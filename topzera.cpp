#include "proc_common.h"
#include <string>
#include <pwd.h>
#include <stdio.h>
#include <pthread.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>


using namespace std;

#define UPDATE_TIME 1

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int row, col;
int o_row, o_col, u_row, u_col;


void
cli(void)
{
    char prompt[] = "> ";
    char str[80];

    while (1)
    {
        pthread_mutex_lock(&mutex);
        memset(str, 0, 80);
        move(LINES - 1, 0);
        clrtoeol();
        printw("%s", prompt);
        pthread_mutex_unlock(&mutex);

        getstr(str);
        // scanf("%s\n", str);
        if (strcmp(str, "q") == 0)
        {
            endwin();
            exit(0);
        }

        pthread_mutex_lock(&mutex);
        move(LINES - 2, 0);
        clrtoeol();
        mvprintw(LINES - 2, 0, "You Entered: %s", str);
        pthread_mutex_unlock(&mutex);
    }
}

void
show_process(void)
{
    int i = 0;
    while (1)
    {
        pthread_mutex_lock(&mutex);
        getmaxyx(stdscr, row, col); /* get the number of rows and columns */
        getyx(stdscr, o_row, o_col);
        move(0, 0);

        vector<proc_info> process_list = get_process_list(); // Defined at proc_common

        printw("TOPZERA\n");
        printw("| PID | USER | PROCNAME | STATE\n");

        passwd *p_passwd;
        for (size_t i = 0; i < process_list.size() && i < row -4; i++)
        {
            p_passwd = getpwuid(process_list[i].uid);
            printw("| %d  | %s | %s | %c \n",
            process_list[i].pid, p_passwd->pw_name, process_list[i].comm, process_list[i].status);
        }

        move(o_row, o_col);
        refresh();
        pthread_mutex_unlock(&mutex);
        sleep(UPDATE_TIME);
    }
}

int
main()
{
    initscr();                  /* start the curses mode */
    getmaxyx(stdscr, row, col); /* get the number of rows and columns */
    pthread_t p1, p2;
    pthread_create(&p1, NULL, (void* (*)(void*))cli, NULL);
    pthread_create(&p2, NULL, (void* (*)(void*))show_process, NULL);
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    return 0;
}
