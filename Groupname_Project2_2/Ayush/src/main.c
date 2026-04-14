#include <stdio.h>
#include <stdlib.h>
#include "algorithms.h"
#include "types.h"

// ============================================================
//  Test data — 20 processes with tickets (lottery) and
//  periods (RMS).  Fields:
//    id, arrival, burst, remaining, period, tickets, priority,
//    state, completion, start, cpu_id
// ============================================================

void reset_processes(Process processes[], int n) {
    Process initial[20] = {
        // Initial batch
        { 1,   0,   8,  8,  20,  10, 0, READY, 0, -1, -1},
        { 2,   1,  25, 25,  80,   5, 0, READY, 0, -1, -1},
        { 3,   2,   3,  3,  10,  20, 0, READY, 0, -1, -1},
        { 4,   5,  12, 12,  40,   8, 0, READY, 0, -1, -1},
        { 5,   8,   2,  2,  15,  25, 0, READY, 0, -1, -1},

        // Mid-simulation arrivals
        { 6,  10,  40, 40, 120,   3, 0, READY, 0, -1, -1},
        { 7,  12,   5,  5,  25,  15, 0, READY, 0, -1, -1},
        { 8,  15,   6,  6,  30,  12, 0, READY, 0, -1, -1},
        { 9,  20,  15, 15,  50,   7, 0, READY, 0, -1, -1},
        {10,  22,   4,  4,  20,  18, 0, READY, 0, -1, -1},

        // Late arrivals
        {11,  25,   8,  8,  35,  10, 0, READY, 0, -1, -1},
        {12,  30,  50, 50, 150,   2, 0, READY, 0, -1, -1},
        {13,  35,   2,  2,  12,  30, 0, READY, 0, -1, -1},
        {14,  40,   9,  9,  45,   9, 0, READY, 0, -1, -1},
        {15,  45,   3,  3,  18,  22, 0, READY, 0, -1, -1},

        // Very late arrivals
        {16,  50,  18, 18,  70,   6, 0, READY, 0, -1, -1},
        {17,  55,   5,  5,  22,  16, 0, READY, 0, -1, -1},
        {18,  60,   7,  7,  30,  11, 0, READY, 0, -1, -1},
        {19,  65,  22, 22, 100,   4, 0, READY, 0, -1, -1},
        {20,  70,   4,  4,  16,  20, 0, READY, 0, -1, -1}
    };

    for (int i = 0; i < n; i++) {
        processes[i] = initial[i];
    }
}

int main() {
    int num_processes = 20;
    int num_cpus      = 2;
    Process processes[20];

    int choice;

    printf("==========================================\n");
    printf("  Multiprocessor Scheduling Simulator\n");
    printf("  Algorithms: Lottery | RMS\n");
    printf("==========================================\n");
    printf("  CPUs: %d  |  Processes: %d\n", num_cpus, num_processes);
    printf("==========================================\n");

    while (1) {
        reset_processes(processes, num_processes);

        printf("\nSelect a scheduling algorithm:\n");
        printf("  1. Lottery Scheduling\n");
        printf("  2. Rate Monotonic Scheduling (RMS)\n");
        printf("  3. Exit\n");
        printf("Enter your choice: ");

        scanf("%d", &choice);

        switch (choice) {
            case 1:
                run_lottery(processes, num_processes, num_cpus);
                break;
            case 2:
                run_rms(processes, num_processes, num_cpus);
                break;
            case 3:
                printf("\nExiting the simulator.\n");
                exit(0);
            default:
                printf("\nInvalid choice. Please pick 1, 2, or 3.\n");
        }
    }

    return 0;
}
