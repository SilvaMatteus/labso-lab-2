#ifndef PROC_COMMON_H
#define PROC_COMMON_H

#include <limits.h>
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pwd.h>
#include <vector>
#include <fstream>

#ifdef __APPLE__
#include <assert.h>
#include <errno.h>
#include <sys/sysctl.h>
#include <sys/param.h>

#define PID_MAX 99999 // from XNU - Mac OS X kernel http://opensource.apple.com/

typedef struct kinfo_proc kinfo_proc;
#endif

struct proc_info {
    int pid;
    int ppid;
    char* comm;
    char status;
    int uid;
};

std::vector<proc_info>
get_process_list();

int
get_pid_max();

#endif