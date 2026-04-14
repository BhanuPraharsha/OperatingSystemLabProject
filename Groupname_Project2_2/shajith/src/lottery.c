#include <stdio.h>
#include <stdlib.h>
#include "lottery.h"

#define MAX_PROC 10
#define GANTT_SIZE 500

struct proc {
    int pid;
    int burst;
    int tickets;
    int remaining;
    int done;
    int finish_time;
};

static int pick_winner(struct proc p[], int n, int total) {
    int r = rand() % total;
    int sum = 0, i;
    for (i = 0; i < n; i++) {
        if (p[i].done) continue;
        sum += p[i].tickets;
        if (r < sum) return i;
    }
    return -1;
}

static int count_tickets(struct proc p[], int n) {
    int total = 0, i;
    for (i = 0; i < n; i++)
        if (!p[i].done) total += p[i].tickets;
    return total;
}

static int all_done(struct proc p[], int n) {
    int i;
    for (i = 0; i < n; i++)
        if (!p[i].done) return 0;
    return 1;
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

void run_lottery() {
    struct proc p[MAX_PROC];
    int n, i;
    unsigned int seed;

    printf("\n--- Lottery Scheduling ---\n");
    printf("Enter number of processes (max %d): ", MAX_PROC);
    scanf("%d", &n);

    if (n < 1 || n > MAX_PROC) {
        printf("Invalid number of processes\n");
        return;
    }

    for (i = 0; i < n; i++) {
        p[i].pid = i + 1;
        p[i].done = 0;
        p[i].finish_time = 0;
        printf("P%d burst time: ", i + 1);
        scanf("%d", &p[i].burst);
        printf("P%d tickets: ", i + 1);
        scanf("%d", &p[i].tickets);
        p[i].remaining = p[i].burst;
    }

    printf("Enter seed for random (e.g. 42): ");
    scanf("%u", &seed);
    srand(seed);

    printf("\nProcess Info:\n");
    printf("%-6s %-10s %-10s\n", "PID", "Burst", "Tickets");
    printf("---------------------------\n");
    for (i = 0; i < n; i++)
        printf("%-6d %-10d %-10d\n", p[i].pid, p[i].burst, p[i].tickets);

    int gantt[GANTT_SIZE];
    int glen = 0, tick = 0;

    while (!all_done(p, n)) {
        int total = count_tickets(p, n);
        int winner = pick_winner(p, n, total);
        if (winner == -1) break;

        gantt[glen++] = p[winner].pid;
        p[winner].remaining--;
        tick++;

        if (p[winner].remaining == 0) {
            p[winner].done = 1;
            p[winner].finish_time = tick;
        }
    }

    print_gantt(gantt, glen);

    float total_wt = 0, total_tt = 0;
    printf("\n%-6s %-10s %-10s %-12s %-12s\n", "PID", "Burst", "Tickets", "Waiting", "Turnaround");
    printf("--------------------------------------------------\n");
    for (i = 0; i < n; i++) {
        int tt = p[i].finish_time;
        int wt = tt - p[i].burst;
        printf("%-6d %-10d %-10d %-12d %-12d\n", p[i].pid, p[i].burst, p[i].tickets, wt, tt);
        total_wt += wt;
        total_tt += tt;
    }
    printf("--------------------------------------------------\n");
    printf("Avg Waiting Time: %.2f\n", total_wt / n);
    printf("Avg Turnaround Time: %.2f\n", total_tt / n);
}
