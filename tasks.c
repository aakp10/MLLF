#include "tasks.h"
#include <stdio.h>
#include <stdlib.h>
task*
task_init(int task_id, int wcet, int period, int deadline)
{
    task *temp = (task*)malloc(sizeof(task));
    temp->task_id = task_id;
    temp->wcet = wcet;
    temp->period = period;
    temp->deadline = deadline;
    temp->next_release_time = period;
    return temp;
}

process*
process_init(int pid_v, int wcet_v, int task_id, task *task_ref)
{
    process *temp = (process*)malloc(sizeof(process));
    temp->task_id = task_id;
    temp->pid = pid_v;
    temp->wcet = wcet_v;
    // NOTE: Sorry but FUTURE can't be predicted.
    // hence, resort to a random hacky actual exec time generator.
    //temp->aet = get_aet(wcet_v, pid_v);
    temp->ret = wcet_v;
    temp->task_ref = task_ref;
    temp->slack = task_ref->deadline - wcet_v;
    return temp;
}

void
submit_processes(task *taskset[], int task_count, int pid_count, process *ready_queue[])
{
    FILE *task_file = fopen("tasks", "r");
    int wcet, period, deadline;
    int task_no = 0;
    int process_count;
    fscanf(task_file,"%d", &process_count);
    //pqueue *ready_queue = pqueue_init(process_count, MAXPROCESS);
    taskset = (task**)malloc(process_count * sizeof(task*));
    //wcet, period, deadline
    while(fscanf(task_file, "%d, %d, %d", &wcet, &period, &deadline) == 3)
    {
        task *t = task_init(task_count++, wcet, period, deadline);
        //process_init(int pid_v, int wcet_v, int priority_v, int task_id, task *task_ref);
        process *p = process_init(pid_count++, wcet, task_count, t);
        taskset[task_count-1] = t;
        ready_queue[task_count-1] = p;
    }
    fclose(task_file);
}
