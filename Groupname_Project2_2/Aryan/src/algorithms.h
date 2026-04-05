#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "types.h"

void run_custom_rr(Process processes[], int num_processes, int num_cpus);
void run_edf(Process processes[], int num_processes, int num_cpus);
void run_mlfq(Process processes[], int num_processes, int num_cpus);
#endif