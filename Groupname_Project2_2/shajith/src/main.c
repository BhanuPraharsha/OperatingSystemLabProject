#include <stdio.h>
#include "lottery.h"
#include "mlfq.h"

int main() {
    int choice;

    printf("CPU Scheduling Simulator\n");
    printf("-------------------------\n");
    printf("1. Lottery Scheduling\n");
    printf("2. MLFQ Scheduling\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice == 1)
        run_lottery();
    else if (choice == 2)
        run_mlfq();
    else
        printf("Invalid choice\n");

    return 0;
}
