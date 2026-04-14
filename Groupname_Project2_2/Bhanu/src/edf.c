#include "edf.h"
#include <stdio.h>

// returns index of READY process with earliest deadline that has arrived, or -1
static int pick_earliest_deadline(Process p[], int n, int cycle) {
  int chosen = -1, earliest = 100000;
  for (int i = 0; i < n; i++) {
    if (p[i].state != READY || p[i].arrival_time > cycle)
      continue;
    if (p[i].deadline < earliest) {
      earliest = p[i].deadline;
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
  printf("\n%-5s %-8s %-8s %-10s %-12s %-12s %-10s %-6s\n", "PID", "Arrival",
         "Burst", "Deadline", "Completion", "Turnaround", "Waiting", "Miss?");
  printf("---------------------------------------------------------------------"
         "--\n");
  double t_sum = 0, w_sum = 0;
  int misses = 0;
  for (int i = 0; i < n; i++) {
    int tat = p[i].completion_time - p[i].arrival_time;
    int wt = tat - p[i].burst_time;
    int missed = p[i].completion_time > p[i].deadline;
    t_sum += tat;
    w_sum += wt;
    if (missed)
      misses++;
    printf("%-5d %-8d %-8d %-10d %-12d %-12d %-10d %-6s\n", p[i].id,
           p[i].arrival_time, p[i].burst_time, p[i].deadline,
           p[i].completion_time, tat, wt, missed ? "MISS" : "ok");
  }
  printf("---------------------------------------------------------------------"
         "--\n");
  printf(
      "Avg Turnaround: %.2f  |  Avg Waiting: %.2f  |  Deadline Misses: %d/%d\n",
      t_sum / n, w_sum / n, misses, n);
}

void run_edf(Process p[], int np, int cpus) {
  printf("\n--- Earliest Deadline First ---\n");
  printf("Preemptive: each clock cycle, CPUs pick the job with the nearest "
         "deadline\n");
  printf("Cycle\t");
  for (int i = 0; i < cpus; i++)
    printf("CPU%d\t", i);
  printf("\n--------------------------------\n");

  CPU cpu[cpus];
  for (int i = 0; i < cpus; i++) {
    cpu[i] = (CPU){i, -1, 0, 0};
  }

  int cycle = 0, finished = 0;

  while (finished < np) {
    int best = pick_earliest_deadline(p, np, cycle);

    for (int i = 0; i < cpus && best != -1; i++) {
      int running = cpu[i].current_process_id;

      if (running == -1) {
        cpu[i].current_process_id = best;
        p[best].state = RUNNING;
        p[best].cpu_id = i;
        if (p[best].start_time == -1)
          p[best].start_time = cycle;
        best = pick_earliest_deadline(p, np, cycle); // next best for next CPU
      } else if (p[best].deadline < p[running].deadline) {
        p[running].state = READY;
        cpu[i].current_process_id = best;
        p[best].state = RUNNING;
        p[best].cpu_id = i;
        if (p[best].start_time == -1)
          p[best].start_time = cycle;
        best = pick_earliest_deadline(p, np, cycle);
      }
    }

    show_row(cycle, cpu, cpus, p);

    // advance one clock cycle
    for (int i = 0; i < cpus; i++) {
      int idx = cpu[i].current_process_id;
      if (idx == -1)
        continue;
      p[idx].remaining_time--;
      if (p[idx].remaining_time == 0) {
        p[idx].state = FINISHED;
        p[idx].completion_time = cycle + 1;
        cpu[i].current_process_id = -1;
        finished++;
        if (p[idx].completion_time > p[idx].deadline)
          printf("  [MISS] P%d finished at cycle %d, deadline was %d\n",
                 p[idx].id, p[idx].completion_time, p[idx].deadline);
      }
    }
    cycle++;
  }

  show_results(p, np);
}
