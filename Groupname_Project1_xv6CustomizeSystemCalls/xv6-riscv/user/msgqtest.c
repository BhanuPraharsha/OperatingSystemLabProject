#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  char buf[64];
  char sendbuf[64];
  int pid;
  int status; // Used to store the result of our system calls

  printf("\n--- Message Queue Test ---\n");

  pid = fork();
  
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid > 0){
    // ================= PARENT PROCESS =================
    
    // 1. Prepare the message
    memset(sendbuf, 0, sizeof(sendbuf));
    strcpy(sendbuf, "Hello from the Parent!");

    // 2. Print to console BEFORE sending to avoid overlapping text
    printf("parent: sending message -> %s\n", sendbuf);

    // 3. ACTUALLY SEND THE MESSAGE
    // We execute the msgq_send function and save its return value into 'status'
    status = msgq_send(0, sendbuf);
    
    // 4. Check if it failed
    if(status < 0) {
        printf("parent: msgq_send failed!\n");
    }

    // Wait for the child to finish before the parent exits
    wait(0); 

  } else {
    // ================= CHILD PROCESS =================
    
    // 1. We want to read from Queue 0, but it might be empty.
    // Try to receive. If the queue is empty, msgq_recv returns -1.
    status = msgq_recv(0, buf);
    
    // 2. Poll (wait) until a message arrives
    while(status < 0) {
        pause(1); // Sleep for 1 tick (you renamed sleep to pause in your OS!)
        
        // Try again
        status = msgq_recv(0, buf);
    }
    
    // 3. We successfully received the message! Print it out.
    printf("child: received message -> %s\n", buf);
    
    exit(0);
  }

  exit(0);
}
