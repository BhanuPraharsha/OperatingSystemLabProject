#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "types.h"
#include "stats.h"


// algorithm 1: Custom Multiprocessor Round Robin (GLobal Ready Queue)

void run_custom_rr(Process processes[], int num_processes, int num_cpus) {
    printf("\n --- Starting Multiprocessor Round Robin (Qunatum = 4)---\n");
    printf("Time\t");

    for(int c = 0; c< num_cpus; c++) {
        printf("cpu %d\t", c);

    }

    printf("\n--------------------------------------\n");

    CPU cpus[num_cpus];
    for(int i =0; i<num_cpus; i++) {
        cpus[i].id = i;
        cpus[i].current_process_id  = -1;
        cpus[i].time_quantum_left = 0;

    }

    int current_time = 0;
    int completed_count = 0;
    int time_quantum = 4;


    while (completed_count < num_processes) {
        //assign tasks to idle cpus
        for(int i=0; i< num_cpus; i++){
            if(cpus[i].current_process_id == -1){
                for(int p =0; p < num_processes; p++){
                    if(processes[p].state == READY && processes[p].arrival_time <= current_time){
                        cpus[i].current_process_id = p;
                        processes[p].state = RUNNING;
                        processes[p].cpu_id = i;
                        cpus[i].time_quantum_left = time_quantum;

                        //record start time for metric
                        if(processes[p].start_time == -1){
                            processes[p].start_time = current_time;
                        }
                        break;
                    }
                }
            }
        }
        print_gantt_chart(current_time, cpus, num_cpus, processes);

        for(int i =0; i< num_cpus; i++){
            if(cpus[i].current_process_id !=-1){
                int pid = cpus[i].current_process_id;
                processes[pid].remaining_time--;
                cpus[i].time_quantum_left--;
                //check if process completed
                if(processes[pid].remaining_time==0){
                    processes[pid].state = FINISHED;
                    processes[pid].completion_time = current_time+1;
                    cpus[i].current_process_id = -1;
                    completed_count++;
                }
                //check if time quantum expired
                else if(cpus[i].time_quantum_left==0){
                    processes[pid].state = READY;
                    cpus[i].current_process_id = -1;
                }
            }
        }
        current_time++;
    }
}


//algorithm 2: Earliest Deadline First

void run_edf(Process processes[], int num_processes, int num_cpus){
    printf("\n---Starting Earliest Deadline First (EDF) ---\n");
    printf("Time\t");
    
    for(int c = 0; c < num_cpus; c++){
        printf("CPU %d\t", c);

    }
    printf("\n----------------------------------------\n");

    CPU cpus[num_cpus];
    for(int i=0; i<num_cpus; i++){
        cpus[i].id = i;
        cpus[i].current_process_id = -1;
    }

    int current_time = 0;
    int completed_count = 0;

    while(completed_count < num_processes){




        // 1. Assign Tasks to idle cpu based on deadline

        for(int i=0; i< num_cpus; i++){
            if (cpus[i].current_process_id == -1){

                int best_process = -1;
                int earliest_deadline = 1e5;

                for(int p =0; p< num_processes; p++){
                    if(processes[p].state == READY && processes[p].arrival_time <= current_time)
                    if(earliest_deadline  > processes[p].deadline){
                        earliest_deadline = processes[p].deadline;
                        best_process = p;
                    }

                }
                    if(best_process!=-1) {
                        cpus[i].current_process_id = best_process;
                        processes[best_process].state = RUNNING;
                        processes[best_process].cpu_id = i;
                    

                    if(processes[best_process].start_time==-1){
                        processes[best_process].start_time = current_time;
                    }
            }
        }
    }
        print_gantt_chart(current_time, cpus, num_cpus, processes);

        //execute tick
        for(int i=0; i< num_cpus; i++){
            if(cpus[i].current_process_id!=-1){
                int pid = cpus[i].current_process_id;
                processes[pid].remaining_time--;

                if(processes[pid].remaining_time == 0){
                    processes[pid].state = FINISHED;
                    processes[pid].completion_time = current_time+1;
                    cpus[i].current_process_id = -1;
                    completed_count++;
                }
            }
        }
        current_time++;
        // if(current_time > 1000){
        //     printf("wtf\n");
        //     break;
        // }
    }
}

