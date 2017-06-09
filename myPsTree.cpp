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

#ifdef __APPLE__
#include <assert.h>
#include <errno.h>
#include <sys/sysctl.h>
#include <sys/param.h>

#define PID_MAX 99999 // from XNU - Mac OS X kernel http://opensource.apple.com/
#endif

bool ** children, * child_alloc;
char ** proc_comm;

#ifdef __APPLE__
typedef struct kinfo_proc kinfo_proc;

int GetBSDProcessList(kinfo_proc **procList, size_t *procCount)
{
    int                 err;
    kinfo_proc *        result;
    bool                done;
    static const int    name[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };
    size_t              length;

    assert( procList != NULL);
    assert(*procList == NULL);
    assert(procCount != NULL);

    *procCount = 0;

    result = NULL;
    done = false;
    do {
        assert(result == NULL);

        length = 0;
        err = sysctl( (int *) name, (sizeof(name) / sizeof(*name)) - 1,
                      NULL, &length,
                      NULL, 0);
        if (err == -1) {
            err = errno;
        }

        if (err == 0) {
            result = (kinfo_proc *) malloc(length);
            if (result == NULL) {
                err = ENOMEM;
            }
        }

        if (err == 0) {
            err = sysctl( (int *) name, (sizeof(name) / sizeof(*name)) - 1,
                          result, &length,
                          NULL, 0);
            if (err == -1) {
                err = errno;
            }
            if (err == 0) {
                done = true;
            } else if (err == ENOMEM) {
                assert(result != NULL);
                free(result);
                result = NULL;
                err = 0;
            }
        }
    } while (err == 0 && ! done);

    if (err != 0 && result != NULL) {
        free(result);
        result = NULL;
    }
    *procList = result;
    if (err == 0) {
        *procCount = length / sizeof(kinfo_proc);
    }

    assert( (err == 0) == (*procList != NULL) );

    return err;
}
#endif

int
get_pid_max()
{
#ifdef __APPLE__
    return PID_MAX;
#endif

    int pid_max, n;

    FILE *pid_max_fd = fopen("/proc/sys/kernel/pid_max", "r");
    if (!pid_max_fd) return -1;

    n = fscanf(pid_max_fd, "%d", &pid_max);
    fclose(pid_max_fd);

    if (n != 1) return -1;

    return pid_max;
}

void
print_children(int proc_num, int level, int pid_max)
{
    for (size_t i = 0; i < pid_max; i++)
    {
        if (child_alloc[proc_num] && children[proc_num][i])
        {
            for (int t = 0; t < level; t++) printf("\t"); // level-based indentation

            printf("%ss (\e[33;1m%lu\33[0m)\n", proc_comm[i], i);
            print_children(i, level + 1, pid_max);
        }
    }
}

int
main()
{
    clock_t begin = clock();

    int pid_max = get_pid_max();

    if (pid_max == -1)
    {
        printf("Error reading '/proc/sys/kernel/pid_max' file. Exiting...\n");

        return 1;
    }

    children = (bool **) malloc(sizeof(bool *) * pid_max + 1);
    child_alloc = (bool *) calloc(sizeof(bool), pid_max + 1);

    proc_comm = (char **) malloc(sizeof(char *) * pid_max + 1);

    int proc, curr_proc_pid, curr_proc_ppid;
    char curr_proc_comm[NAME_MAX + 2], curr_proc_state;
    
#ifndef __APPLE__
    DIR *dir;
    struct dirent *lsdir;
    dir = opendir("/proc");
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

            if (!child_alloc[curr_proc_ppid])
            {
                children[curr_proc_ppid] = (bool *) calloc(sizeof(bool), pid_max + 1);
                child_alloc[curr_proc_ppid] = true;
            }

            children[curr_proc_ppid][curr_proc_pid] = true;

            proc_comm[curr_proc_pid] = (char *) calloc(sizeof(char), strlen(&curr_proc_comm[1]) - 1 + 1);
            memcpy(proc_comm[curr_proc_pid], &curr_proc_comm[1], strlen(&curr_proc_comm[1]) - 1);
        }
    }
    closedir(dir);
#else
    kinfo_proc *p_process =NULL;
    size_t n_process = 0;
    int err = GetBSDProcessList(&p_process, &n_process);
    for (size_t i = 0; i < n_process; ++i) {
        curr_proc_pid = (int) p_process[i].kp_proc.p_pid;
        curr_proc_ppid = (int) p_process[i].kp_eproc.e_ppid;

        if (!child_alloc[curr_proc_ppid])
        {
            children[curr_proc_ppid] = (bool *) calloc(sizeof(bool), pid_max + 1);
            child_alloc[curr_proc_ppid] = true;
        }

        children[curr_proc_ppid][curr_proc_pid] = curr_proc_ppid != curr_proc_pid;

        proc_comm[curr_proc_pid] = (char *) calloc(sizeof(char), strlen(p_process[i].kp_proc.p_comm));
        memcpy(proc_comm[curr_proc_pid], p_process[i].kp_proc.p_comm, strlen(p_process[i].kp_proc.p_comm));
    }
#endif

    print_children(0, 0, pid_max);

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("%f secs\n", time_spent);

    return 0;

}
