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