//  algorithm 3: Multilevel Feedback Queue

void run_mlfq(Process processes[], int num_processes, int num_cpu){
    printf("\n---Starting Multilevel Feedback Queue---\n");
    printf("Queue 0: Quantum = 4 | Queue 1: Quantum: 8 | Queue 2: FCFS\n");

    printf("Time\t");
    for(int c =0; c< num_cpu; c++){
        printf("CPU %d\t", c);

    }
    printf("\n------------------------------------------\n");

    CPU cpus[num_cpu];
    for(int i=0; i<num_cpu; i++){
        cpus[i].id = i;
        cpus[i].current_process_id = -1;
        cpus[i].time_quantum_left = 0;
    }

    int current_time = 0;
    int completed_count = 0;

    //time quantums for Queue 0 and Queue 1.
    // Queue 2 is FCFS, so we can use a summy value for the 3rd slot.
    int quantums[3] = {4, 8, 1e5};
        int boost_interval = 20;

    while(completed_count < num_processes) {
        //boost the exisitng processes


        if(current_time > 0  && current_time % boost_interval == 0){
            for(int p = 0; p< num_processes; p++){
                if(processes[p].state == READY  || processes[p].state == RUNNING){
                    processes[p].priority_level= 0;
                

                if(processes[p].state == RUNNING){
                    int cpu = processes[p].cpu_id;
                    cpus[cpu].time_quantum_left = quantums[0];
                }
            }
        }
    }
        for(int i=0; i<num_cpu; i++){
            int best_process = -1;
            int best_priority = 1e4;

            //find highest priority
            for(int p =0; p<num_processes; p++){
                if(processes[p].state == READY && processes[p].arrival_time <= current_time)
                if(processes[p].priority_level < best_priority){
                    best_priority = processes[p].priority_level;
                    best_process = p;
                }
            }

            if(best_priority !=-1){
                if(cpus[i].current_process_id == -1){
                    cpus[i].current_process_id = best_process;
                    processes[best_process].state = RUNNING;
                    processes[best_process].cpu_id = i;

                    cpus[i].time_quantum_left = quantums[processes[best_process].priority_level];

                    if(processes[best_process].start_time ==-1){
                        processes[best_process].start_time = current_time;
                    }
                }
            }

            else{
                int current_pid = cpus[i].current_process_id;
                if(processes[best_process].priority_level < processes[current_pid].priority_level){
                    //preempt
                    processes[current_pid].state = READY;
                //swap the new higher-priority task
                    cpus[i].current_process_id = best_process;
                    processes[best_process].state = RUNNING;
                    processes[best_process].cpu_id = i;
                    cpus[i].time_quantum_left = quantums[processes[best_process].priority_level];

                    if(processes[best_process].start_time ==-1){
                        processes[best_process].start_time = current_time;
                    }
                }
            }
        }


        print_gantt_chart(current_time, cpus, num_cpu, processes);

        for(int i=0; i<num_cpu; i++){
            if(cpus[i].current_process_id !=-1){
                int pid = cpus[i].current_process_id;
                processes[pid].remaining_time--;

                    //only decrease time quantum if its not fscs
                if(processes[pid].priority_level < 2){
                    cpus[i].time_quantum_left--;
                }

                if(processes[pid].remaining_time <=0){
                    processes[pid].state = FINISHED;
                    processes[pid].completion_time = current_time +1;
                    cpus[i].current_process_id = -1;
                    completed_count++;
                }
                //check if time quantum expired
                else if(processes[pid].priority_level < 2 && cpus[i].time_quantum_left <= 0){
                    processes[pid].priority_level++;
                    processes[pid].state = READY;
                    cpus[i].current_process_id = -1;
                }
            }
        }
        current_time++;
    }
}