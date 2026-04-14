#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// ============================================================
// Ayush's thread_create / thread_join API
//
// These wrapper functions provide a cleaner interface on top
// of the raw clone() and join() system calls implemented by
// Bhanu.  thread_create() allocates a stack internally and
// invokes clone(); thread_join() waits for the thread and
// frees its stack.
// ============================================================

#define STACK_SIZE 4096

// Simple bookkeeping so thread_join can free the right stack.
// We support up to 8 concurrent threads in this test.
#define MAX_THREADS 8

struct thread_info {
  int    tid;
  void  *stack;
};

static struct thread_info threads[MAX_THREADS];
static int nthreads = 0;

// thread_create  — Ayush's API
// Creates a new thread running `fn(arg)`.
// Returns the thread-id (pid) on success, -1 on failure.
int
thread_create(void (*fn)(void *), void *arg)
{
  // allocate a page-aligned stack for the child thread
  void *stack = malloc(STACK_SIZE);
  if (stack == 0)
    return -1;

  int tid = clone(fn, arg, stack);
  if (tid < 0) {
    free(stack);
    return -1;
  }

  // remember the mapping so thread_join can free() the stack
  if (nthreads < MAX_THREADS) {
    threads[nthreads].tid   = tid;
    threads[nthreads].stack = stack;
    nthreads++;
  }

  return tid;
}

// thread_join  — Ayush's API
// Waits for the thread identified by `tid` to exit.
// Returns the tid on success, -1 on failure.
int
thread_join(int tid)
{
  int status;
  int ret = join(tid, &status);

  // free the stack we allocated in thread_create
  for (int i = 0; i < nthreads; i++) {
    if (threads[i].tid == tid) {
      free(threads[i].stack);
      // compact the array
      threads[i] = threads[nthreads - 1];
      nthreads--;
      break;
    }
  }

  return ret;
}

// ============================================================
// Test cases
// ============================================================

// ---- shared mutable state (proves address-space sharing) ----
int shared_var = 0;

// --- Test 1: basic thread create + join -----------------------
void
worker_basic(void *arg)
{
  int id = *(int *)arg;
  printf("  [thread %d] running, incrementing shared_var\n", id);
  shared_var += 10;
  exit(0);
}

void
test_basic(void)
{
  printf("\n=== Test 1: basic thread_create / thread_join ===\n");

  shared_var = 0;
  int arg = 1;

  int tid = thread_create(worker_basic, &arg);
  if (tid < 0) {
    printf("FAIL: thread_create returned %d\n", tid);
    exit(1);
  }
  printf("Parent: created thread tid=%d\n", tid);

  int ret = thread_join(tid);
  if (ret != tid) {
    printf("FAIL: thread_join returned %d, expected %d\n", ret, tid);
    exit(1);
  }

  printf("Parent: shared_var = %d (expected 10)\n", shared_var);
  if (shared_var == 10)
    printf("TEST 1 PASSED\n");
  else
    printf("TEST 1 FAILED\n");
}

// --- Test 2: multiple threads --------------------------------
void
worker_multi(void *arg)
{
  int id = *(int *)arg;
  printf("  [thread %d] adding 100 to shared_var\n", id);
  shared_var += 100;
  exit(0);
}

void
test_multiple_threads(void)
{
  printf("\n=== Test 2: multiple threads sharing memory ===\n");

  shared_var = 0;

  int ids[3];
  int tids[3];

  for (int i = 0; i < 3; i++) {
    ids[i] = i + 1;
    tids[i] = thread_create(worker_multi, &ids[i]);
    if (tids[i] < 0) {
      printf("FAIL: thread_create for thread %d failed\n", i + 1);
      exit(1);
    }
    printf("Parent: created thread %d with tid=%d\n", i + 1, tids[i]);
  }

  // join all threads
  for (int i = 0; i < 3; i++) {
    int ret = thread_join(tids[i]);
    if (ret < 0) {
      printf("FAIL: thread_join for tid=%d returned %d\n", tids[i], ret);
      exit(1);
    }
    printf("Parent: joined thread tid=%d\n", ret);
  }

  printf("Parent: shared_var = %d (expected 300)\n", shared_var);
  if (shared_var == 300)
    printf("TEST 2 PASSED\n");
  else
    printf("TEST 2 FAILED\n");
}

// --- Test 3: thread argument passing -------------------------
int arg_result = 0;

void
worker_arg(void *arg)
{
  int val = *(int *)arg;
  arg_result = val * val;   // square the argument
  printf("  [thread] computed %d * %d = %d\n", val, val, arg_result);
  exit(0);
}

void
test_argument_passing(void)
{
  printf("\n=== Test 3: argument passing to thread ===\n");

  arg_result = 0;
  int val = 7;

  int tid = thread_create(worker_arg, &val);
  if (tid < 0) {
    printf("FAIL: thread_create returned %d\n", tid);
    exit(1);
  }

  thread_join(tid);

  printf("Parent: arg_result = %d (expected 49)\n", arg_result);
  if (arg_result == 49)
    printf("TEST 3 PASSED\n");
  else
    printf("TEST 3 FAILED\n");
}

// ---- main ---------------------------------------------------
int
main(int argc, char *argv[])
{
  printf("========================================\n");
  printf(" thread_test_ayush — Ayush's thread API\n");
  printf("========================================\n");

  test_basic();
  test_multiple_threads();
  test_argument_passing();

  printf("\n=== All tests finished ===\n");
  exit(0);
}
