#include "proc_common.h"
#include <string>
#include <pwd.h>

using namespace std;

#define MAX_PRINT_LEN 20

int
main()
{
    vector<proc_info> process_list = get_process_list(); // Defined at proc_common

    printf("\e[35;1mTOP\e[33;1mZE\e[34;1mRA\33[0m\n");
    printf("| \e[33;1mPID\33[0m | \e[34;1mUSER\33[0m | \e[35;1mPROCNAME\33[0m | \e[36;1mSTATE\33[0m |\n");

    passwd *p_passwd;
    for (size_t i = 0; i < process_list.size() && i < MAX_PRINT_LEN; i++)
    {
        p_passwd = getpwuid(process_list[i].uid);
        printf("| \e[33;1m%d\33[0m   | \e[34;1m%s\33[0m | \e[35;1m%s\33[0m | \e[36;1m%c\33[0m \n",
        process_list[i].pid, p_passwd->pw_name, process_list[i].comm, process_list[i].status);
    }

    return 0;
}
