// lottery scheduler simulation - shajith
// each process gets tickets, random draw each round

#include <stdio.h>
#include <stdlib.h>

#define N 5

struct proc {
    int pid;
    char name[4];
    int burst;
    int tickets;
    int remaining;
    int done;
    int finish_time;
};

// pick a winner based on ticket ranges
int pick_winner(struct proc p[], int total) {
    int r = rand() % total;
    int sum = 0;
    int i;
    for (i = 0; i < N; i++) {
        if (p[i].done) continue;
        sum += p[i].tickets;
        if (r < sum) return i;
    }
    return -1; // shouldnt happen
}

// count total tickets of alive processes
int count_tickets(struct proc p[]) {
    int total = 0, i;
    for (i = 0; i < N; i++)
        if (!p[i].done) total += p[i].tickets;
    return total;
}

int all_done(struct proc p[]) {
    int i;
    for (i = 0; i < N; i++)
        if (!p[i].done) return 0;
    return 1;
}

void print_gantt(int gantt[], int len) {
    int i, start;
    printf("\nGantt Chart:\n|");
    i = 0;
    while (i < len) {
        start = i;
        while (i < len && gantt[i] == gantt[start]) i++;
        int width = i - start;
        int j;
        printf(" P%d ", gantt[start]);
        for (j = 0; j < width - 1; j++) printf("-");
        printf("|");
    }
    printf("\n");
}

void print_results(struct proc p[], int total_time) {
    int i;
    float total_wt = 0, total_tt = 0;
    printf("\n%-5s %-6s %-8s %-10s %-12s %-12s\n",
        "PID", "Name", "Burst", "Tickets", "Waiting", "Turnaround");
    printf("--------------------------------------------------\n");
    for (i = 0; i < N; i++) {
        int tt = p[i].finish_time;  // all arrive at 0
        int wt = tt - p[i].burst;
        printf("%-5d %-6s %-8d %-10d %-12d %-12d\n",
            p[i].pid, p[i].name, p[i].burst, p[i].tickets, wt, tt);
        total_wt += wt;
        total_tt += tt;
    }
    printf("--------------------------------------------------\n");
    printf("Avg Waiting Time: %.2f\n", total_wt / N);
    printf("Avg Turnaround Time: %.2f\n", total_tt / N);
}

int main() {
    srand(42);

    struct proc p[N] = {
        {1, "P1", 5, 20, 5, 0, 0},
        {2, "P2", 3, 40, 3, 0, 0},
        {3, "P3", 7, 10, 7, 0, 0},
        {4, "P4", 4, 25, 4, 0, 0},
        {5, "P5", 2, 5,  2, 0, 0}
    };

    int gantt[200];
    int glen = 0;
    int tick = 0;
    int i;

    printf("Lottery Scheduling Simulation\n\n");

    // show process info
    printf("%-5s %-6s %-8s %-10s\n", "PID", "Name", "Burst", "Tickets");
    for (i = 0; i < N; i++)
        printf("%-5d %-6s %-8d %-10d\n",
            p[i].pid, p[i].name, p[i].burst, p[i].tickets);

    while (!all_done(p)) {
        int total = count_tickets(p);
        int winner = pick_winner(p, total);

        // run winner for 1 tick
        gantt[glen++] = p[winner].pid;
        p[winner].remaining--;
        tick++;

        if (p[winner].remaining == 0) {
            p[winner].done = 1;
            p[winner].finish_time = tick;
        }
    }

    print_gantt(gantt, glen);
    print_results(p, tick);

    return 0;
}
