# Project 1: xv6 Customized System Calls - Semaphore Implementation

## Overview
This section of the project focuses on adding **Semaphore system calls** (`sem_wait` and `sem_post`) to the xv6 operating system kernel, along with a user-space test program (`semtest`) to verify their functionality. Semaphores are synchronization primitives that help manage concurrent processes and prevent race conditions when accessing shared resources.

## Changes Made for Semaphore Portion

The following files were modified and added to implement the semaphore functionality:

### 1. `kernel/sysproc.c`
This file contains the core logic for the semaphore system calls.
* **`struct sem` & `semtable`**: Created a semaphore structure containing a spinlock and a count variable. An array of 10 semaphores (`semtable[10]`) was added to allow multiple user processes to use semaphores globally.
* **`seminit(void)`**: A function to initialize the 10 semaphores as binary semaphores (initial count = 1) and initialize their spinlocks. This is designed to be called when the kernel boots up.
* **`sys_sem_wait(void)`**: Implements the logic to acquire a semaphore resource. If the semaphore count is `0` or less, the calling process goes to sleep using XV6's `sleep()` mechanism. Once acquired, the count is decremented.
* **`sys_sem_post(void)`**: Implements the logic to release a semaphore resource. It increments the semaphore count and uses `wakeup()` to awaken any processes that went to sleep waiting for this specific semaphore.

### 2. Kernel and System Call wiring (Typical XV6 flow)
To make `sem_wait` and `sem_post` accessible to user programs, they were wired into the kernel:
* **`kernel/syscall.h`**: Defined the system call numbers (e.g., `SYS_sem_wait` and `SYS_sem_post`).
* **`kernel/syscall.c`**: Added the function pointers to the `syscalls` array to route the user requests to `sys_sem_wait` and `sys_sem_post`.
* **`user/usys.pl`**: Added stubs so that the user-space assembly code for system calls is automatically generated.
* **`user/user.h`**: Added the C function prototypes `int sem_wait(int);` and `int sem_post(int);` so user programs can call them.

### 3. `user/semtest.c` (New File)
A user-level test program was created to demonstrate mutual exclusion.
* The program calls `fork()` to create a parent and a child process.
* Both processes attempt to enter a "critical section" where they print a message.
* `sem_wait(0)` and `sem_post(0)` are used around the print statements to guarantee that the parent and child do not execute the print statements simultaneously, ensuring mutual exclusion lock over the terminal output.

### 4. `Makefile`
* Appended `$U/_semtest` to the `UPROGS` section so that the `semtest` program compiles into the xv6 file system when running `make qemu`.

---

## How to Test and Verify

To verify that the semaphores work correctly, run the OS and execute the test program on the xv6 terminal.

1. Open your terminal in the `xv6-riscv` folder.
2. Compile and boot the xv6 testing environment by running:
   ```bash
   make qemu
   ```
3. Once xv6 boots up and you see the `$` prompt, type the following command to run your test:
   ```bash
   semtest
   ```

### Expected Output
Because the semaphores enforce mutual exclusion, the parent and child processes will print one after another without mixing their outputs. You should see output similar to this:

```text
$ semtest
semaphore test starting...
child: inside critical section
parent: inside critical section
semaphore test done!
```
*(Note: The order of "child" vs "parent" might swap depending on CPU scheduling, but they will never print on top of each other).*

4. To exit the xv6 QEMU environment, press `Ctrl-A` followed by `X`.

---

## Explanation for Presentation / Others
When explaining your portion to others, you can highlight the following points:
1. **The Goal**: Normal processes are unaware of each other and can cause data corruption if they access shared resources simultaneously. My contribution was to introduce **Semaphores**, a fundamental OS tool to prevent this.
2. **The Implementation**: I integrated an array of 10 binary semaphores directly into the xv6 kernel memory space. I had to use kernel-level **spinlocks** (`acquire` and `release`) inside the semaphore logic to prevent the semaphores themselves from race conditions during multi-core CPU execution.
3. **Wait & Sleep**: I utilized xv6's `sleep()` and `wakeup()` kernel API inside `sys_sem_wait` and `sys_sem_post` so that processes waiting for a lock don't waste CPU cycles (busy waiting), but rather securely pause execution until signaled.
4. **Validation**: I designed `semtest.c` to deliberately spawn a child process constraint against a parent process to battle for the same console write permission. The clean sequential printout validates the successful lock and release pipeline.
# Project 1 — Custom System Calls in xv6 RISC-V

