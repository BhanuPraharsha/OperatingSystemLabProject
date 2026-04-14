#ifndef STATS_H
#define STATS_H
#include "types.h"
void print_process_stats(Process processes[], int num_processes);
void print_gantt_chart(int time, CPU cpus[], int num_cpus, Process processes[]);
#endif