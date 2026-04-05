# xv6 Thread System Calls: `clone` and `join`

This project implements lightweight multi-threading in the xv6 RISC-V operating system. 

Unlike `fork()`, which deep-copies a process's memory, our custom `clone()` syscall creates a new thread that shares the exact same virtual address space as the parent, while maintaining its own program counter and stack. We also implemented `join()` to allow a parent to wait for a specific thread ID to finish.

## Core Modifications

1. **Syscall Registration:** 
   Added `SYS_clone (27)` and `SYS_join (28)` into `syscall.h`, with execution wrappers in `sysproc.c` and user stubs generated via `usys.pl`.

2. **Shared Memory (`vm.c`):** 
   Created `uvmshare()`. Instead of copying physical memory pages, it maps the child's Page Table Entries (PTEs) directly to the parent's physical memory frames (shallow aliasing).

3. **Process Block & Cleanup (`proc.h`, `proc.c`):** 
   Added tracking flags (`is_thread` and `mem_parent`). Modified the `freeproc()` cleanup routine so exiting threads only destroy their page tables, preventing them from accidentally freeing the parent's shared physical memory.

4. **Kernel Handlers:** 
   - `kclone()`: Allocates a proc block, sets the program counter to the passed function, sets the stack pointer to the user-allocated stack heap, and calls `uvmshare()` to link memory.
   - `kjoin()`: Scans the process table and sleeps the caller until the specific child thread transitions into a `ZOMBIE` state.

## Testing
We added a user-space program `threadtest.c` to verify functionality. A parent process allocates a new stack via `malloc()`, spawns a child using `clone()`, and waits using `join()`. The child successfully increments a global variable (`shared_counter`), proving that the two instances physically share memory concurrently.
