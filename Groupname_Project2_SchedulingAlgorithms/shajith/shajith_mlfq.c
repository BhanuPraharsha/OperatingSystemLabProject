// mlfq scheduling simulation - shajith
// 3 queues with different time quantums

#include <stdio.h>

#define N 5
#define QUEUES 3

struct proc {
    int pid;
    char name[4];
    int burst;
    int arrival;
    int remaining;
    int queue;      // which queue its in
    int done;
    int wait_time;
    int finish_time;
};

int quantum[QUEUES] = {2, 4, 8};

// queue arrays - each holds process indices
int q[QUEUES][N];
int qsize[QUEUES] = {0, 0, 0};

void add_to_queue(int level, int idx) {
    q[level][qsize[level]] = idx;
    qsize[level]++;
}

// remove front of queue and shift everything left
int pop_front(int level) {
    if (qsize[level] == 0) return -1;
    int val = q[level][0];
    int i;
    for (i = 1; i < qsize[level]; i++)
        q[level][i-1] = q[level][i];
    qsize[level]--;
    return val;
}

// check if all processes are done
int all_done(struct proc p[]) {
    int i;
    for (i = 0; i < N; i++)
        if (!p[i].done) return 0;
    return 1;
}

// add any processes that arrive at this tick
void check_arrivals(struct proc p[], int tick) {
    int i;
    for (i = 0; i < N; i++) {
        if (p[i].arrival == tick && !p[i].done) {
            p[i].queue = 0;
            add_to_queue(0, i);
        }
    }
}

// find which queue has something to run
int find_queue() {
    int i;
    for (i = 0; i < QUEUES; i++)
        if (qsize[i] > 0) return i;
    return -1;
}

void print_gantt(int gantt[], int len) {
    int i, start, end;
    printf("\nGantt Chart:\n");
    printf("|");
    i = 0;
    while (i < len) {
        start = i;
        while (i < len && gantt[i] == gantt[start]) i++;
        end = i;
        // print the block
        int width = end - start;
        int j;
        printf(" P%d ", gantt[start]);
        for (j = 0; j < width - 1; j++) printf("-");
        printf("|");
    }
    printf("\n");
}

void print_results(struct proc p[]) {
    int i;
    float total_wt = 0, total_tt = 0;
    printf("\n%-5s %-6s %-8s %-12s %-12s\n",
        "PID", "Name", "Burst", "Waiting", "Turnaround");
    printf("-------------------------------------------\n");
    for (i = 0; i < N; i++) {
        int tt = p[i].finish_time - p[i].arrival;
        int wt = tt - p[i].burst;
        printf("%-5d %-6s %-8d %-12d %-12d\n",
            p[i].pid, p[i].name, p[i].burst, wt, tt);
        total_wt += wt;
        total_tt += tt;
    }
    printf("-------------------------------------------\n");
    printf("Avg Waiting Time: %.2f\n", total_wt / N);
    printf("Avg Turnaround Time: %.2f\n", total_tt / N);
}

int main() {
    struct proc p[N] = {
        {1, "P1", 6, 0, 6, 0, 0, 0, 0},
        {2, "P2", 3, 1, 3, 0, 0, 0, 0},
        {3, "P3", 8, 2, 8, 0, 0, 0, 0},
        {4, "P4", 4, 3, 4, 0, 0, 0, 0},
        {5, "P5", 2, 5, 2, 0, 0, 0, 0}
    };

    int gantt[200];
    int glen = 0;
    int tick = 0;
    int i;

    printf("MLFQ Scheduling Simulation\n");
    printf("Queues: Q0 (qt=%d), Q1 (qt=%d), Q2 (qt=%d)\n\n",
        quantum[0], quantum[1], quantum[2]);

    // show process info
    printf("%-5s %-6s %-8s %-10s\n", "PID", "Name", "Burst", "Arrival");
    for (i = 0; i < N; i++)
        printf("%-5d %-6s %-8d %-10d\n",
            p[i].pid, p[i].name, p[i].burst, p[i].arrival);

    while (!all_done(p)) {
        check_arrivals(p, tick);

        int ql = find_queue();
        if (ql == -1) {
            // nobody ready, cpu idle
            gantt[glen++] = 0;
            tick++;
            continue;
        }

        int idx = pop_front(ql);
        int qt = quantum[ql];
        int ran = 0;

        // run this process for up to qt ticks
        while (ran < qt && p[idx].remaining > 0) {
            // check for new arrivals each tick
            if (ran > 0) check_arrivals(p, tick);
            gantt[glen++] = p[idx].pid;
            p[idx].remaining--;
            tick++;
            ran++;
        }

        if (p[idx].remaining == 0) {
            // process finished
            p[idx].done = 1;
            p[idx].finish_time = tick;
        } else {
            // used full quantum, demote
            int next_q = ql + 1;
            if (next_q >= QUEUES) next_q = QUEUES - 1;
            p[idx].queue = next_q;
            add_to_queue(next_q, idx);
        }
    }

    print_gantt(gantt, glen);
    print_results(p);

    return 0;
}
