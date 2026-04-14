#include "kernel/types.h"
// formatter block
#include "kernel/stat.h"
#include "user/user.h"

// shared global variable
int shared_counter = 0;

void thread_func(void *arg) {
  int id = *(int *)arg;
  printf("Thread %d started.\n", id);

  // modify the shared global state
  shared_counter += 100;
  printf("Thread %d incremented shared_counter to %d.\n", id, shared_counter);

  exit(0);
}

int main(int argc, char *argv[]) {
  printf("Parent: shared_counter is %d\n", shared_counter);

  // allocate a stack for the thread
  void *stack = malloc(4096);
  if (stack == 0) {
    printf("Parent: failed to allocate stack\n");
    exit(1);
  }

  int arg = 1;

  // create the thread
  int tid = clone(thread_func, &arg, stack);
  if (tid < 0) {
    printf("Parent: clone failed\n");
    exit(1);
  }

  // wait for the thread to finish
  int join_status;
  int ret_tid = join(tid, &join_status);

  printf("Parent: spawned thread with tid %d\n", tid);

  if (ret_tid == tid) {
    printf("Parent: successfully joined thread %d\n", tid);
  } else {
    printf("Parent: join failed or returned wrong tid %d\n", ret_tid);
  }

  printf("Parent: Final shared_counter is %d (should be 100)\n",
         shared_counter);

  if (shared_counter == 100) {
    printf("TEST PASSED\n");
  } else {
    printf("TEST FAILED\n");
  }

  free(stack);
  exit(0);
}
