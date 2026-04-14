#include "kernel/types.h"
#include "user/user.h"

void handle_sigint(int signum){
    // If we see this, the delivery worked!
    printf(">>> CHILD: Caught signal %d! I refuse to die.\n", signum);

    if (sigreturn() < 0) {
        printf(">>> CHILD ERROR: sigreturn() failed!\n");
    }
}

int main() {
    
    int pid = fork();
    
    if(pid == 0){
signal(2, handle_sigint);
        
        while(1) {
            pause(10);
        }   
    }
    else {
        printf("Parent: Waiting for child to set up...\n");
        pause(20);
        
        printf("Parent: Sending SIGINT (2) to child...\n");
        kill(pid, 2);

        // Wait to give the child time to run the handler
        pause(20);

        // We MUST send signal 9, otherwise wait(0) will hang if the child survives!
        printf("Parent: Sending SIGKILL (9) to child...\n");
        kill(pid, 9);

        wait(0);
        printf("Parent: Child reaped. Exiting.\n");
    }
    exit(0);
}