## Overview

This project extends the xv6 operating system (RISC-V) with **8 new system calls**. Each team member picked a different OS concept and implemented it as a working kernel-level system call — modifying the kernel source code, registering new syscall numbers, adding data structures to the process control block, and wiring everything through the trap/dispatch pipeline.

The original xv6 has 21 system calls. We brought it up to **29**.

---

## System Calls Implemented

### 1. `msgq_send` & `msgq_recv` — Message Queue IPC *(Shajith)*

**Syscall Numbers:** 25, 26

A kernel-level message queue for inter-process communication. Instead of shared memory, this uses fixed-size buffers inside kernel space. One process pushes a message into a queue, another reads from it — pure message passing.

**Internal Design:**
- The kernel maintains **4 message queues**, each with 8 slots of 64 bytes
- Each queue is a circular buffer protected by a `spinlock`
- `msgq_send()` uses `copyin()` to copy from user space into the kernel buffer
- `msgq_recv()` uses `copyout()` to copy the oldest message back to user space
- Queues are initialized lazily on first use

**Files Modified:**
| File | Changes |
|------|---------|
| `kernel/sysproc.c` | Queue struct, `sys_msgq_send()`, `sys_msgq_recv()` |
| `kernel/syscall.h` | Syscall numbers 25, 26 |
| `kernel/syscall.c` | Dispatch table entries |
| `user/user.h` | Function prototypes |
| `user/usys.pl` | Stub generation |
| `user/msgqtest.c` | Test program (NEW) |
| `Makefile` | Added `_msgqtest` to UPROGS |

**Test Output:**
```
$ msgqtest
msgq test: starting
parent: sent message
child: got message -> hello from parent!
```

---

### 2. `thread_create` & `thread_join` — Lightweight Threads *(Ayush)*

Ayush designed and implemented the thread creation mechanism for xv6. The task was to build a syscall that creates a new thread of execution within the same process. To avoid memory management complexity, the user program allocates the stack space using standard arrays/malloc and passes the pointer to the kernel. The kernel then sets up the trapframe — specifically the program counter and stack pointer — so the new thread starts executing the given function concurrently while sharing the parent's memory.

---

### 3. `clone` & `join` — Thread Creation with Shared Page Tables *(Bhanu)*

**Syscall Numbers:** 27, 28

Bhanu implemented the kernel-level mechanics for `clone` and `join`. Unlike `fork()` which deep-copies memory, `clone()` creates a new process that **shares the parent's address space** via page table aliasing. The child gets its own trapframe and stack, but heap, globals, and code are all shared.

**Internal Design:**
- Created `uvmshare()` in `vm.c` — maps child PTEs directly to parent's physical frames (shallow aliasing, no copy)
- Added `is_thread` and `mem_parent` fields to `struct proc`
- Modified `freeproc()` so threads only unmap PTEs without freeing the parent's physical memory
- `kclone()` in `proc.c` sets the program counter to the function pointer, stack pointer to user-allocated stack, and calls `uvmshare()` to link memory
- `kjoin()` sleeps the caller until the target thread transitions to ZOMBIE state

**Files Modified:**
| File | Changes |
|------|---------|
| `kernel/proc.h` | Added `is_thread`, `mem_parent` to struct proc |
| `kernel/proc.c` | `kclone()`, `kjoin()`, modified `freeproc()` |
| `kernel/vm.c` | Added `uvmshare()` |
| `kernel/vm.h` | Header for shared memory constants |
| `kernel/sysproc.c` | `sys_clone()`, `sys_join()` |
| `user/threadtest.c` | Test program (NEW) |

**How it works:**
- The `threadtest.c` user program demonstrates this: a parent spawns a thread using clone function, that increments a shared global variable, proving both execution contexts share the same memory

**Test Output:**
```
$ threadtest
Parent: shared_counter is 0
Thread 1 started.
Thread 1 incremented shared_counter to 100.
Parent: spawned thread with tid 4
Parent: successfully joined thread 4
Parent: Final shared_counter is 100 (should be 100)
TEST PASSED
```
---

