/*
myps
A program to read the /proc directory and create a process tree.
*/

// testing
#include <dirent.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>

#include "proc_common.h"

bool ** children, * child_alloc;
char ** proc_comm;

void
print_children(int proc_num, int level, int pid_max)
{
    for (size_t i = 0; i < pid_max; i++)
    {
        if (child_alloc[proc_num] && children[proc_num][i])
        {
            for (int t = 0; t < level; t++) printf("\t"); // level-based indentation

            printf("\e[35;1m%s\33[0m (\e[33;1m%lu\33[0m)\n", proc_comm[i], i);
            print_children(i, level + 1, pid_max);
        }
    }
}

int
main()
{
    clock_t begin = clock();

    int pid_max = get_pid_max();

    children = (bool **) malloc(sizeof(bool *) * pid_max + 1);
    child_alloc = (bool *) calloc(sizeof(bool), pid_max + 1);

    proc_comm = (char **) malloc(sizeof(char *) * pid_max + 1);

    std::vector<proc_info> process_list = get_process_list();
    for (size_t i = 0; i < process_list.size(); i++) {
        if (!child_alloc[process_list[i].ppid])
        {
            children[process_list[i].ppid] = (bool *) calloc(sizeof(bool), pid_max + 1);
            child_alloc[process_list[i].ppid] = true;
        }

        children[process_list[i].ppid][process_list[i].pid] = process_list[i].ppid != process_list[i].pid;
        proc_comm[process_list[i].pid] = (char *) calloc(sizeof(char), strlen(process_list[i].comm) + 1);
        memcpy(proc_comm[process_list[i].pid], process_list[i].comm, strlen(process_list[i].comm));
    }

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    print_children(0, 0, pid_max);

    printf("%f secs\n", time_spent);

    return 0;

}
