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
submit_processes(task ***global_tasks, int *task_count, int *pid_count, process ***rq)
{
    FILE *task_file = fopen("input", "r");
    int wcet, period, deadline;
    int task_no = 0;
    int process_count;
    fscanf(task_file,"%d", &process_count);
    printf("%d", process_count);
    //pqueue *ready_queue = pqueue_init(process_count, MAXPROCESS);
    task **taskset = (task**)malloc(process_count * sizeof(task*));
    process **ready_queue = (process **)malloc(process_count * sizeof(process*));
    //wcet, period, deadline
    while(fscanf(task_file, "%d, %d, %d", &wcet, &period, &deadline) == 3)
    {
        printf("%d, %d, %d\n", wcet, period, deadline);
        task *t = task_init((*task_count)++, wcet, period, deadline);
        process *p = process_init((*pid_count)++, wcet, *task_count, t);
        taskset[(*task_count)-1] = t;
        ready_queue[(*task_count)-1] = p;
    }
    fclose(task_file);
    *global_tasks = taskset;
    *rq = ready_queue;
}


void
display_process(process **ready_queue, int size)
{
    printf("READY PROCESSES\n");
    for(int i = 0; i < size; i++)
    {
        printf("pid:%d deadline:%d\n", ready_queue[i]->pid, ready_queue[i]->task_ref->deadline);
    }
    printf("END\n");

}

static int
get_gcd(int num1, int num2)
{
    if(num2 == 0)
        return num1;
    return get_gcd(num2, num1%num2);
}

int get_lcm(task **global_tasks, int task_count)
{
    int hyper_period = 1;
    for(int i = 0; i < task_count; i++)
    {
        task *p2 = global_tasks[i];
        //printf("%d, ", p2->period);
        hyper_period = (hyper_period * p2->period)/get_gcd(hyper_period, p2->period);
    }
    return hyper_period;
}