### 4. `sem_wait` & `sem_post` — Counting Semaphores *(Vijay)*

Vijay implemented counting semaphores for process synchronization in xv6. `sem_wait` decrements the semaphore counter and puts the process to sleep if the count drops below zero. `sem_post` increments the counter and wakes up any sleeping processes. The implementation handles CPU yielding and process state transitions to prevent race conditions.

**Design Principles:**
- Standard counting semaphore semantics — `wait` (P operation) and `post` (V operation)
- Uses xv6's `sleep()` and `wakeup()` primitives for blocking/unblocking
- Spinlock protection around the semaphore counter to prevent race conditions
- Processes are put into `SLEEPING` state when the semaphore count is zero

---

### 5. `signal` & `sigreturn` — UNIX-Style Signal Handling

**Syscall Numbers:** 23, 24

A full signal delivery system. Processes can register custom signal handlers via `signal()`, other processes send signals using `kill(pid, signum)`, and the kernel handles delivery by intercepting the trap return path.

**Internal Design:**
- Added to `struct proc`: `pending_signals` bitmask, `signal_handlers[32]` array, `sig_tf` (backup trapframe), `is_handling_signal` (re-entrancy guard)
- Modified `kkill()` to set bits in `pending_signals` instead of directly killing
- In `usertrap()` (trap.c): before returning to user space, checks `pending_signals`, backs up the trapframe, and redirects `epc` to the user's handler
- `sigreturn()` restores the original trapframe so execution resumes where it left off
- `SIG_DFL` kills the process, `SIG_IGN` silently drops the signal, `SIGKILL (9)` cannot be caught

**New Files:**
- `kernel/signal.h` — Signal constants (`NSIG=32`, `SIGINT=2`, `SIGKILL=9`, `SIGTERM=15`, `SIG_DFL`, `SIG_IGN`)

**Files Modified:**
| File | Changes |
|------|---------|
| `kernel/proc.h` | Signal fields in struct proc |
| `kernel/proc.c` | Modified `allocproc()`, `freeproc()`, `kkill()` |
| `kernel/trap.c` | Signal delivery logic in `usertrap()` |
| `kernel/sysproc.c` | `sys_signal()`, `sys_sigreturn()`, modified `sys_kill()` |
| `user/test_signal.c` | Test program (NEW) |

**Test Output:**
```
$ test_signal
Parent: Waiting for child to set up...
Parent: Sending SIGINT (2) to child...
>>> CHILD: Caught signal 2! I refuse to die.
Parent: Sending SIGKILL (9) to child...
Parent: Child reaped. Exiting.
```

> **Note:** `kill` now takes two arguments — `kill <pid> <signal>`. Use `kill <pid> 9` to terminate a process.

---

### 6. `psinfo` — Process Table Diagnostics *(Aryan)*

**Syscall Number:** 22

A `ps`-like utility that reads the kernel's internal process table and returns PID, state, parent PID, and command name for all active processes.

**Internal Design:**
- Created `struct uproc` in `kernel/pstat.h` — a user-friendly struct for process data
- `get_psinfo()` in `proc.c` iterates the `proc[]` array, locks each entry, and uses `copyout()` to send data to user space
- The user program `ps.c` allocates an array of `uproc` structs and passes it to the syscall

**Files Modified:**
| File | Changes |
|------|---------|
| `kernel/pstat.h` | `struct uproc` definition (NEW) |
| `kernel/proc.c` | `get_psinfo()` helper |
| `kernel/sysproc.c` | `sys_psinfo()` |
| `user/ps.c` | User program (NEW) |

**Test Output:**
```
$ ps
PID     PPID    STATE           CMD
---     ----    -----           ---
1       0       SLEEPING        init
2       1       SLEEPING        sh
3       2       RUNNING         ps
```

---

### 7. `getcwd` — Print Working Directory *(Manjula)*

**Syscall Number:** 29

xv6 doesn't have a built-in `pwd` command. This syscall traverses the file system inode tree from the current directory up to root, building the absolute path string along the way.

**Internal Design:**
- Starts at `myproc()->cwd` and walks up using `".."` entries via `dirlookup()`
- At each level, scans the parent directory to find the entry matching the current inode number
- Collects names in reverse order, then builds the full path
- Stops when `current->inum == parent->inum` (root directory reached)
- Uses `copyout()` to return the result to user space

