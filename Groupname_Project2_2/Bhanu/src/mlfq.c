#include "mlfq.h"
#include <stdio.h>

#define LEVELS 3
#define BOOST_CYCLE 30
static const int TQ[LEVELS] = {4, 8, 100000}; // q2 = FCFS
// returns index of highest-priority READY process that has arrived, or -1
static int pick_by_queue(Process p[], int n, int curr) {
  int chosen = -1, best_level = LEVELS;
  for (int i = 0; i < n; i++) {
    if (p[i].state != READY || p[i].arrival_time > curr)
      continue;
    if (p[i].priority_level < best_level) {
      best_level = p[i].priority_level;
      chosen = i;
    }
  }
  return chosen;
}

static void show_row(int cycle, CPU cpu[], int cpus, Process p[]) {
  printf("%02d\t", cycle);
  for (int i = 0; i < cpus; i++) {
    int idx = cpu[i].current_process_id;
    printf(idx == -1 ? "[--]\t" : "[P%d]\t", idx == -1 ? 0 : p[idx].id);
  }
  printf("\n");
}

static void show_results(Process p[], int n) {
  printf("\n%-5s %-8s %-8s %-12s %-12s %-10s\n", "PID", "Arrival", "Burst",
         "Completion", "Turnaround", "Waiting");
  printf("---------------------------------------------------\n");
  double t_sum = 0, w_sum = 0;
  for (int i = 0; i < n; i++) {
    int tat = p[i].completion_time - p[i].arrival_time;
    int wt = tat - p[i].burst_time;
    t_sum += tat;
    w_sum += wt;
    printf("%-5d %-8d %-8d %-12d %-12d %-10d\n", p[i].id, p[i].arrival_time,
           p[i].burst_time, p[i].completion_time, tat, wt);
  }
  printf("---------------------------------------------------\n");
  printf("Avg Turnaround: %.2f  |  Avg Waiting: %.2f\n", t_sum / n, w_sum / n);
}

void run_mlfq(Process p[], int np, int cpus) {
  printf("\n--- Multilevel Feedback Queue (MLFQ) ---\n");
  printf(
      "Q0 quantum=%d | Q1 quantum=%d | Q2=FCFS | Boost every %d clock cycles\n",
      TQ[0], TQ[1], BOOST_CYCLE);
  printf("Cycle\t");
  for (int i = 0; i < cpus; i++)
    printf("CPU%d\t", i);
  printf("\n--------------------------------\n");

  CPU cpu[cpus];
  for (int i = 0; i < cpus; i++) {
    cpu[i] = (CPU){i, -1, 0, 0};
  }
  for (int i = 0; i < np; i++)
    p[i].priority_level = 0;

  int cycle = 0, finished = 0;

  while (finished < np) {
    // periodic priority boost
    if (cycle > 0 && cycle % BOOST_CYCLE == 0) {
      printf("** Boost at cycle=%d: all reset to Q0 **\n", cycle);
      for (int i = 0; i < np; i++) {
        if (p[i].state == READY || p[i].state == RUNNING) {
          p[i].priority_level = 0;
          if (p[i].state == RUNNING)
            cpu[p[i].cpu_id].time_quantum_left = TQ[0];
        }
      }
    }

    // assigning idle CPUs and handling preemption
    for (int i = 0; i < cpus; i++) {
      int best = pick_by_queue(p, np, cycle);
      if (best == -1)
        continue;

      int running = cpu[i].current_process_id;
      if (running == -1) {
        cpu[i].current_process_id = best;
        cpu[i].time_quantum_left = TQ[p[best].priority_level];
        p[best].state = RUNNING;
        p[best].cpu_id = i;
        if (p[best].start_time == -1)
          p[best].start_time = cycle;
      } else if (p[best].priority_level < p[running].priority_level) {
        p[running].state = READY;
        cpu[i].current_process_id = best;
        cpu[i].time_quantum_left = TQ[p[best].priority_level];
        p[best].state = RUNNING;
        p[best].cpu_id = i;
        if (p[best].start_time == -1)
          p[best].start_time = cycle;
      }
    }

    show_row(cycle, cpu, cpus, p);

    // advance one clock cycle
    for (int i = 0; i < cpus; i++) {
      int idx = cpu[i].current_process_id;
      if (idx == -1)
        continue;
      p[idx].remaining_time--;
      if (p[idx].priority_level < LEVELS - 1)
        cpu[i].time_quantum_left--;

      if (p[idx].remaining_time == 0) {
        p[idx].state = FINISHED;
        p[idx].completion_time = cycle + 1;
        cpu[i].current_process_id = -1;
        finished++;
      } else if (p[idx].priority_level < LEVELS - 1 &&
                 cpu[i].time_quantum_left == 0) {
        p[idx].priority_level++;
        p[idx].state = READY;
        cpu[i].current_process_id = -1;
      }
    }
    cycle++;
  }

  show_results(p, np);
}
