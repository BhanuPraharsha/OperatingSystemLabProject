# Message Queue Syscalls (`msgq_send` and `msgq_recv`)

## What is this?

A kernel-level message queue for inter-process communication (IPC) in xv6 (RISC-V). Processes can send and receive short messages through shared kernel buffers using two new system calls.

- `msgq_send(int qid, char *msg)` - sends a message to a kernel queue
- `msgq_recv(int qid, char *buf)` - receives the next message from a kernel queue

## How it works

The kernel maintains 4 message queues. Each queue is a struct with:

- A spinlock for synchronization
- A circular buffer of 8 message slots (64 bytes each)
- `head`, `tail`, and `count` fields to track the buffer state

When a process calls `msgq_send`, the kernel uses `copyin()` to copy the message from user space into the next available slot in the circular buffer. When a process calls `msgq_recv`, the kernel uses `copyout()` to copy the oldest message from the buffer back to user space.

The spinlock makes sure only one process touches a queue at a time, so it's safe to use between parent and child (or any two processes).

All queues are initialized lazily on first use.

## Syscall numbers

| Syscall | Number |
|---------|--------|
| `msgq_send` | 25 |
| `msgq_recv` | 26 |

## Files modified

All paths are relative to `xv6-riscv/`:

| File | What changed |
|------|-------------|
| `kernel/syscall.h` | Added `SYS_msgq_send` (25) and `SYS_msgq_recv` (26) |
| `kernel/syscall.c` | Added extern declarations and dispatch table entries |
| `user/usys.pl` | Added entries for user-space stubs |
| `user/user.h` | Added function prototypes |
| `kernel/sysproc.c` | Implemented `sys_msgq_send()` and `sys_msgq_recv()` with the queue struct |
| `user/msgqtest.c` | Test program that demos send/recv between parent and child |
| `Makefile` | Added `_msgqtest` to UPROGS |

## How to test

```
cd xv6-riscv
make qemu
```

Once xv6 boots up, run:

```
$ msgqtest
```

### Expected output

```
msgq test: starting
parent: sent message
child: got message -> hello from parent!
```

(Note: the parent and child lines might appear slightly interleaved since xv6 doesn't synchronize console output between processes. This is normal.)

## Screenshot

See [screenshot.png](screenshot.png) for a screenshot of the test running in QEMU.

## Author

Shajith
