#ifndef TYPES_H
#define TYPES_H

typedef enum { READY, RUNNING, FINISHED } State;

typedef struct {
  int id;
  int arrival_time;
  int burst_time;
  int remaining_time;
  int deadline;
  int priority_level; // MLFQ queue level
  State state;

  // metrics
  int completion_time;
  int start_time;
  int cpu_id;
  int wait_time_acc; // accumulated wait cycles
} Process;

typedef struct {
  int id;
  int current_process_id;
  int time_quantum_left;
  int busy_ticks; // total cycles this CPU was active
} CPU;

#endif
