// semtest.c - a simple program to test our semaphore system calls
// it creates a parent and child process and both try to print a message
// the semaphore makes sure only one prints at a time (mutual exclusion)

#include "kernel/types.h"
#include "user/user.h"

int main(void) {
  int pid;

  printf("semaphore test starting...\n");

  pid = fork();

  if (pid < 0) {
    // fork failed
    printf("fork failed!\n");
    exit(1);
  }

  if (pid == 0) {
    // this is the child process
    sem_wait(0);   // wait to get the semaphore
    printf("child: inside critical section\n");
    sem_post(0);   // release the semaphore
    exit(0);
  } else {
    // this is the parent process
    sem_wait(0);   // wait to get the semaphore
    printf("parent: inside critical section\n");
    sem_post(0);   // release the semaphore
    wait(0);       // wait for child to finish
  }

  printf("semaphore test done!\n");
  exit(0);
}
