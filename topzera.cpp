#include "proc_common.h"

#define MAX_PRINT_LEN 20

int
main()
{
    std::vector<proc_info> process_list = get_process_list();

    printf("<< Topzera >>\n");
    printf("| PID | User | PROCNAME | Estado |\n");

    for (size_t i = 0; i < process_list.size() && i < MAX_PRINT_LEN; i++)
    {
        printf("%d | %s | %c \n", process_list[i].pid, process_list[i].comm, process_list[i].status);
    }
    return 0;
}
