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
using namespace std;

int main()
{
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
