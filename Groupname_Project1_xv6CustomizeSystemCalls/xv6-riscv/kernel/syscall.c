#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"
#include "defs.h"

// Fetch the uint64 at addr from the current process.
int fetchaddr(uint64 addr, uint64 *ip)
{
  struct proc *p = myproc();
  if (addr >= p->sz || addr + sizeof(uint64) > p->sz) // both tests needed, in case of overflow
    return -1;
  if (copyin(p->pagetable, (char *)ip, addr, sizeof(*ip)) != 0)
    return -1;
  return 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
int fetchstr(uint64 addr, char *buf, int max)
{
  struct proc *p = myproc();
  if (copyinstr(p->pagetable, buf, addr, max) < 0)
    return -1;
  return strlen(buf);
}

static uint64
argraw(int n)
{
  struct proc *p = myproc();
  switch (n)
  {
  case 0:
    return p->trapframe->a0;
  case 1:
    return p->trapframe->a1;
  case 2:
    return p->trapframe->a2;
  case 3:
    return p->trapframe->a3;
  case 4:
    return p->trapframe->a4;
  case 5:
    return p->trapframe->a5;
  }
  panic("argraw");
  return -1;
}

// Fetch the nth 32-bit system call argument.
void argint(int n, int *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
void argaddr(int n, uint64 *ip)
{
  *ip = argraw(n);
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (including nul), -1 if error.
int argstr(int n, char *buf, int max)
{
  uint64 addr;
  argaddr(n, &addr);
  return fetchstr(addr, buf, max);
}

// Prototypes for the functions that handle system calls.
extern uint64 sys_fork(void);
extern uint64 sys_exit(void);
extern uint64 sys_wait(void);
extern uint64 sys_pipe(void);
extern uint64 sys_read(void);
extern uint64 sys_kill(void);
extern uint64 sys_exec(void);
extern uint64 sys_fstat(void);
extern uint64 sys_chdir(void);
extern uint64 sys_dup(void);
extern uint64 sys_getpid(void);
extern uint64 sys_sbrk(void);
extern uint64 sys_pause(void);
extern uint64 sys_uptime(void);
extern uint64 sys_open(void);
extern uint64 sys_write(void);
extern uint64 sys_mknod(void);
extern uint64 sys_unlink(void);
extern uint64 sys_link(void);
extern uint64 sys_mkdir(void);
extern uint64 sys_close(void);
extern uint64 sys_psinfo(void);
extern uint64 sys_signal(void);
extern uint64 sys_sigreturn(void);
extern uint64 sys_msgq_send(void);
extern uint64 sys_msgq_recv(void);
extern uint64 sys_clone(void);
extern uint64 sys_join(void);
extern uint64 sys_sem_wait(void);
extern uint64 sys_sem_post(void);
extern uint64 sys_getcwd(void);

// An array mapping syscall numbers from syscall.h
// to the function that handles the system call.
static uint64 (*syscalls[])(void) = {
    [SYS_fork] sys_fork,
    [SYS_exit] sys_exit,
    [SYS_wait] sys_wait,
    [SYS_pipe] sys_pipe,
    [SYS_read] sys_read,
    [SYS_kill] sys_kill,
    [SYS_exec] sys_exec,
    [SYS_fstat] sys_fstat,
    [SYS_chdir] sys_chdir,
    [SYS_dup] sys_dup,
    [SYS_getpid] sys_getpid,
    [SYS_sbrk] sys_sbrk,
    [SYS_pause] sys_pause,
    [SYS_uptime] sys_uptime,
    [SYS_open] sys_open,
    [SYS_write] sys_write,
    [SYS_mknod] sys_mknod,
    [SYS_unlink] sys_unlink,
    [SYS_link] sys_link,
    [SYS_mkdir] sys_mkdir,
    [SYS_close] sys_close,
    [SYS_psinfo] sys_psinfo,
    [SYS_signal] sys_signal,
    [SYS_sigreturn] sys_sigreturn,
    [SYS_msgq_send] sys_msgq_send,
    [SYS_msgq_recv] sys_msgq_recv,
    [SYS_clone] sys_clone,
    [SYS_join] sys_join,
[SYS_sem_wait] sys_sem_wait,
[SYS_sem_post] sys_sem_post,
    [SYS_getcwd] sys_getcwd,
};

void syscall(void)
{
  int num;
  struct proc *p = myproc();

  num = p->trapframe->a7;
  if (num > 0 && num < NELEM(syscalls) && syscalls[num])
  {
    // Use num to lookup the system call function for num, call it,
    // and store its return value in p->trapframe->a0
    p->trapframe->a0 = syscalls[num]();
  }
  else
  {
    printf("%d %s: unknown sys call %d\n",
           p->pid, p->name, num);
    p->trapframe->a0 = -1;
  }
}

uint64
sys_getcwd(void)
{
  char *buf;
  int size;

  // Get user pointer and buffer size from arguments
  argaddr(0, (uint64 *)&buf);
  argint(1, &size);

  // Temporary kernel buffer to build the path
  char kbuf[512];
  int len = 0;

  // Component storage: walk up, collect names bottom-up
  // Each component: at most DIRSIZ chars
  char components[32][DIRSIZ + 1];
  int depth = 0;

  struct inode *cur, *parent;
  struct dirent de;
  uint off;

  // Start at the process's current working directory
  // Hold a reference so we can walk up safely
  cur = idup(myproc()->cwd);

  while (1)
  {
    // Open ".." inside cur to get the parent inode
    // In xv6 we can use dirlookup directly on the locked inode
    ilock(cur);
    parent = dirlookup(cur, "..", 0); // returns a new ref, unlocked
    iunlock(cur);

    if (parent == 0)
    {
      iunlockput(cur); // shouldn't happen
      return -1;
    }

    ilock(parent);

    // If cur and parent have same inum (and same dev), we are at root
    if (cur->inum == parent->inum)
    {
      iunlock(parent);
      iput(parent);
      iput(cur);
      break;
    }

    // Scan parent directory to find entry whose inum == cur->inum
    int found = 0;
    for (off = 0; off < (uint)parent->size; off += sizeof(de))
    {
      if (readi(parent, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
        break;
      if (de.inum == 0)
        continue;
      if (de.inum == cur->inum)
      {
        // Copy the name
        memmove(components[depth], de.name, DIRSIZ);
        components[depth][DIRSIZ] = '\0';
        depth++;
        found = 1;
        break;
      }
    }

    iunlock(parent);

    if (!found)
    {
      iput(parent);
      iput(cur);
      return -1;
    }

    iput(cur);
    cur = parent; // move up one level
  }

  // Build the path string in kbuf
  if (depth == 0)
  {
    // cwd is root itself
    kbuf[0] = '/';
    kbuf[1] = '\0';
    len = 2;
  }
  else
  {
    len = 0;
    // Walk components in reverse (deepest last in array, first in path)
    for (int i = depth - 1; i >= 0; i--)
    {
      kbuf[len++] = '/';
      int nlen = strlen(components[i]);
      memmove(kbuf + len, components[i], nlen);
      len += nlen;
    }
    kbuf[len++] = '\0';
  }

  if (len > size)
    return -1; // user buffer too small

  // Copy result to user space
  if (copyout(myproc()->pagetable, (uint64)buf, kbuf, len) < 0)
    return -1;

  return (uint64)buf;
}