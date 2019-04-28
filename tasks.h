#ifndef __TASKS_H_
#define __TASKS_H_

#define min(a,b) (a<b?a:b)
typedef struct process process;
typedef struct _task task;
struct _task{
    int task_id;
    int wcet;
    int period;
    //absolute deadline
    int deadline;
    int next_release_time;
    process *job;
    int job_index;
    float response_time[100];
    float arrival[100];
};


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

void    submit_processes(task ***taskset, int *task_count, int *pid_count, process ***ready_queue, float *util);
void    display_process(process **ready_queue, int size);
int     get_lcm(task **global_tasks, int task_count);
process* process_init(int pid_v, int wcet_v, int task_id, task *task_ref);
float   get_next_arrival(process **rdqueue, int cur_time, int nproc, task **taskset);
int
get_next_edf(int min_deadline_task, process **rdqueue, int nproc);
int
get_min_lax_procs(process **ready_queue, int task_count);
#endif