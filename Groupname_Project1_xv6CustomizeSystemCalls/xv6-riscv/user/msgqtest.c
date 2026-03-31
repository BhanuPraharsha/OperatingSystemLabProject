#include "kernel/types.h"
#include "user/user.h"


int main(int argc, char *argv[])
{
  char buf[64];
  int pid;

  printf("msgq test: starting\n");

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid > 0){
    // parent sends a message on queue 0
    char *msg = "hello from parent!";
    // copy into a 64-byte buffer so we send exactly 64 bytes
    char sendbuf[64];
    memset(sendbuf, 0, sizeof(sendbuf));
    strcpy(sendbuf, msg);

    if(msgq_send(0, sendbuf) < 0){
      printf("msgq_send failed\n");
      exit(1);
    }
    printf("parent: sent message\n");
    wait(0);
  } else {
    // child receives from queue 0
    // small delay so parent sends first
    for(int i = 0; i < 100000; i++)
      ;

    if(msgq_recv(0, buf) < 0){
      printf("msgq_recv failed\n");
      exit(1);
    }
    printf("child: got message -> %s\n", buf);
  }

  exit(0);
}
