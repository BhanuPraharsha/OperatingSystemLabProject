# CPU Scheduling Algorithm Simulator

A terminal-based CPU scheduling simulation written in **C** that implements and visualizes **Lottery Scheduling** and **Rate-Monotonic Scheduling (RMS)** algorithms. The program takes dynamic user input, simulates the scheduling, and displays a **Gantt chart** directly in the terminal.

---

## Project Structure

```
main.c          - Entry point, user input, menu, and orchestration
algorithms.c    - Scheduling algorithms (Lottery, RMS) and Gantt chart printer
algorithms.h    - Header file with structs and function declarations
Makefile        - Build system (compile and run)
README.md       - This documentation file
```

---

## Algorithms

### 1. Lottery Scheduling

Lottery scheduling is a **probabilistic** CPU scheduling algorithm.

- Each process is assigned a number of **lottery tickets**.
- At every time unit, a random ticket is drawn.
- The process holding the winning ticket gets to use the CPU for that time unit.
- Processes with more tickets have a **higher probability** of being scheduled.
- The simulation continues until all processes finish their burst time or the simulation time runs out.

**Key Parameters:**

| Parameter    | Description                                              |
|-------------|----------------------------------------------------------|
| Burst Time  | Total CPU time the process needs                         |
| Arrival Time| When the process enters the ready queue                   |
| Tickets     | Number of lottery tickets (higher = more likely to run)   |

### 2. Rate-Monotonic Scheduling (RMS)

RMS is a **fixed-priority preemptive** scheduling algorithm designed for periodic tasks in real-time systems.

- Each process has a fixed **period** (how often it needs to run).
- Priority is assigned based on period: **shorter period = higher priority**.
- At each time unit, the ready process with the shortest period runs.
- If a higher-priority process becomes ready, it **preempts** the currently running process.
- Processes are **re-released** at the start of each new period with their original burst time.

**Key Parameters:**

| Parameter    | Description                                              |
|-------------|----------------------------------------------------------|
| Burst Time  | CPU time needed per period                                |
| Arrival Time| When the process first arrives                            |
| Period      | How often the process repeats (shorter = higher priority) |

---

## How to Build and Run

### Prerequisites
- **GCC** compiler (MinGW on Windows, or gcc on Linux/Mac)
- **Make** (optional, for using Makefile)

### Build
```bash
make
```

### Build and Run
```bash
make run
```

### Clean
```bash
make clean
```

### Manual Build (without Make)
```bash
gcc -Wall -g -o scheduler.exe main.c algorithms.c
./scheduler.exe
```

---

## Usage

When you run the program, it will:

1. Display a menu to choose an algorithm (Lottery, RMS, or Both)
2. Ask for the number of processes and simulation time
3. Ask for each process's details (burst time, arrival time, tickets/period)
4. Run the simulation and display:
   - Step-by-step execution log
   - Gantt chart showing process execution over time
   - Execution summary table

---

## Sample Test Runs

### Test 1: Lottery Scheduling (3 processes)

**Input:**
```
Choice: 1 (Lottery Scheduling)
Number of processes: 3
Total simulation time: 18

Process 1: Burst=6, Arrival=0, Tickets=10
Process 2: Burst=4, Arrival=0, Tickets=20
Process 3: Burst=5, Arrival=2, Tickets=15
```

**Output:**
```
  CPU Scheduling Algorithm Simulator
  Lottery & Rate-Monotonic Scheduling

--- Starting Lottery Scheduling ---
Time 0: Process P2 runs (Lottery)
Time 1: Process P2 runs (Lottery)
Time 2: Process P1 runs (Lottery)
Time 3: Process P2 runs (Lottery)
Time 4: Process P3 runs (Lottery)
Time 5: Process P2 runs (Lottery)
Time 6: Process P1 runs (Lottery)
Time 7: Process P3 runs (Lottery)
Time 8: Process P3 runs (Lottery)
Time 9: Process P3 runs (Lottery)
Time 10: Process P1 runs (Lottery)
Time 11: Process P3 runs (Lottery)
Time 12: Process P1 runs (Lottery)
Time 13: Process P1 runs (Lottery)
Time 14: Process P1 runs (Lottery)
Time 15: All processes finished.
--- Lottery Scheduling Done ---

-------------------------------------------
GANTT CHART: Lottery Scheduling
-------------------------------------------

  +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
  | P2 | P2 | P1 | P2 | P3 | P2 | P1 | P3 | P3 | P3 | P1 | P3 | P1 | P1 | P1 |
  +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
  0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15

Execution Summary:
+------+----------+
| Time | Process  |
+------+----------+
|   0  |   P2     |
|   1  |   P2     |
|   2  |   P1     |
|   3  |   P2     |
|   4  |   P3     |
|   5  |   P2     |
|   6  |   P1     |
|   7  |   P3     |
|   8  |   P3     |
|   9  |   P3     |
|  10  |   P1     |
|  11  |   P3     |
|  12  |   P1     |
|  13  |   P1     |
|  14  |   P1     |
+------+----------+

  Simulation Complete!
```

