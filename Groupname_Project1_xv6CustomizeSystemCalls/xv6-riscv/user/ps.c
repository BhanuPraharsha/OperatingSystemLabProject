#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#include "kernel/pstat.h"


int main(int argc, char* argv[]){
    //array to hold the process info
    //NPROC: max number of procs, usually 64

    struct uproc ptable[NPROC];

    int active_procs = psinfo(ptable);

    if(active_procs < 0){
        printf("ps: failed to get information\n");
        exit(1);


    }
    char *state_names[] = {
        "UNUSED  ",
        "USED    ",
        "SLEEPING",
        "RUNNABLE",
        "RUNNING ",
        "ZOMBIE  "
        
    };

    printf("PID\tPPID\tSTATE\t\tCMD\n");

    printf("---\t----\t-----\t\t---\n");
    for(int i=0; i<active_procs; i++){
        char *state_str = "UNKNOWN";
        if (ptable[i].state >=0 && ptable[i].state<=5){
            state_str = state_names[ptable[i].state];

        }
        printf("%d\t%d\t%s\t%s\n",
            ptable[i].pid,
            ptable[i].ppid,
            state_str,
            ptable[i].name
        );
    }

    exit(0);
}
