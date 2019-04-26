#ifndef __TASKS_H_
#define __TASKS_H_

typedef struct _task task;
struct _task{
    int task_id;
    int wcet;
    int period;
    //absolute deadline
    int deadline;
    int next_release_time;
};

typedef struct process process;
struct process{
    int pid;
    //to map the job to task
    int task_id;
    //actual execution time of this job
    int aet;
    //apriori known as a part of the temporal parameters of the tasks.
    int wcet;
    //remaining execution time
    int ret;
    //current priority : @t=0- assigned; @t=t- current
    int priority;
    //used in sched policies dynamically assigning current prio based upon slack
    int slack;
    task *task_ref;
};

void    submit_processes(task ***taskset, int *task_count, int *pid_count, process ***ready_queue);
void    display_process(process **ready_queue, int size);
#endif