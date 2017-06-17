#include "proc_common.h"

#ifdef __APPLE__

std::vector<proc_info>
get_bsd_process_list()
{
    int                 err;
    kinfo_proc *        result;
    bool                done;
    static const int    name[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };
    size_t              length;

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

    std::vector<proc_info> v_process;

    if (err != 0 && result != NULL) {
        free(result);
        result = NULL;

        return v_process;
    }

    for (size_t i = 0; i < length / sizeof(kinfo_proc); ++i)
    {
        proc_info curr_proc;
        curr_proc.pid = (int)result[i].kp_proc.p_pid;
        curr_proc.ppid = (int)result[i].kp_eproc.e_ppid;
        curr_proc.status = result[i].kp_proc.p_stat;
        curr_proc.uid = (int)result[i].kp_eproc.e_ucred.cr_uid;

        curr_proc.comm = (char *)calloc(sizeof(char), strlen(result[i].kp_proc.p_comm));
        memcpy(curr_proc.comm, result[i].kp_proc.p_comm, strlen(result[i].kp_proc.p_comm));

        v_process.push_back(curr_proc);
    }

    free(result);

    return v_process;
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

std::vector<proc_info>
get_process_list()
{
#ifdef __APPLE__
    return get_bsd_process_list();
#endif
    int pid_max = get_pid_max();
    DIR *dir;
    struct dirent *lsdir;
    dir = opendir("/proc");
    char *proc_directory = (char *) calloc(PATH_MAX, sizeof(char));

    FILE *arq;

    std::vector<proc_info> v_process;

    int proc, curr_proc_pid, curr_proc_ppid;
    char curr_proc_status;

    char *curr_proc_comm = (char *) calloc(NAME_MAX + 2, sizeof(char));
    char *parse_status;
    proc_info *curr_proc;

    while ((lsdir = readdir(dir)) != NULL)
    {
        if (isdigit(lsdir->d_name[0])) // Add to directories list only its name starts with a digit
        {

            proc = atoi(lsdir->d_name);

            sprintf(proc_directory, "/proc/%s/stat", lsdir->d_name);

            arq = fopen(proc_directory, "r");

            fscanf(arq,"%d %s %c %d", &curr_proc_pid, curr_proc_comm, &curr_proc_status, &curr_proc_ppid);

            fclose(arq);

            curr_proc = (proc_info *) malloc(sizeof(proc_info));

            curr_proc->status = curr_proc_status;
            curr_proc->comm = (char *) calloc(sizeof(char), strlen(&curr_proc_comm[1]) - 1 + 1);
            memcpy(curr_proc->comm, &curr_proc_comm[1], strlen(&curr_proc_comm[1]) - 1);
            
            curr_proc->pid = curr_proc_pid;
            curr_proc->ppid = curr_proc_ppid;

            sprintf(proc_directory, "/proc/%s/status", lsdir->d_name);

            std::ifstream input(proc_directory);
            for( std::string line; getline( input, line ); )
            {
                parse_status = strtok((char *) line.c_str(), " \t");
                if (!strcmp(parse_status, "Uid:")) {
                    parse_status = strtok(NULL, " \t");
                    curr_proc->uid = atoi(parse_status);
                }
            }

            input.close();

            v_process.push_back(*curr_proc);
        }
    }

    closedir(dir);

    free(curr_proc_comm);
    free(proc_directory);
    free(curr_proc);

    return v_process;
}