**Files Modified:**
| File | Changes |
|------|---------|
| `kernel/syscall.c` | `sys_getcwd()` implementation |
| `kernel/sysproc.c` | Alternative `sys_getcwd()` implementation |
| `user/pwd.c` | User program (NEW) |

**Test Output:**
```
$ pwd
/
$ mkdir testdir
$ cd testdir
$ pwd
/testdir
```

---

## Project Structure

```
Groupname_Project1_xv6CustomizeSystemCalls/
├── xv6-riscv/
│   ├── kernel/
│   │   ├── sysproc.c        ← Syscall implementations (msgq, signal, clone, getcwd)
│   │   ├── proc.c           ← kclone, kjoin, get_psinfo, modified kkill
│   │   ├── proc.h           ← Signal + thread fields added to struct proc
│   │   ├── trap.c           ← Signal delivery logic in usertrap()
│   │   ├── syscall.h        ← Syscall number definitions (22–29)
│   │   ├── syscall.c        ← Dispatch table + getcwd implementation
│   │   ├── signal.h         ← Signal constants (NEW)
│   │   ├── pstat.h          ← struct uproc for psinfo (NEW)
│   │   ├── vm.c             ← uvmshare() for clone
│   │   └── ...
│   ├── user/
│   │   ├── msgqtest.c       ← Message queue test (Shajith)
│   │   ├── threadtest.c     ← Thread test (Ayush & Bhanu)
│   │   ├── test_signal.c    ← Signal handling test
│   │   ├── ps.c             ← psinfo user program (Aryan)
│   │   ├── pwd.c            ← getcwd user program (Manjula)
│   │   ├── user.h           ← Updated with new syscall prototypes
│   │   └── ...
│   └── Makefile             ← Updated UPROGS
│
└── docs/
    ├── clone/               ← Thread documentation + screenshots
    ├── msgq/                ← Message queue documentation + screenshot
    ├── psinfo/              ← Change log + screenshots
    └── signal/              ← Change log + screenshots
```

---

## How to Build & Run

### Prerequisites
- RISC-V GCC toolchain (`riscv64-unknown-elf-gcc` or `riscv64-linux-gnu-gcc`)
- QEMU ≥ 7.2 with RISC-V 64-bit support
- GNU Make, Perl

### Build & Boot
```bash
cd Groupname_Project1_xv6CustomizeSystemCalls/xv6-riscv
make qemu
```

### Run Test Programs (inside xv6 shell)
```bash
$ ps              # Process table diagnostics
$ msgqtest        # Message queue IPC
$ threadtest      # Thread creation and shared memory
$ test_signal     # Signal handling
$ pwd             # Print working directory
```

### Clean Build
```bash
make clean
make qemu
```

---

## Syscall Summary

| # | Syscall | Number | Category | Author |
|---|---------|--------|----------|--------|
| 1 | `psinfo` | 22 | System Diagnostics | Aryan |
| 2 | `signal` | 23 | Signal Handling | Aryan |
| 3 | `sigreturn` | 24 | Signal Handling | Aryan |
| 4 | `msgq_send` | 25 | IPC | Shajith |
| 5 | `msgq_recv` | 26 | IPC | Shajith |
| 6 | `clone` | 27 | Threads | Bhanu |
| 7 | `join` | 28 | Threads | Bhanu |
| 8 | `getcwd` | 29 | File System | Manjula |

---

## Team Contributions

| Member | Syscall Work | Key Files |
|--------|-------------|-----------|
| Shajith | `msgq_send` / `msgq_recv` (Message Queue IPC) | `sysproc.c`, `msgqtest.c` |
| Ayush | `thread_create` / `thread_join` (Thread system + test program) | `threadtest.c`, thread architecture |
| Bhanu | `clone` / `join` (Kernel-level thread with page table sharing) | `proc.c`, `proc.h`, `vm.c` |
| Aryan | `psinfo` (Process Diagnostics), `signals` | `proc.c`, `pstat.h`, `ps.c`, `signals.h`, `proc.h` |
| Vijay | `sem_wait` / `sem_post` (Counting Semaphores) | Semaphore design |
| Manjula | `getcwd` (Current Working Directory) | `syscall.c`, `sysproc.c`, `pwd.c` |
