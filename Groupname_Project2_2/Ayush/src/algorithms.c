#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "types.h"

// ============================================================
//  Helper: print Gantt chart row
// ============================================================

void print_gantt_chart(int time, CPU cpus[], int num_cpus, Process processes[]) {
    printf("%02d\t", time);
    for (int i = 0; i < num_cpus; i++) {
        if (cpus[i].current_process_id != -1) {
            printf("[P%d]\t", processes[cpus[i].current_process_id].id);
        } else {
            printf("[--]\t");
        }
    }
    printf("\n");
}

// ============================================================
//  Helper: print per-process metrics after scheduling completes
// ============================================================

void print_metrics(Process processes[], int num_processes) {
    printf("\n--- Performance Metrics ---\n");
    printf("PID\tArrival\tBurst\tStart\tFinish\tTurnaround\tWaiting\n");
    printf("---\t-------\t-----\t-----\t------\t----------\t-------\n");

    float total_tat = 0, total_wt = 0;
    for (int i = 0; i < num_processes; i++) {
        int tat = processes[i].completion_time - processes[i].arrival_time;
        int wt  = tat - processes[i].burst_time;
        total_tat += tat;
        total_wt  += wt;
        printf("P%d\t%d\t%d\t%d\t%d\t%d\t\t%d\n",
               processes[i].id,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].start_time,
               processes[i].completion_time,
               tat, wt);
    }
    printf("\nAvg Turnaround Time : %.2f\n", total_tat / num_processes);
    printf("Avg Waiting Time    : %.2f\n", total_wt  / num_processes);
}

// ============================================================
//  Algorithm A – Lottery Scheduling
// ============================================================
//
//  Each process holds a number of "tickets".  At every scheduling
//  decision point (when a CPU becomes idle or a quantum expires)
//  a winning ticket is drawn uniformly at random from the pool of
//  all READY processes.  The process that owns the winning ticket
//  is dispatched.  A fixed time-quantum of 4 ticks is used so that
//  lottery draws happen periodically (preemptive lottery).
//
// ============================================================

void run_lottery(Process processes[], int num_processes, int num_cpus) {
    printf("\n--- Starting Lottery Scheduling (Quantum = 4) ---\n");
    printf("Time\t");
    for (int c = 0; c < num_cpus; c++) {
        printf("CPU %d\t", c);
    }
    printf("\n---------------------------------\n");

    CPU cpus[num_cpus];
    for (int i = 0; i < num_cpus; i++) {
        cpus[i].id = i;
        cpus[i].current_process_id = -1;
        cpus[i].time_quantum_left  = 0;
    }

    int current_time    = 0;
    int completed_count = 0;
    int time_quantum    = 4;

    srand((unsigned int)time(NULL));

    while (completed_count < num_processes) {

        // ---- 1. Pick a winner for every idle CPU ----
        for (int i = 0; i < num_cpus; i++) {
            if (cpus[i].current_process_id != -1)
                continue;   // CPU busy

            // Sum tickets of all READY processes that have arrived
            int total_tickets = 0;
            for (int p = 0; p < num_processes; p++) {
                if (processes[p].state == READY &&
                    processes[p].arrival_time <= current_time) {
                    total_tickets += processes[p].tickets;
                }
            }

            if (total_tickets == 0)
                continue;   // no eligible process

            // Draw a winning ticket  [1 .. total_tickets]
            int winner = (rand() % total_tickets) + 1;
            int cumulative = 0;

            for (int p = 0; p < num_processes; p++) {
                if (processes[p].state == READY &&
                    processes[p].arrival_time <= current_time) {
                    cumulative += processes[p].tickets;
                    if (cumulative >= winner) {
                        // Dispatch this process
                        cpus[i].current_process_id = p;
                        processes[p].state  = RUNNING;
                        processes[p].cpu_id = i;
                        cpus[i].time_quantum_left = time_quantum;

                        if (processes[p].start_time == -1) {
                            processes[p].start_time = current_time;
                        }
                        break;
                    }
                }
            }
        }

        // ---- 2. Print Gantt row ----
        print_gantt_chart(current_time, cpus, num_cpus, processes);

        // ---- 3. Execute one tick on every busy CPU ----
        for (int i = 0; i < num_cpus; i++) {
            if (cpus[i].current_process_id == -1)
                continue;

            int pid = cpus[i].current_process_id;
            processes[pid].remaining_time--;
            cpus[i].time_quantum_left--;

            // Process finished
            if (processes[pid].remaining_time == 0) {
                processes[pid].state           = FINISHED;
                processes[pid].completion_time = current_time + 1;
                cpus[i].current_process_id     = -1;
                completed_count++;
            }
            // Quantum expired – put back in READY pool
            else if (cpus[i].time_quantum_left == 0) {
                processes[pid].state       = READY;
                cpus[i].current_process_id = -1;
            }
        }

        current_time++;
    }

    print_metrics(processes, num_processes);
}

