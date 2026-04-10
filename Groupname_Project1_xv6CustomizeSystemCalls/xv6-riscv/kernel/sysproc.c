#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "signal.h"
#include "vm.h"

// ---- semaphore stuff ----

// this struct holds one semaphore
// lock is used to protect the count variable
// count keeps track of how many resources are available
struct sem {
  struct spinlock lock;
  int count;
};

// we have 10 semaphores that any process can use
struct sem semtable[10];

// this function sets up all 10 semaphores
// called once when the kernel boots up (from main.c)
void seminit(void) {
  for (int i = 0; i < 10; i++) {
    initlock(&semtable[i].lock, "sem");
    semtable[i].count = 1;  // start with count 1 (binary semaphore)
  }
}

uint64 sys_exit(void) {
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}

uint64 sys_getpid(void) { return myproc()->pid; }

uint64 sys_fork(void) { return kfork(); }

uint64 sys_wait(void) {
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64 sys_sbrk(void) {
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64 sys_pause(void) {
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64 sys_kill(void) {
  int pid;
  int signum; // 1. create a variable to hold the signal number

  // 2.fetch both arguments:
  argint(0, &pid);
  argint(1, &signum);
  // printf("KERNEL: sys_kill received pid=%d, signum=%d\n", pid, signum);
  return kkill(pid, signum);
}

// return how many clock tick interrupts have occurred
// since start.
uint64 sys_uptime(void) {
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// function for psinfo that will take the pointer passed from userspace and pass
// it to the helper
uint64 sys_psinfo(void) {
  uint64 p; // holds the virtual address of user's array

  // argaddr fetches the 0th argument that is our struct uproc pointer
  argaddr(0, &p);
  return get_psinfo(p);
}

// user programs need a way to tell the kernel when i got sigint run this
uint64 sys_signal(void) {
  int signum;
  uint64 handler_addr;
  argint(0, &signum);
  argaddr(1, &handler_addr);
  // printf("KERNEL: sys_signal registered signum=%d to address 0x%lx\n",
  // signum, handler_addr);
  if (signum < 0 || signum >= NSIG || signum == SIGKILL)
    return -1; // cannot catch sigkill

  struct proc *p = myproc();
  acquire(&p->lock);
  p->signal_handlers[signum] = (void (*)(int))handler_addr;
  release(&p->lock);

  return 0;
}

// ---- message queue stuff ----

#define MAX_MSG_SIZE 64
#define MAX_MSGS 8
#define NUM_QUEUES 4

struct msg_queue {
  struct spinlock lock;
  char msgs[MAX_MSGS][MAX_MSG_SIZE];
  int head;
  int tail;
  int count;
};

struct msg_queue msgqueues[NUM_QUEUES];
int msgq_initialized = 0;

// init all queues on first use
void msgq_init(void) {
  for (int i = 0; i < NUM_QUEUES; i++) {
    initlock(&msgqueues[i].lock, "msgq");
    msgqueues[i].head = 0;
    msgqueues[i].tail = 0;
    msgqueues[i].count = 0;
  }
  msgq_initialized = 1;
}

uint64 sys_msgq_send(void) {
  int qid;
  uint64 addr;

  if (!msgq_initialized)
    msgq_init();

  argint(0, &qid);
  argaddr(1, &addr);

  // check if qid is valid
  if (qid < 0 || qid >= NUM_QUEUES)
    return -1;

  struct msg_queue *q = &msgqueues[qid];
  acquire(&q->lock);

  // queue full
  if (q->count == MAX_MSGS) {
    release(&q->lock);
    return -1;
  }

  // copy message from user space into kernel buffer
  struct proc *p = myproc();
  if (copyin(p->pagetable, q->msgs[q->tail], addr, MAX_MSG_SIZE) < 0) {
    release(&q->lock);
    return -1;
  }

  q->tail = (q->tail + 1) % MAX_MSGS;
  q->count++;
  release(&q->lock);
  return 0;
}

uint64 sys_msgq_recv(void) {
  int qid;
  uint64 addr;

  if (!msgq_initialized)
    msgq_init();

  argint(0, &qid);
  argaddr(1, &addr);

  if (qid < 0 || qid >= NUM_QUEUES)
    return -1;

  struct msg_queue *q = &msgqueues[qid];
  acquire(&q->lock);

  // queue empty
  if (q->count == 0) {
    release(&q->lock);
    return -1;
  }

  // copy message from kernel buffer to user space
  struct proc *p = myproc();
  if (copyout(p->pagetable, addr, q->msgs[q->head], MAX_MSG_SIZE) < 0) {
    release(&q->lock);
    return -1;
  }

  q->head = (q->head + 1) % MAX_MSGS;
  q->count--;
  release(&q->lock);
  return 0;
}

uint64 sys_sigreturn(void) {
  struct proc *p = myproc();

  // restore the original execution state
  memmove(p->trapframe, p->sig_tf, sizeof(struct trapframe));

  // allow new signals to be handled
  p->is_handling_signal = 0;
  return p->trapframe->a0; // return the original signal
}

// extract arguments for clone and pass to kclone
uint64 sys_clone(void) {
  uint64 fcn, arg, stack;

  // getting arguments passed from user space
  argaddr(0, &fcn);
  argaddr(1, &arg);
  argaddr(2, &stack);

  return kclone(fcn, stack, arg);
}

// extract argument for join and pass to kjoin
uint64 sys_join(void) {
  int tid;
  uint64 status_addr;

  // getting the thread ID and the status pointer
  argint(0, &tid);
  argaddr(1, &status_addr);

  return kjoin(tid, status_addr);
}

// sys_sem_wait - this is called when a process wants to use a shared resource
// if the semaphore count is 0 or less, the process will go to sleep
// when it wakes up (or if count > 0), it decrements the count and continues
uint64 sys_sem_wait(void) {
  int id;

  // get the semaphore id from user space
  argint(0, &id);

  // check if the id is valid
  if (id < 0 || id >= 10)
    return -1;

  acquire(&semtable[id].lock);

  // if no resources available, sleep until someone does sem_post
  while (semtable[id].count <= 0) {
    sleep(&semtable[id], &semtable[id].lock);
  }

  // now we got the resource, so decrease the count
  semtable[id].count--;

  release(&semtable[id].lock);
  return 0;
}

// sys_sem_post - this is called when a process is done with a shared resource
// it increments the count and wakes up any sleeping process
uint64 sys_sem_post(void) {
  int id;

  // get the semaphore id from user space
  argint(0, &id);

  // check if the id is valid
  if (id < 0 || id >= 10)
    return -1;

  acquire(&semtable[id].lock);

  // increase the count because we are releasing the resource
  semtable[id].count++;

  // wake up any process that was waiting for this semaphore
  wakeup(&semtable[id]);

  release(&semtable[id].lock);
  return 0;
}
