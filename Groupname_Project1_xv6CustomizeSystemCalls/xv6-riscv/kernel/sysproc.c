#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
#include "signal.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if(t == SBRK_EAGER || n < 0) {
    if(growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if(addr + n < addr)
      return -1;
    if(addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;
  int signum; //1. create a variable to hold the signal number

  //2.fetch both arguments:
  argint(0, &pid);
  argint(1, &signum);
// printf("KERNEL: sys_kill received pid=%d, signum=%d\n", pid, signum);
  return kkill(pid, signum);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//function for psinfo that will take the pointer passed from userspace and pass it to the helper
uint64
sys_psinfo(void)
{
  uint64 p; //holds the virtual address of user's array

  //argaddr fetches the 0th argument that is our struct uproc pointer
  argaddr(0, &p);
  return get_psinfo(p);


}

//user programs need a way to tell the kernel when i got sigint run this
uint64
sys_signal(void){
  int signum;
  uint64 handler_addr;
  argint(0, &signum);
  argaddr(1, &handler_addr);
// printf("KERNEL: sys_signal registered signum=%d to address 0x%lx\n", signum, handler_addr);
  if(signum < 0 || signum >= NSIG || signum == SIGKILL)
  return -1; //cannot catch sigkill


  struct proc*p = myproc();
  acquire(&p->lock);
  p->signal_handlers[signum] = (void (*)(int))handler_addr;
  release(&p->lock);

  return 0;
}


uint64
sys_sigreturn(void) {
  struct proc *p = myproc();

  //restore the original execution state
  memmove(p->trapframe, p->sig_tf, sizeof(struct trapframe));

  //allow new signals to be handled
  p->is_handling_signal = 0;
  return p->trapframe->a0;//return the original signal
}
