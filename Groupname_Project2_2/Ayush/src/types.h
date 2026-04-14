#ifndef TYPES_H
#define TYPES_H

typedef enum { READY, RUNNING, FINISHED } State;

// Process structure
typedef struct {
    int id;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int period;             // used for RMS (periodic task period)
    int tickets;            // used for Lottery scheduling
    int priority_level;     // unused here, kept for compatibility
    State state;

    // metrics
    int completion_time;
    int start_time;
    int cpu_id;
} Process;

// CPU structure
typedef struct {
    int id;
    int current_process_id; // -1 if idle
    int time_quantum_left;  // for time-sliced scheduling
} CPU;

#endif
