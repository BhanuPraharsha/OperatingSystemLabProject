#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "algorithms.h"
#include "stats.h"

void reset_processes(Process processes[], int n) {
    Process initial[20] = {
        {1,   0,   8,  8,  15, 0, READY, 0, -1, -1},
        {2,   1,  25, 25,  60, 0, READY, 0, -1, -1},  
        {3,   2,   3,  3,  10, 0, READY, 0, -1, -1},  
        {4,   5,  12, 12,  30, 0, READY, 0, -1, -1},
        {5,   8,   2,  2,  12, 0, READY, 0, -1, -1},
        {6,  10,  40, 40, 100, 0, READY, 0, -1, -1},  
        {7,  12,   5,  5,  25, 0, READY, 0, -1, -1},
        {8,  15,   6,  6,  30, 0, READY, 0, -1, -1},
        {9,  20,  15, 15,  50, 0, READY, 0, -1, -1},
        {10, 22,   4,  4,  30, 0, READY, 0, -1, -1},
        {11, 25,   8,  8,  40, 0, READY, 0, -1, -1},
        {12, 30,  50, 50, 150, 0, READY, 0, -1, -1},  
        {13, 35,   2,  2,  40, 0, READY, 0, -1, -1},
        {14, 40,   9,  9,  60, 0, READY, 0, -1, -1},
        {15, 45,   3,  3,  50, 0, READY, 0, -1, -1},
        {16, 50,  18, 18,  80, 0, READY, 0, -1, -1},  
        {17, 55,   5,  5,  65, 0, READY, 0, -1, -1},
        {18, 60,   7,  7,  75, 0, READY, 0, -1, -1},
        {19, 65,  22, 22, 100, 0, READY, 0, -1, -1},
        {20, 70,   4,  4,  80, 0, READY, 0, -1, -1}
    };
    
    for (int i = 0; i < n; i++) {
        processes[i] = initial[i];
    }
}

int main() {
    int num_processes = 20;
    int num_cpus; 
    Process processes[20];
    int choice;

    printf("=========================================\n");
    printf(" Multiprocessor Scheduling Simulator\n");
    printf("=========================================\n\n");

    // Ask for the number of CPUs dynamically
    printf("Enter the number of CPUs to simulate (e.g., 2, 4, 8): ");
    if (scanf("%d", &num_cpus) != 1 || num_cpus < 1) {
        printf("Invalid input. Defaulting to 2 CPUs.\n");
        num_cpus = 2; // Fallback just in case you type a letter by accident
    }

    while (1) {
        reset_processes(processes, num_processes); // MUST happen before algorithm runs

        printf("\nSystem Configuration: %d CPUs | %d Processes\n", num_cpus, num_processes);
        printf("\nSelect Algorithm:\n");
        printf("1. Custom Multiprocessor Round Robin\n");
        printf("2. Earliest Deadline First (EDF)\n");
        printf("3. Multilevel Feedback Queue (MLFQ)\n");
        printf("4. Exit\n");
        printf("Choice: ");
        
        if (scanf("%d", &choice) != 1) break;

        if (choice == 1) {
            run_custom_rr(processes, num_processes, num_cpus);
            print_process_stats(processes, num_processes);
        } else if (choice == 2) {
            run_edf(processes, num_processes, num_cpus);
            print_process_stats(processes, num_processes);
        } else if (choice == 3) {
            run_mlfq(processes, num_processes, num_cpus);
            print_process_stats(processes, num_processes);
        } else if (choice == 4) {
            printf("\nExiting Simulator. Goodbye!\n");
            break;
        } else {
            printf("\nInvalid choice. Try again.\n");
        }
    }
    return 0;
}