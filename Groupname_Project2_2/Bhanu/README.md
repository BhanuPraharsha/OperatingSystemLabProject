# CPU Scheduling: EDF and MLFQ

This folder contains my implementation of two CPU scheduling algorithms: **Earliest Deadline First (EDF)** and **Multi-Level Feedback Queue (MLFQ)**. These are designed to work with 2 CPUs simultaneously.

---

## 1. Earliest Deadline First (EDF)

The idea here is simple: always run the job that needs to be finished first.

- **How it picks jobs**: Every single clock cycle, the scheduler looks at all the ready processes and picks the one with the closest deadline.
- **Preemption**: Since deadlines can change (or new jobs can arrive), a job currently running on a CPU might be stopped if a new job arrives with an even earlier deadline.
- **Missing Deadlines**: If a process finishes after its deadline, the program logs it as a "MISS". At the end, you get a summary showing how many jobs actually finished on time.

---

## 2. Multi-Level Feedback Queue (MLFQ)

MLFQ is all about fairness. It tries to finish small tasks quickly while making sure long-running tasks don't hog all the CPU time.

- **The Queues**: We have 3 different priority levels. 
  - **Queue 0**: High priority, 4 cycle limit. New jobs start here.
  - **Queue 1**: Medium priority, 8 cycle limit.
  - **Queue 2**: Low priority (runs until finished or bumped).
- **Moving Down**: If a job takes too long in a high-priority queue, it gets "demoted" to the next level down.
- **Priority Boost**: Every 30 cycles, we reset everyone back to the top queue (Queue 0). This prevents jobs in the bottom queue from getting stuck forever.

---

## How to Run it

Just use the Makefile to compile and start the simulation:

```bash
make
./scheduler_sim
```

It will run through both algorithms and print a table showing the turnaround time, waiting time, and overall results.