// ============================================================
//  Algorithm B – Rate Monotonic Scheduling (RMS)
// ============================================================
//
//  RMS is a fixed-priority preemptive algorithm for periodic
//  real-time tasks.  Priority is assigned inversely to the task
//  period: shorter period  →  higher priority (lower numerical
//  value of `period`).
//
//  At every tick the scheduler checks all CPUs.  If a higher-
//  priority READY task exists, it preempts the currently running
//  task.  A task that finishes its burst before the next period
//  goes to FINISHED.
//
//  For simplicity, each process runs for a single activation
//  (arrival → burst completion) rather than being periodically
//  re-released, making this a single-shot RMS demonstration.
//
// ============================================================

void run_rms(Process processes[], int num_processes, int num_cpus) {
    printf("\n--- Starting Rate Monotonic Scheduling (RMS) ---\n");
    printf("Priority = shortest period first (preemptive)\n");
    printf("Time\t");
    for (int c = 0; c < num_cpus; c++) {
        printf("CPU %d\t", c);
    }
    printf("\n---------------------------------\n");

    CPU cpus[num_cpus];
    for (int i = 0; i < num_cpus; i++) {
        cpus[i].id = i;
        cpus[i].current_process_id = -1;
        cpus[i].time_quantum_left  = 0;
    }

    int current_time    = 0;
    int completed_count = 0;

    while (completed_count < num_processes) {

        // ---- 1. Build a sorted list of READY processes by period (ascending) ----
        //         Shortest period = highest priority in RMS.

        int ready_order[num_processes];
        int ready_count = 0;

        for (int p = 0; p < num_processes; p++) {
            if (processes[p].state == READY &&
                processes[p].arrival_time <= current_time) {
                ready_order[ready_count++] = p;
            }
        }

        // Simple insertion sort by period (ascending)
        for (int a = 1; a < ready_count; a++) {
            int key = ready_order[a];
            int b   = a - 1;
            while (b >= 0 && processes[ready_order[b]].period > processes[key].period) {
                ready_order[b + 1] = ready_order[b];
                b--;
            }
            ready_order[b + 1] = key;
        }

        // ---- 2. Preemption check on busy CPUs ----
        //         If a ready task has a shorter period than the
        //         currently running task, preempt.

        for (int r = 0; r < ready_count; r++) {
            int candidate = ready_order[r];

            // Find the CPU running the lowest-priority (longest period) task
            int worst_cpu  = -1;
            int worst_period = -1;

            for (int i = 0; i < num_cpus; i++) {
                if (cpus[i].current_process_id == -1) {
                    // Idle CPU — best target, use it immediately
                    worst_cpu = i;
                    worst_period = __INT_MAX__;
                    break;
                }
                int running_pid = cpus[i].current_process_id;
                if (processes[running_pid].period > worst_period) {
                    worst_period = processes[running_pid].period;
                    worst_cpu    = i;
                }
            }

            if (worst_cpu == -1)
                break;   // all CPUs busy, could not find one

            // If CPU is idle, just assign
            if (cpus[worst_cpu].current_process_id == -1) {
                cpus[worst_cpu].current_process_id = candidate;
                processes[candidate].state  = RUNNING;
                processes[candidate].cpu_id = worst_cpu;
                if (processes[candidate].start_time == -1) {
                    processes[candidate].start_time = current_time;
                }
            }
            // If candidate has strictly shorter period → preempt
            else {
                int running_pid = cpus[worst_cpu].current_process_id;
                if (processes[candidate].period < processes[running_pid].period) {
                    processes[running_pid].state = READY;
                    cpus[worst_cpu].current_process_id = candidate;
                    processes[candidate].state  = RUNNING;
                    processes[candidate].cpu_id = worst_cpu;
                    if (processes[candidate].start_time == -1) {
                        processes[candidate].start_time = current_time;
                    }
                }
            }
        }

        // ---- 3. Assign remaining READY tasks to idle CPUs ----
        //         (handles the case where no preemption occurred but CPUs are free)

        for (int i = 0; i < num_cpus; i++) {
            if (cpus[i].current_process_id != -1)
                continue;

            int best_p      = -1;
            int best_period = __INT_MAX__;

            for (int p = 0; p < num_processes; p++) {
                if (processes[p].state == READY &&
                    processes[p].arrival_time <= current_time &&
                    processes[p].period < best_period) {
                    best_period = processes[p].period;
                    best_p      = p;
                }
            }

            if (best_p != -1) {
                cpus[i].current_process_id = best_p;
                processes[best_p].state  = RUNNING;
                processes[best_p].cpu_id = i;
                if (processes[best_p].start_time == -1) {
                    processes[best_p].start_time = current_time;
                }
            }
        }

        // ---- 4. Print Gantt row ----
        print_gantt_chart(current_time, cpus, num_cpus, processes);

        // ---- 5. Execute one tick ----
        for (int i = 0; i < num_cpus; i++) {
            if (cpus[i].current_process_id == -1)
                continue;

            int pid = cpus[i].current_process_id;
            processes[pid].remaining_time--;

            if (processes[pid].remaining_time == 0) {
                processes[pid].state           = FINISHED;
                processes[pid].completion_time = current_time + 1;
                cpus[i].current_process_id     = -1;
                completed_count++;
            }
        }

        current_time++;
    }

    print_metrics(processes, num_processes);
}
