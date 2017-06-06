/*
myps
A program to read the /proc directory and create a process tree.
*/

// testing
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <vector>
#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/limits.h>

#define MAX_LINE 100

using namespace std;

int
get_pid_max()
{
    int pid_max, n;

    FILE *pid_max_fd = fopen("/proc/sys/kernel/pid_max", "r");
    if (!pid_max_fd) return -1;

    n = fscanf(pid_max_fd, "%d", &pid_max);
    fclose(pid_max_fd);

    if (n != 1) return -1;

    return pid_max;
}

int
main()
{
    int pid_max = get_pid_max();

    if (pid_max == -1)
    {
        printf("Error reading '/proc/sys/kernel/pid_max' file. Exiting...\n");
        return 1;
    }

    bool ** paternity = (bool **) malloc(sizeof(bool *) * pid_max + 1);
    for (size_t i = 0; i < pid_max; i++)
    {
        paternity[i] = (bool *) calloc(sizeof(bool), pid_max + 1);
    }

    bool ** children = (bool **) malloc(sizeof(bool *) * pid_max + 1);
    for (size_t i = 0; i < pid_max; i++)
    {
        children[i] = (bool *) calloc(sizeof(bool), pid_max + 1);
    }

    char ** proc_comm = (char **) malloc(sizeof(char *) * pid_max + 1);
    for (size_t i = 0; i < pid_max; i++)
    {
        proc_comm[i] = (char *) calloc(sizeof(char), NAME_MAX);
    }

    DIR *dir;
    struct dirent *lsdir;
    dir = opendir("/proc");

    int proc, curr_proc_pid, curr_proc_ppid;
    char curr_proc_comm[NAME_MAX + 2], curr_proc_state;
    char proc_directory[50];
    FILE *arq;


    while ((lsdir = readdir(dir)) != NULL)
    {
        if (isdigit(lsdir->d_name[0])) // Add to directories list only its name starts with a digit
        {
            proc = atoi(lsdir->d_name);

            sprintf(proc_directory, "/proc/%s/stat", lsdir->d_name);

            arq = fopen(proc_directory, "r");

            fscanf(arq,"%d %s %c %d", &curr_proc_pid, curr_proc_comm, &curr_proc_state, &curr_proc_ppid);

            fclose(arq);

            children[curr_proc_ppid][curr_proc_pid] = true;
            paternity[curr_proc_pid][curr_proc_ppid] = true;
            memcpy(proc_comm[curr_proc_pid], &curr_proc_comm[1], strlen(&curr_proc_comm[1]) - 1);
        }
    }
    closedir(dir);

    return 0;

}
