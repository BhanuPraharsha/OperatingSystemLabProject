# Operating Systems Lab Project

> Custom System Calls in xv6 RISC-V & CPU Scheduling Algorithm Simulators

**Course:** Operating Systems Lab  
**Team Size:** 6 Members  
**Architecture:** RISC-V (xv6)  
**Language:** C

---

## About

This repository contains our two major OS Lab projects. In Project 1, we extended the xv6 operating system with custom system calls covering concepts like IPC, threading, signals, and file system traversal. In Project 2, each team member built standalone CPU scheduling simulators implementing algorithms like MLFQ, Lottery, RMS, and EDF.

---

## Repository Structure

```
├── Groupname_Project1_xv6CustomizeSystemCalls/   ← Project 1: Custom System Calls in xv6
│   ├── xv6-riscv/                                 ← Modified xv6 kernel + user programs
│   │   ├── kernel/                                 ← Kernel-level code (syscalls, process mgmt, traps)
│   │   └── user/                                   ← User-space test programs
│   └── docs/                                       ← Documentation, logs, and screenshots per syscall
│
├── Groupname_Project2_2/                          ← Project 2: CPU Scheduling Algorithm Simulators
│   ├── shajith/                                    ← MLFQ + Lottery
│   ├── Ayush/                                      ← Lottery + RMS (Multiprocessor)
│   ├── Bhanu/                                      ← MLFQ + EDF (Multiprocessor)
│   ├── Aryan/                                      ← RMS + EDF + Custom RR (Multiprocessor)
│   └── Vijay/                                      ← Lottery + RMS
│
├── LICENSE
└── README.md
```

---

## Project 1 — Custom System Calls in xv6 RISC-V

We added **8 new system calls** to the xv6 operating system running on RISC-V. Each team member implemented a different syscall touching different OS concepts — IPC, threading, signals, synchronization, process diagnostics, and file system utilities.

| Syscall | Description | Category | Implemented By |
|---------|------------|----------|---------------|
| `msgq_send` / `msgq_recv` | Kernel-buffered message queue for IPC | Inter-Process Communication | Shajith |
| `thread_create` / `thread_join` | Thread creation with shared memory and user-allocated stacks | Threads | Ayush |
| `clone` / `join` | Lightweight thread creation with shared address space (page table aliasing) | Threads | Bhanu |
| `sem_wait` / `sem_post` | Counting semaphores for synchronization | Locks / Synchronization | Vijay |
| `signal` / `sigreturn` | UNIX-style signal delivery with custom handlers | Signal Handling | Team |
| `psinfo` | Reads kernel process table and returns process info | System Diagnostics | Aryan |
| `getcwd` | Traverses inode tree to return absolute CWD path | File System | Manjula |

User-space test programs for each syscall (`msgqtest`, `threadtest`, `test_signal`, `ps`, `pwd`) can be run directly inside the xv6 QEMU emulator.

**→ [Full Project 1 Documentation](./Groupname_Project1_xv6CustomizeSystemCalls/README.md)**

---

## Project 2 — CPU Scheduling Algorithm Simulators

Each team member independently implemented **two CPU scheduling algorithms** as standalone C simulators. These terminal-based programs accept process details as input and simulate scheduling with Gantt chart output and performance metrics.

| Team Member | Algorithm A | Algorithm B |
|------------|-------------|-------------|
| Shajith | MLFQ (Multi-Level Feedback Queue) | Lottery Scheduling |
| Ayush | Lottery Scheduling | RMS (Rate-Monotonic Scheduling) |
| Bhanu | MLFQ | EDF (Earliest Deadline First) |
| Aryan | RMS | EDF + Custom Round Robin |
| Vijay | Lottery Scheduling | RMS |
| Manjula | EDF | MLFQ |

The simulators collectively cover four scheduling algorithms:
- **MLFQ** — Multi-level queues with priority demotion on quantum expiry
- **Lottery** — Probabilistic scheduling based on ticket allocation
- **RMS** — Fixed-priority preemptive scheduling for periodic real-time tasks
- **EDF** — Dynamic-priority scheduling based on absolute deadlines

Some implementations (Ayush, Bhanu, Aryan) include **multiprocessor support** (2+ CPUs), while others (Shajith, Vijay) focus on single-CPU simulation with detailed user input and output.

**→ [Full Project 2 Documentation](./Groupname_Project2_2/README.md)**

---

## Tech Stack

| Component | Technology |
|-----------|-----------|
| OS Kernel | xv6-riscv (MIT's teaching OS) |
| Language | C |
| Emulator | QEMU (RISC-V 64-bit) |
| Build System | GNU Make |
| Toolchain | riscv64-unknown-elf-gcc |
| Platform | Linux / WSL |

---

## Quick Start

### Project 1 — Run Modified xv6
```bash
cd Groupname_Project1_xv6CustomizeSystemCalls/xv6-riscv
make qemu
```
Once xv6 boots, try our custom programs:
```
$ ps             # Process table diagnostics
$ msgqtest       # Message queue IPC demo
$ threadtest     # Thread creation demo
$ test_signal    # Signal handling demo
$ pwd            # Print working directory
```

### Project 2 — Build & Run Any Simulator
```bash
cd Groupname_Project2_2/<MemberName>
make
./scheduler_sim
```

---

## Team

| # | Name | Project 1 (System Call) | Project 2 (Scheduling Algorithms) |
|---|------|------------------------|----------------------------------|
| 1 | Shajith | `msgq_send` / `msgq_recv` — IPC | MLFQ, Lottery |
| 2 | Ayush | `thread_create` / `thread_join` — Threads | Lottery, RMS |
| 3 | Bhanu | `clone` / `join` — Threads | MLFQ, EDF |
| 4 | Aryan | `psinfo` — Process Diagnostics | RMS, EDF, Custom RR |
| 5 | Vijay | `sem_wait` / `sem_post` — Semaphores | Lottery, RMS |
| 6 | Manjula | `getcwd` — File System | EDF, MLFQ |

---

## License

This project is licensed under the [MIT License](./LICENSE).
