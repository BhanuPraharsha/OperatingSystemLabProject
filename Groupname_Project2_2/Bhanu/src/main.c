#include "algorithms.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>

#define NUM_PROCESSES 20
#define NUM_CPUS 2

static void reset_processes(Process procs[], int n) {
  Process initial[NUM_PROCESSES] = {
      /***  id  arr_time  burst_time  rem_time  deadline   priority  state
         completion_time  start_time   cpu_id ***/
      {1, 0, 8, 8, 15, 0, READY, 0, -1, -1, 0},
      {2, 1, 25, 25, 60, 0, READY, 0, -1, -1, 0},
      {3, 2, 3, 3, 10, 0, READY, 0, -1, -1, 0},
      {4, 5, 12, 12, 30, 0, READY, 0, -1, -1, 0},
      {5, 8, 2, 2, 12, 0, READY, 0, -1, -1, 0},
      {6, 10, 40, 40, 100, 0, READY, 0, -1, -1, 0},
      {7, 12, 5, 5, 25, 0, READY, 0, -1, -1, 0},
      {8, 15, 6, 6, 30, 0, READY, 0, -1, -1, 0},
      {9, 20, 15, 15, 50, 0, READY, 0, -1, -1, 0},
      {10, 22, 4, 4, 30, 0, READY, 0, -1, -1, 0},
      {11, 25, 8, 8, 40, 0, READY, 0, -1, -1, 0},
      {12, 30, 50, 50, 150, 0, READY, 0, -1, -1, 0},
      {13, 35, 2, 2, 40, 0, READY, 0, -1, -1, 0},
      {14, 40, 9, 9, 60, 0, READY, 0, -1, -1, 0},
      {15, 45, 3, 3, 50, 0, READY, 0, -1, -1, 0},
      {16, 50, 18, 18, 80, 0, READY, 0, -1, -1, 0},
      {17, 55, 5, 5, 65, 0, READY, 0, -1, -1, 0},
      {18, 60, 7, 7, 75, 0, READY, 0, -1, -1, 0},
      {19, 65, 22, 22, 100, 0, READY, 0, -1, -1, 0},
      {20, 70, 4, 4, 80, 0, READY, 0, -1, -1, 0},
  };
  for (int i = 0; i < n; i++)
    procs[i] = initial[i];
}

int main(void) {
  Process processes[NUM_PROCESSES];
  int choice;

  printf("╔══════════════════════════════════════════════╗\n");
  printf("║      Multiprocessor Scheduling Simulator     ║\n");
  printf("╚══════════════════════════════════════════════╝\n");
  printf("CPUs: %d  |  Processes: %d\n\n", NUM_CPUS, NUM_PROCESSES);

  while (1) {
    printf("Select a scheduling algorithm:\n");
    printf("  1. Multilevel Feedback Queue (MLFQ)\n");
    printf("  2. Earliest Deadline First   (EDF)\n");
    printf("  3. Exit\n");
    printf("Choice: ");

    if (scanf("%d", &choice) != 1) {
      choice = 3;
    }

    switch (choice) {
    case 1:
      reset_processes(processes, NUM_PROCESSES);
      run_mlfq(processes, NUM_PROCESSES, NUM_CPUS);
      break;
    case 2:
      reset_processes(processes, NUM_PROCESSES);
      run_edf(processes, NUM_PROCESSES, NUM_CPUS);
      break;
    case 3:
      printf("\nExiting simulator...\n");
      return 0;
    default:
      printf("Invalid choice. Please enter 1, 2, or 3.\n\n");
    }
  }
}
