# Project 2 — CPU Scheduling Algorithm Simulators

## Overview

Each team member built standalone C programs that **simulate CPU scheduling algorithms**. Given a set of processes with burst times, arrival times, priorities, and other parameters, the simulators run the selected algorithm step-by-step and display Gantt charts, execution logs, and performance metrics directly in the terminal.

---

## Algorithms Implemented

### MLFQ (Multi-Level Feedback Queue)
- Uses multiple priority queues (typically 3 levels)
- New processes start at the highest priority queue (Q0)
- If a process exhausts its time quantum, it gets **demoted** to the next lower queue
- Lower queues have progressively larger time quantums
- Naturally separates short interactive jobs from long CPU-bound ones

**Implemented by:** Shajith, Bhanu, Aryan

### Lottery Scheduling
- Each process is assigned a number of **lottery tickets**
- At every time unit, a random ticket is drawn
- The process holding the winning ticket gets the CPU
- More tickets = higher probability of being scheduled (proportional-share)

**Implemented by:** Shajith, Ayush, Vijay

### RMS (Rate-Monotonic Scheduling)
- Designed for **periodic real-time tasks**
- Priority is fixed based on period — shorter period = higher priority
- Fully preemptive: higher priority processes arriving will preempt the current one
- Processes are re-released at the start of each new period

**Implemented by:** Ayush, Aryan, Vijay

### EDF (Earliest Deadline First)
- Dynamic-priority real-time scheduling
- The process with the **nearest absolute deadline** runs first
- Optimal for single-processor systems (achieves 100% utilization if schedulable)

**Implemented by:** Bhanu, Aryan, Manjula

### Custom Round Robin (RR)
- Symmetric multiprocessor approach using a global ready queue
- Each process is assigned a fixed time quantum for execution
- If a process exhausts its quantum without finishing, it is preempted and moved back to the ready queue
- Idle CPUs continuously pull the next available task, preventing processor affinity issues and ensuring load balancing

**Implemented by:** Aryan

---

## Project Structure

Each team member has their own self-contained directory:

```
Groupname_Project2_2/
├── shajith/                    ← MLFQ + Lottery (single CPU)
│   ├── src/
│   │   ├── main.c              ← Menu-driven entry point
│   │   ├── mlfq.c              ← 3-level MLFQ with configurable quantums
│   │   ├── mlfq.h
│   │   ├── lottery.c           ← Ticket-based probabilistic scheduling
│   │   └── lottery.h
│   ├── screenshots/
│   │   ├── lottery_output.png
│   │   └── mlfq_output.png
│   └── Makefile
│
├── Ayush/                      ← Lottery + RMS (2-CPU multiprocessor)
│   ├── src/
│   │   ├── main.c              ← 20 pre-configured processes, 2 CPUs
│   │   ├── algorithms.c        ← Lottery + RMS with multiprocessor support
│   │   ├── algorithms.h
│   │   └── types.h             ← Process & CPU structs
│   └── Makefile
│
├── Bhanu/                      ← MLFQ + EDF (2-CPU multiprocessor)
│   ├── src/
│   │   ├── main.c              ← 20 processes, 2 CPUs
│   │   ├── mlfq.c              ← MLFQ with wait-time tracking
│   │   ├── mlfq.h
│   │   ├── edf.c               ← Earliest Deadline First
│   │   ├── edf.h
│   │   ├── algorithms.h
│   │   └── types.h
│   ├── simulation/             ← Output screenshots
│   └── Makefile
│
├── Aryan/                      ← Custom RR + EDF + MLFQ (dynamic CPU count)
│   ├── src/
│   │   ├── main.c              ← 20 processes, user-specified CPU count
│   │   ├── algorithms.c        ← Custom RR, EDF, MLFQ — all multiprocessor
│   │   ├── algorithms.h
│   │   ├── stats.c             ← Performance metrics module
│   │   ├── stats.h
│   │   └── types.h
│   ├── results/                ← Output screenshots
│   └── Makefile
│
├── Vijay/                      ← Lottery + RMS (single CPU, dynamic input)
│   ├── main.c
│   ├── algorithms.c            ← Full implementations with Gantt chart printer
│   ├── algorithms.h
│   ├── Makefile
│   └── README.md               ← Detailed README with sample test runs
│
└── README.md
```

---

## Per-Member Details

### Shajith — MLFQ + Lottery *(Single CPU)*

**MLFQ:** 3 queue levels with user-configurable time quantums. Processes arrive at different times and enter Q0. On quantum expiry, they are demoted to the next queue. Outputs a Gantt chart and calculates average waiting and turnaround times.

