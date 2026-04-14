/*
 * algorithms.h
 * Header file for the scheduling simulation.
 * Contains the Process struct and function declarations.
 */

#ifndef ALGORITHMS_H
#define ALGORITHMS_H

/* Maximum number of processes we can handle */
#define MAX_PROCESSES 10

/* Maximum time units for the simulation */
#define MAX_TIME 100

/* Struct to hold all info about a process */
typedef struct {
    int pid;            /* Process ID */
    int burst_time;     /* Total CPU time needed */
    int arrival_time;   /* When the process arrives */
    int tickets;        /* Number of lottery tickets (for Lottery scheduling) */
    int period;         /* Period of the process (for RMS scheduling) */
    int remaining_time; /* How much CPU time is left */
} Process;

/* Struct to store one entry of the execution trace */
typedef struct {
    int time;           /* The time unit */
    int pid;            /* Which process ran at this time */
    char algorithm[20]; /* Name of the algorithm used */
} TraceEntry;

/* Function to run the Lottery scheduling simulation */
void lottery_scheduling(Process procs[], int n, TraceEntry trace[], int *trace_count, int total_time);

/* Function to run the Rate-Monotonic scheduling simulation */
void rms_scheduling(Process procs[], int n, TraceEntry trace[], int *trace_count, int total_time);

/* Function to print Gantt chart in the terminal */
void print_gantt_chart(TraceEntry trace[], int trace_count, const char *title);

#endif
