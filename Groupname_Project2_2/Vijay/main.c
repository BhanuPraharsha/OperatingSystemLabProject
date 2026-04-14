/*
 * main.c
 * Entry point for the CPU scheduling simulation.
 * Takes dynamic user input and runs Lottery or RMS scheduling.
 * Outputs a Gantt chart directly in the terminal.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "algorithms.h"

int main() {
    int choice;
    int num_processes;
    int total_time;
    int i;

    Process procs[MAX_PROCESSES];
    TraceEntry trace[MAX_TIME * 2];
    int trace_count = 0;

    /* Seed random number generator */
    srand((unsigned int)time(NULL));

    printf("\n");
    printf("  CPU Scheduling Algorithm Simulator\n");
    printf("  Lottery & Rate-Monotonic Scheduling\n");
    printf("\n\n");

    /* --- Algorithm Selection Menu --- */
    printf("Select the scheduling algorithm:\n");
    printf("  1. Lottery Scheduling\n");
    printf("  2. Rate-Monotonic Scheduling (RMS)\n");
    printf("  3. Run Both Algorithms\n");
    printf("Enter your choice (1/2/3): ");
    scanf("%d", &choice);

    if (choice < 1 || choice > 3) {
        printf("Invalid choice! Exiting.\n");
        return 1;
    }

    /* --- Get number of processes --- */
    printf("\nEnter number of processes (1 to %d): ", MAX_PROCESSES);
    scanf("%d", &num_processes);

    if (num_processes < 1 || num_processes > MAX_PROCESSES) {
        printf("Invalid number of processes! Must be between 1 and %d.\n", MAX_PROCESSES);
        return 1;
    }

    /* --- Get total simulation time --- */
    printf("Enter total simulation time: ");
    scanf("%d", &total_time);

    if (total_time < 1 || total_time > MAX_TIME) {
        printf("Invalid simulation time! Must be between 1 and %d.\n", MAX_TIME);
        return 1;
    }

    /* --- Get process details from user --- */
    printf("\nEnter process details:\n");
    printf("-----------------------------------\n");

    for (i = 0; i < num_processes; i++) {
        printf("\nProcess %d:\n", i + 1);

        procs[i].pid = i + 1;

        printf("  Burst Time     : ");
        scanf("%d", &procs[i].burst_time);

        printf("  Arrival Time   : ");
        scanf("%d", &procs[i].arrival_time);

        if (choice == 1 || choice == 3) {
            printf("  Tickets        : ");
            scanf("%d", &procs[i].tickets);
        } else {
            procs[i].tickets = 0;
        }

        if (choice == 2 || choice == 3) {
            printf("  Period         : ");
            scanf("%d", &procs[i].period);
        } else {
            procs[i].period = 0;
        }

        procs[i].remaining_time = procs[i].burst_time;
    }

    printf("\n");

    /* =========================================== */
    /*       RUN THE SELECTED ALGORITHM(S)         */
    /* =========================================== */

    if (choice == 1 || choice == 3) {
        /* --- Lottery Scheduling --- */
        Process lottery_copy[MAX_PROCESSES];
        int lottery_trace_start = trace_count;

        for (i = 0; i < num_processes; i++) {
            lottery_copy[i] = procs[i];
            lottery_copy[i].remaining_time = procs[i].burst_time;
        }

        lottery_scheduling(lottery_copy, num_processes, trace, &trace_count, total_time);

        /* Print Gantt chart for Lottery results */
        print_gantt_chart(trace + lottery_trace_start, trace_count - lottery_trace_start, "Lottery Scheduling");
    }

    if (choice == 2 || choice == 3) {
        /* --- RMS Scheduling --- */
        Process rms_copy[MAX_PROCESSES];
        int rms_trace_start = trace_count;

        for (i = 0; i < num_processes; i++) {
            rms_copy[i] = procs[i];
            rms_copy[i].remaining_time = rms_copy[i].burst_time;
        }

        rms_scheduling(rms_copy, num_processes, trace, &trace_count, total_time);

        /* Print Gantt chart for RMS results */
        print_gantt_chart(trace + rms_trace_start, trace_count - rms_trace_start, "Rate-Monotonic Scheduling (RMS)");
    }

    /* --- Final Summary --- */
    printf("\n");
    printf("  Simulation Complete!\n");
    printf("\n");

    return 0;
}
