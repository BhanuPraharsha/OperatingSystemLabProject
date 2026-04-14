#ifndef TYPES_H
#define TYPES_H

typedef enum { READY, RUNNING, FINISHED} State;

//process structure
typedef struct {
    int id;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int deadline;           //only used for edf
    int priority_level;     //only used for MLFQ(0: highest priority)
    State state;

    //metrics
    int completion_time;
    int start_time;
    int cpu_id;
} Process;

//cpu structure
typedef struct  {
    int id;
    int current_process_id; //-1 if idle
    int time_quantum_left; //for round robin
} CPU;

#endif