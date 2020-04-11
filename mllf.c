#include <stdio.h>
#include <stdlib.h>
#include "tasks.h"

#define MAXPROCESS 100
task **global_tasks = NULL;
process **ready_queue;
static int pid_count = 0;
static int task_count = 0;

int main()
{
    float util = 0;
    submit_processes(&global_tasks, &task_count, &pid_count, &ready_queue, &util);
    display_process(ready_queue, task_count);
    int lcm = get_lcm(global_tasks, task_count);
    // FILE *log_file = fopen("sched-op-mllf.txt", "a+");
    FILE *log_file = fopen("sched-op-mllf.txt", "w");
    fprintf(log_file, "Hyperperiod %d\n", lcm);
    fprintf(log_file, "Utilisation %f\n", util);
    fclose(log_file);
    printf("%d\n", get_lcm(global_tasks, task_count));
    schedule_mllf(ready_queue, task_count, lcm, global_tasks, &pid_count);
    calculate_parameters(global_tasks, task_count);
    return 0;
}