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

#define MAX_LINE 100

using namespace std;

int
get_pid_max()
{
    int pid_max_fd;
    char *line = (char *) calloc(sizeof(char), MAX_LINE);
    ssize_t n;

    pid_max_fd = open("/proc/sys/kernel/pid_max", O_RDONLY);
    if (pid_max_fd == -1) return -1;

    n = read(pid_max_fd, line, MAX_LINE);
    close(pid_max_fd);

    if (n == -1) return -1;

    int pid_max = atoi(line);
    free(line);

    return pid_max;
}

int
main()
{
    int pid_max = get_pid_max();

    bool ** paternity = (bool **) malloc(sizeof(bool *) * pid_max);
    for (size_t i; i < pid_max; i++)
    {
        paternity[i] = (bool *) calloc(sizeof(bool), pid_max);
    }

    // FILE *arq;
    // arq = fopen("test.txt", "r");
    // char* ch1;
    // char* ch2;
    // char* ch3;
    // ch1 = malloc(sizeof(char)*50);
    // ch2 = malloc(sizeof(char)*50);
    // ch3 = malloc(sizeof(char)*50);
    // fscanf(arq,"%s %s %s", ch1, ch2, ch3);
    // printf("ola\n");
    // printf("%s\n", ch2);
    // printf("ola\n");
    DIR *dir;
    struct dirent *lsdir;
    dir = opendir("/proc");
    vector<char*> directories;

    while ((lsdir = readdir(dir)) != NULL)
    {
        if (isdigit(lsdir->d_name[0])) // Add to directories list only its name starts with a digit
        {
            char* dir = (char*) calloc(strlen(lsdir->d_name) +1, 1);
            memcpy(dir, &lsdir->d_name, strlen(lsdir->d_name));
            directories.push_back(dir);
            printf("%s\n", dir);
        }
    }
    closedir(dir);

    for(vector<char*>::iterator it = directories.begin(); it != directories.end(); it++)
    {
        // printf("valor ==> %s\n", *it);
        char* proc_directory;
        proc_directory = (char*) calloc(sizeof(char), 50);
        memcpy(proc_directory, "/proc/", 6);
        strcat(proc_directory, *it);
        strcat(proc_directory, "/stat");
        printf("proc_directory ===> %s\n", proc_directory);
        FILE *arq;
        arq = fopen(proc_directory, "r");
        char* ch1;
        char* ch2;
        char* ch3;
        char* ch4;
        ch1 = (char*) calloc(sizeof(char), 50);
        ch2 = (char*) calloc(sizeof(char), 50);
        ch3 = (char*) calloc(sizeof(char), 50);
        ch4 = (char*) calloc(sizeof(char), 50);
        fscanf(arq,"%s %s %s %s", ch1, ch2, ch3, ch4);
        printf("%s(%s) parent(%s)\n", ch2, ch1, ch4);
        fclose(arq);
    }

    return 0;

}
