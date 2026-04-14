/*
 * algorithms.c
 * Contains the actual simulation logic for Lottery and RMS scheduling.
 * Also has the Gantt chart printing function for terminal output.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algorithms.h"

/*
 * Lottery Scheduling Simulation
 * ---------------------------------
 * How it works:
 * - Each process has some "tickets".
 * - At every time unit, we pick a random ticket number.
 * - Whichever process holds that ticket gets to run.
 * - We keep doing this until all processes finish or time runs out.
 */
void lottery_scheduling(Process procs[], int n, TraceEntry trace[], int *trace_count, int total_time) {
    int time;
    int i;
    int total_tickets;
    int winner;
    int running;
    int found;

    printf("\n--- Starting Lottery Scheduling ---\n");

    for (time = 0; time < total_time; time++) {

        /* First, count the total tickets of all processes that still have work left */
        total_tickets = 0;
        for (i = 0; i < n; i++) {
            if (procs[i].remaining_time > 0 && procs[i].arrival_time <= time) {
                total_tickets += procs[i].tickets;
            }
        }

        /* If no process has any remaining time, we are done */
        if (total_tickets == 0) {
            printf("Time %d: All processes finished.\n", time);
            break;
        }

        /* Pick a random ticket number between 0 and total_tickets - 1 */
        running = rand() % total_tickets;

        /* Find which process owns that ticket */
        found = -1;
        winner = 0;
        for (i = 0; i < n; i++) {
            if (procs[i].remaining_time > 0 && procs[i].arrival_time <= time) {
                winner += procs[i].tickets;
                if (running < winner) {
                    found = i;
                    break;
                }
            }
        }

        /* Run the winning process for 1 time unit */
        if (found != -1) {
            procs[found].remaining_time--;
            printf("Time %d: Process P%d runs (Lottery)\n", time, procs[found].pid);

            /* Save this to the trace */
            trace[*trace_count].time = time;
            trace[*trace_count].pid = procs[found].pid;
            strcpy(trace[*trace_count].algorithm, "Lottery");
            (*trace_count)++;
        }
    }

    printf("--- Lottery Scheduling Done ---\n\n");
}

/*
 * Rate-Monotonic Scheduling (RMS) Simulation
 * -----------------------------------------------
 * How it works:
 * - Each process has a "period". Shorter period = higher priority.
 * - At each time unit, we check which processes are ready to run.
 * - We pick the one with the shortest period.
 * - If a process with a shorter period arrives, it preempts the current one.
 * - Processes get a new burst every time their period starts again.
 */
void rms_scheduling(Process procs[], int n, TraceEntry trace[], int *trace_count, int total_time) {
    int time;
    int i;
    int selected;
    int shortest_period;

    /* Arrays to track deadline and remaining time for periodic tasks */
    int next_deadline[MAX_PROCESSES];
    int remaining[MAX_PROCESSES];

    printf("--- Starting RMS Scheduling ---\n");

    /* Initialize deadlines and remaining times */
    for (i = 0; i < n; i++) {
        next_deadline[i] = procs[i].arrival_time + procs[i].period;
        remaining[i] = procs[i].burst_time;
    }

    for (time = 0; time < total_time; time++) {

        /* Check if any process needs to be "released" again (new period starts) */
        for (i = 0; i < n; i++) {
            if (time >= next_deadline[i] && procs[i].arrival_time <= time) {
                /* New period starts, reset the burst */
                remaining[i] = procs[i].burst_time;
                next_deadline[i] += procs[i].period;
            }
        }

        /* Find the ready process with the shortest period (highest priority) */
        selected = -1;
        shortest_period = 99999;

        for (i = 0; i < n; i++) {
            if (remaining[i] > 0 && procs[i].arrival_time <= time) {
                if (procs[i].period < shortest_period) {
                    shortest_period = procs[i].period;
                    selected = i;
                }
            }
        }

        /* If we found a process to run, execute it for 1 time unit */
        if (selected != -1) {
            remaining[selected]--;
            printf("Time %d: Process P%d runs (RMS, period=%d)\n", time, procs[selected].pid, procs[selected].period);

            /* Save to trace */
            trace[*trace_count].time = time;
            trace[*trace_count].pid = procs[selected].pid;
            strcpy(trace[*trace_count].algorithm, "RMS");
            (*trace_count)++;
        } else {
            /* CPU is idle at this time */
            printf("Time %d: CPU idle (RMS)\n", time);
            trace[*trace_count].time = time;
            trace[*trace_count].pid = -1;  /* -1 means idle */
            strcpy(trace[*trace_count].algorithm, "RMS");
            (*trace_count)++;
        }

        /* RMS runs for the full simulation time (periodic tasks keep re-releasing) */
    }

    printf("--- RMS Scheduling Done ---\n\n");
}

/*
 * Print Gantt Chart in Terminal
 * ----------------------------------
 * Draws a simple text-based Gantt chart directly in the terminal.
 * Shows which process ran at each time unit.
 */
void print_gantt_chart(TraceEntry trace[], int trace_count, const char *title) {
    int i;
    int max_time = 0;

    if (trace_count == 0) {
        printf("(No trace data to display)\n");
        return;
    }

    /* Find the maximum time value */
    for (i = 0; i < trace_count; i++) {
        if (trace[i].time > max_time) {
            max_time = trace[i].time;
        }
    }

    /* Print chart title */
    printf("-------------------------------------------\n");
    printf("GANTT CHART: %s\n", title);
    printf("-------------------------------------------\n\n");

    /* --- Row 1: Top border --- */
    printf("  ");
    for (i = 0; i < trace_count; i++) {
        printf("+----");
    }
    printf("+\n");

    /* --- Row 2: Process blocks --- */
    printf("  ");
    for (i = 0; i < trace_count; i++) {
        if (trace[i].pid == -1) {
            printf("| -- ");
        } else {
            printf("| P%-2d", trace[i].pid);
        }
    }
    printf("|\n");

    /* --- Row 3: Bottom border --- */
    printf("  ");
    for (i = 0; i < trace_count; i++) {
        printf("+----");
    }
    printf("+\n");

    /* --- Row 4: Time axis labels --- */
    printf("  ");
    for (i = 0; i < trace_count; i++) {
        printf("%-5d", trace[i].time);
    }
    printf("%d\n", max_time + 1);

    /* --- Print Summary Table --- */
    printf("\nExecution Summary:\n");
    printf("+------+----------+\n");
    printf("| Time | Process  |\n");
    printf("+------+----------+\n");
    for (i = 0; i < trace_count; i++) {
        if (trace[i].pid == -1) {
            printf("| %3d  |   IDLE   |\n", trace[i].time);
        } else {
            printf("| %3d  |   P%-2d    |\n", trace[i].time, trace[i].pid);
        }
    }
    printf("+------+----------+\n\n");
}