> **Note:** Lottery scheduling is randomized, so the output order of processes may vary between runs. P2 (20 tickets) gets scheduled more often early on due to higher ticket count.

---

### Test 2: RMS Scheduling (3 processes)

**Input:**
```
Choice: 2 (Rate-Monotonic Scheduling)
Number of processes: 3
Total simulation time: 20

Process 1: Burst=1, Arrival=0, Period=4
Process 2: Burst=2, Arrival=0, Period=6
Process 3: Burst=2, Arrival=0, Period=8
```

**Output:**
```
  CPU Scheduling Algorithm Simulator
  Lottery & Rate-Monotonic Scheduling

--- Starting RMS Scheduling ---
Time 0: Process P1 runs (RMS, period=4)
Time 1: Process P2 runs (RMS, period=6)
Time 2: Process P2 runs (RMS, period=6)
Time 3: Process P3 runs (RMS, period=8)
Time 4: Process P1 runs (RMS, period=4)
Time 5: Process P3 runs (RMS, period=8)
Time 6: Process P2 runs (RMS, period=6)
Time 7: Process P2 runs (RMS, period=6)
Time 8: Process P1 runs (RMS, period=4)
Time 9: Process P3 runs (RMS, period=8)
Time 10: Process P3 runs (RMS, period=8)
Time 11: CPU idle (RMS)
Time 12: Process P1 runs (RMS, period=4)
Time 13: Process P2 runs (RMS, period=6)
Time 14: Process P2 runs (RMS, period=6)
Time 15: CPU idle (RMS)
Time 16: Process P1 runs (RMS, period=4)
Time 17: Process P3 runs (RMS, period=8)
Time 18: Process P2 runs (RMS, period=6)
Time 19: Process P2 runs (RMS, period=6)
--- RMS Scheduling Done ---

-------------------------------------------
GANTT CHART: Rate-Monotonic Scheduling (RMS)
-------------------------------------------

  +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
  | P1 | P2 | P2 | P3 | P1 | P3 | P2 | P2 | P1 | P3 | P3 | -- | P1 | P2 | P2 | -- | P1 | P3 | P2 | P2 |
  +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
  0    1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18   19   20

Execution Summary:
+------+----------+
| Time | Process  |
+------+----------+
|   0  |   P1     |
|   1  |   P2     |
|   2  |   P2     |
|   3  |   P3     |
|   4  |   P1     |
|   5  |   P3     |
|   6  |   P2     |
|   7  |   P2     |
|   8  |   P1     |
|   9  |   P3     |
|  10  |   P3     |
|  11  |   IDLE   |
|  12  |   P1     |
|  13  |   P2     |
|  14  |   P2     |
|  15  |   IDLE   |
|  16  |   P1     |
|  17  |   P3     |
|  18  |   P2     |
|  19  |   P2     |
+------+----------+

  Simulation Complete!
```

**Key Observations (RMS):**
- P1 (period=4) has the highest priority and runs first in each cycle.
- P2 (period=6) has medium priority.
- P3 (period=8) has the lowest priority and fills remaining time slots.
- `--` (IDLE) slots appear when all periodic tasks have completed their burst for the current period.

---

### Test 3: Run Both Algorithms (3 processes)

**Input:**
```
Choice: 3 (Run Both)
Number of processes: 3
Total simulation time: 20

Process 1: Burst=1, Arrival=0, Tickets=10, Period=4
Process 2: Burst=2, Arrival=0, Tickets=20, Period=6
Process 3: Burst=2, Arrival=0, Tickets=15, Period=8
```

This mode runs both Lottery and RMS on the same set of processes, displaying two separate Gantt charts for comparison.

---

## Limitations

- Maximum 10 processes (configurable via `MAX_PROCESSES` in `algorithms.h`)
- Maximum 100 time units (configurable via `MAX_TIME` in `algorithms.h`)
- Lottery scheduling output is non-deterministic (random ticket draws)

---

## Authors

CPU Scheduling Algorithm Simulator - Educational project for understanding and visualizing scheduling algorithms.
