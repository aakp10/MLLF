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
    submit_processes(&global_tasks, &task_count, &pid_count, &ready_queue);
    display_process(ready_queue, task_count);
    //printf("%d", get_lcm());
    //anticipated_arrival = NULL;
    //schedule_lst(ready_queue, task_count, get_lcm());*/
    return 0;
}