**Lottery:** User inputs burst times, ticket counts, and a random seed. The simulator runs ticket-based selection and tracks per-process completion, waiting time, and turnaround time.

```bash
cd shajith && make && ./scheduler_sim
```

---

### Ayush — Lottery + RMS *(2-CPU Multiprocessor)*

Pre-configured with **20 processes** (arrival times from 0 to 70, burst times from 2 to 50). Simulates scheduling across **2 CPUs** with multiprocessor-aware logic for both Lottery (ticket selection across CPUs) and RMS (periodic fixed-priority preemption).

```bash
cd Ayush && make && ./scheduler_sim
```

---

### Bhanu — MLFQ + EDF *(2-CPU Multiprocessor)*

20 processes with deadlines simulated on 2 CPUs. MLFQ includes wait-time accumulation tracking per process. EDF selects the process with the earliest absolute deadline at each tick. Includes simulation screenshots in the `simulation/` folder.

```bash
cd Bhanu && make && ./scheduler_sim
```

---

### Aryan — Custom RR + EDF + MLFQ *(Dynamic CPU Count)*

The most comprehensive simulator — **3 algorithms** in one binary. Users can specify the number of CPUs at runtime (2, 4, 8, etc.). Includes a dedicated `stats.c` module for printing detailed performance metrics (turnaround time, waiting time, CPU utilization). Output screenshots in `results/`.

```bash
cd Aryan && make && ./scheduler_sim
```

---

### Vijay — Lottery + RMS *(Single CPU, Dynamic Input)*

All process parameters are entered by the user at runtime (burst time, arrival time, tickets, period). Supports running both algorithms back-to-back on the same process set for side-by-side comparison. Produces text-based Gantt charts and execution summary tables. Comes with its own detailed README with sample test runs.

```bash
cd Vijay && make run
```

---

## How to Build & Run

### Prerequisites
- GCC (MinGW on Windows, gcc on Linux/Mac)
- GNU Make

### Build Any Simulator
```bash
cd Groupname_Project2_2/<MemberName>
make
./scheduler_sim       # or ./scheduler.exe for Vijay on Windows
```

### Clean
```bash
make clean
```

---

## Algorithm Comparison

| Algorithm | Type | Preemptive | Priority Model | Best For |
|-----------|------|-----------|----------------|----------|
| MLFQ | Multi-Queue | Yes (quantum-based) | Dynamic (demotion) | General-purpose systems |
| Lottery | Probabilistic | Yes (per-tick) | Proportional (tickets) | Fair sharing |
| RMS | Fixed-Priority | Yes (arrival-based) | Static (by period) | Hard real-time periodic tasks |
| EDF | Dynamic-Priority | Yes (deadline-based) | Dynamic (by deadline) | Real-time, optimal utilization |

---

## Team Contributions

| Member | Algorithm A | Algorithm B | Features |
|--------|-------------|-------------|----------|
| Shajith | MLFQ (3 queues) | Lottery | Custom quantums, seed control |
| Ayush | Lottery | RMS | 2-CPU multiprocessor |
| Bhanu | MLFQ | EDF | 2-CPU multiprocessor, wait-time tracking |
| Aryan | Custom RR + MLFQ | EDF | Dynamic CPU count, stats module |
| Vijay | Lottery | RMS | Dynamic user input, dual Gantt charts |
| Manjula | EDF | MLFQ | — |

---

## Sample Output

Example from Vijay's Lottery scheduling simulation:

```
  CPU Scheduling Algorithm Simulator
  Lottery & Rate-Monotonic Scheduling

--- Starting Lottery Scheduling ---
Time 0: Process P2 runs (Lottery)
Time 1: Process P2 runs (Lottery)
Time 2: Process P1 runs (Lottery)
Time 3: Process P2 runs (Lottery)
Time 4: Process P3 runs (Lottery)
...
Time 15: All processes finished.
--- Lottery Scheduling Done ---

-------------------------------------------
GANTT CHART: Lottery Scheduling
-------------------------------------------

  +----+----+----+----+----+----+----+----+----+
  | P2 | P2 | P1 | P2 | P3 | P2 | P1 | P3 | P3 |
  +----+----+----+----+----+----+----+----+----+
  0    1    2    3    4    5    6    7    8    9

Execution Summary:
+------+----------+
| Time | Process  |
+------+----------+
|   0  |   P2     |
|   1  |   P2     |
|   2  |   P1     |
|   3  |   P2     |
...
+------+----------+

  Simulation Complete!
```
