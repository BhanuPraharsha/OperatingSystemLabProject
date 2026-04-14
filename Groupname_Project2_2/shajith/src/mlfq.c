#include <stdio.h>
#include "mlfq.h"

#define MAX_PROC 10
#define QUEUES 3
#define GANTT_SIZE 500

struct proc {
    int pid;
    int burst;
    int arrival;
    int remaining;
    int queue;
    int arrived;
    int done;
    int finish_time;
};

static int q[QUEUES][MAX_PROC];
static int qsize[QUEUES];

static void add_to_queue(int level, int idx) {
    q[level][qsize[level]++] = idx;
}

static int pop_front(int level) {
    if (qsize[level] == 0) return -1;
    int val = q[level][0], i;
    for (i = 1; i < qsize[level]; i++)
        q[level][i - 1] = q[level][i];
    qsize[level]--;
    return val;
}

static int all_done(struct proc p[], int n) {
    int i;
    for (i = 0; i < n; i++)
        if (!p[i].done) return 0;
    return 1;
}

static void check_arrivals(struct proc p[], int n, int tick) {
    int i;
    for (i = 0; i < n; i++) {
        if (!p[i].arrived && p[i].arrival == tick) {
            p[i].arrived = 1;
            p[i].queue = 0;
            add_to_queue(0, i);
        }
    }
}

static int find_queue() {
    int i;
    for (i = 0; i < QUEUES; i++)
        if (qsize[i] > 0) return i;
    return -1;
}

static void print_gantt(int gantt[], int len) {
    int i = 0;
    printf("\nGantt Chart:\n|");
    while (i < len) {
        int start = i;
        while (i < len && gantt[i] == gantt[start]) i++;
        int width = i - start, j;
        printf(" P%d ", gantt[start]);
        for (j = 0; j < width - 1; j++) printf("-");
        printf("|");
    }
    printf("\n");
}

void run_mlfq() {
    struct proc p[MAX_PROC];
    int n, i;
    int quantum[QUEUES];

    for (i = 0; i < QUEUES; i++) qsize[i] = 0;

    printf("\n--- MLFQ Scheduling ---\n");
    printf("Enter number of processes (max %d): ", MAX_PROC);
    scanf("%d", &n);

    if (n < 1 || n > MAX_PROC) {
        printf("Invalid number of processes\n");
        return;
    }

    for (i = 0; i < n; i++) {
        p[i].pid = i + 1;
        p[i].done = 0;
        p[i].arrived = 0;
        p[i].finish_time = 0;
        p[i].queue = 0;
        printf("P%d burst time: ", i + 1);
        scanf("%d", &p[i].burst);
        printf("P%d arrival time: ", i + 1);
        scanf("%d", &p[i].arrival);
        p[i].remaining = p[i].burst;
    }

    printf("Time quantum for Q0: ");
    scanf("%d", &quantum[0]);
    printf("Time quantum for Q1: ");
    scanf("%d", &quantum[1]);
    printf("Time quantum for Q2: ");
    scanf("%d", &quantum[2]);

    printf("\nQueues: Q0(qt=%d)  Q1(qt=%d)  Q2(qt=%d)\n", quantum[0], quantum[1], quantum[2]);
    printf("\nProcess Info:\n");
    printf("%-6s %-10s %-10s\n", "PID", "Burst", "Arrival");
    printf("---------------------------\n");
    for (i = 0; i < n; i++)
        printf("%-6d %-10d %-10d\n", p[i].pid, p[i].burst, p[i].arrival);

    int gantt[GANTT_SIZE];
    int glen = 0, tick = 0;

    while (!all_done(p, n)) {
        check_arrivals(p, n, tick);

        int ql = find_queue();
        if (ql == -1) {
            gantt[glen++] = 0;
            tick++;
            continue;
        }

        int idx = pop_front(ql);
        int qt = quantum[ql];
        int ran = 0;

        while (ran < qt && p[idx].remaining > 0) {
            if (ran > 0) check_arrivals(p, n, tick);
            gantt[glen++] = p[idx].pid;
            p[idx].remaining--;
            tick++;
            ran++;
        }

        if (p[idx].remaining == 0) {
            p[idx].done = 1;
            p[idx].finish_time = tick;
        } else {
            int next_q = ql + 1;
            if (next_q >= QUEUES) next_q = QUEUES - 1;
            p[idx].queue = next_q;
            add_to_queue(next_q, idx);
        }
    }

    print_gantt(gantt, glen);

    float total_wt = 0, total_tt = 0;
    printf("\n%-6s %-10s %-10s %-12s %-12s\n", "PID", "Burst", "Arrival", "Waiting", "Turnaround");
    printf("--------------------------------------------------\n");
    for (i = 0; i < n; i++) {
        int tt = p[i].finish_time - p[i].arrival;
        int wt = tt - p[i].burst;
        printf("%-6d %-10d %-10d %-12d %-12d\n", p[i].pid, p[i].burst, p[i].arrival, wt, tt);
        total_wt += wt;
        total_tt += tt;
    }
    printf("--------------------------------------------------\n");
    printf("Avg Waiting Time: %.2f\n", total_wt / n);
    printf("Avg Turnaround Time: %.2f\n", total_tt / n);
}
