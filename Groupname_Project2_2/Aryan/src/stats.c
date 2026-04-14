#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "types.h"

void print_gantt_chart(int time, CPU cpus[], int num_cpus, Process processes[]) {
    printf("%02d\t", time);
    for(int i=0; i<num_cpus; i++) {
        if(cpus[i].current_process_id!=-1){
            printf("[P%d]\t", processes[cpus[i].current_process_id].id);

        }
        else{
            printf("[--]\t");
        }
    }

    printf("\n");
}

// Helper function to print the final statistics table
void print_process_stats(Process processes[], int num_processes) {
    printf("\n");
    // Print the header row with left-aligned spacing
    printf("%-8s %-10s %-8s %-15s %-15s %-10s\n", 
           "PID", "Arrival", "Burst", "Completion", "Turnaround", "Waiting");
    printf("-------------------------------------------------------------------------\n");

    float total_turnaround = 0;
    float total_waiting = 0;

    // Calculate and print metrics for each process
    for (int i = 0; i < num_processes; i++) {
        int turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        int waiting_time = turnaround_time - processes[i].burst_time;

        total_turnaround += turnaround_time;
        total_waiting += waiting_time;

        printf("%-8d %-10d %-8d %-15d %-15d %-10d\n",
               processes[i].id,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].completion_time,
               turnaround_time,
               waiting_time);
    }
    
    printf("-------------------------------------------------------------------------\n");
    printf("Average Turnaround Time: %.2f\n", total_turnaround / num_processes);
    printf("Average Waiting Time: %.2f\n\n", total_waiting / num_processes);
}