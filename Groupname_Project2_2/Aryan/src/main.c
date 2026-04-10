#include <stdio.h>
#include <stdlib.h>
#include "algorithms.h"
#include "types.h"



void reset_processes(Process processes[], int n) {
    // id, arrival_time, burst_time, remaining_time, deadline, priority_level, state, completion, start, cpu_id
    Process initial[20] = {
        // Initial Batch (Mix of heavy and light)
        {1,   0,   8,  8,  15, 0, READY, 0, -1, -1},
        {2,   1,  25, 25,  60, 0, READY, 0, -1, -1},  // CPU heavy, will drop to Queue 2
        {3,   2,   3,  3,  10, 0, READY, 0, -1, -1},  // Quick, urgent deadline
        {4,   5,  12, 12,  30, 0, READY, 0, -1, -1},
        {5,   8,   2,  2,  12, 0, READY, 0, -1, -1},
        
        // Mid-simulation arrivals
        {6,  10,  40, 40, 100, 0, READY, 0, -1, -1},  // Massive job
        {7,  12,   5,  5,  25, 0, READY, 0, -1, -1},
        {8,  15,   6,  6,  30, 0, READY, 0, -1, -1},
        {9,  20,  15, 15,  50, 0, READY, 0, -1, -1},
        {10, 22,   4,  4,  30, 0, READY, 0, -1, -1},
        
        // Late arrivals (Tests preemption of older tasks)
        {11, 25,   8,  8,  40, 0, READY, 0, -1, -1},
        {12, 30,  50, 50, 150, 0, READY, 0, -1, -1},  // The biggest hog
        {13, 35,   2,  2,  40, 0, READY, 0, -1, -1},
        {14, 40,   9,  9,  60, 0, READY, 0, -1, -1},
        {15, 45,   3,  3,  50, 0, READY, 0, -1, -1},
        
        // Arriving right around the Priority Boost window (tick 50)
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
    int num_cpus = 2;
    Process processes[20];

    int choice;
    
    printf("Multiprocessor Scheduling Simulator\n");
    printf("-----------------------------------\n");

    printf("CPUs: %d | Processes: %d\n", num_cpus, num_processes);
    printf("-----------------------------------\n");


    // TODO: add menu to select the algorithm

    while(1){
        reset_processes(processes, num_processes);
        printf("\nSelect a scheduling algorithm:\n");
        printf("1. Round Robin\n");
        printf("2. Earliest Deadline First\n");
        printf("3. Multilevel Feedback Queue\n");
        printf("4. Exit\n");

        printf("enter your choice: ");

        scanf("%d", &choice);
        switch (choice) {
            case 1: 
                run_custom_rr(processes, num_processes, num_cpus);
                break;
            case 2:
                run_edf(processes, num_processes, num_cpus);
                break;
                
            case 3:
                run_mlfq(processes, num_processes, num_cpus);
                break;

            case 4:
                printf("\nexiting the simulator.\n");
                exit(0);

            default:
                printf("\n invalid choice. Please pick a number \n");
        }
    }

    return 0;
}