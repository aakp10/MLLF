#include <stdio.h>
#include <stdlib.h>
#include "tasks.h"

#define MAXPROCESS 100
task **global_tasks = NULL;
process **ready_queue;
static int pid_count = 0;
static int task_count = 0;

static void
update_slack(process **rdqueue, int nproc, int cur_time)
{
    //FIXME
    int prev_slack;
    //printf("UPDATION\n");
    for(int i = 0; i < nproc; i++)
    {
        process *cur_proc = rdqueue[i];
        if(cur_proc == NULL)
            continue;
        prev_slack = cur_proc->slack;
        cur_proc->slack = cur_proc->task_ref->deadline - cur_time - cur_proc->ret;
        printf("pid:%d priority: %d slack %d \n", cur_proc->pid, cur_proc->priority, cur_proc->slack);
    }
    printf("END\n");
}

static void
check_arrivals(process **rdqueue, int cur_time, int nproc)
{
    for(int i = 0; i < nproc; i++)
    {
        if(ready_queue[i] == NULL && global_tasks[i]->next_release_time/*period*/ <= cur_time)
        {
            printf("Entered check_arrivals for %d at %d", i, cur_time);
            //global_processes[i]->ret = global_processes[i]->et;
            //update release time
            global_tasks[i]->next_release_time += global_tasks[i]->period;
            //deadline is considered same as period
            global_tasks[i]->deadline += global_tasks[i]->period;
            printf("Deadline updated for task %d is %d", i, global_tasks[i]->deadline);
            //recreate the process .
            process *p = process_init(pid_count++, global_tasks[i]->wcet,
                                        global_tasks[i]->task_id, global_tasks[i]);
            //decrease the slack by cur_time
            p->slack -= cur_time;
            //enqueue into the job_list
            //task_submit_job(global_tasks[i], p);
            ready_queue[i] = p;
            //pqueue_insert_process(rdqueue, p);
        }
    }
    //pqueue_display_process(rdqueue);
}

void
schedule_lst(process **rdqueue, int nproc, int hyperperiod)
{
    int cur_time = 0;
    int prev_task_id = -1;
    int cur_task_id = -1;
    int cpu_idle_time = 0;
    int prev_min_deadline_task;
    while(cur_time < hyperperiod)
    {
        /**
        * FIXME: new arrivals
        */
        check_arrivals(rdqueue, cur_time, nproc);
        //int *least_lax_procs = get_min_lax_procs(rdqueue, nproc);
        //find min deadline job among least_lax_procs
        int min_deadline_task = get_min_lax_procs(rdqueue, nproc);
        if(min_deadline_task != -1)
        {
            if(prev_min_deadline_task && rdqueue[prev_min_deadline_task] && rdqueue[min_deadline_task]->slack == rdqueue[prev_min_deadline_task]->slack)
            {
                min_deadline_task = prev_min_deadline_task;
            }
        }
        

        //find next least deadline more than this task and laxity more than this
        int next_least_deadline_task = get_next_edf(min_deadline_task, rdqueue, nproc);
        if(min_deadline_task != -1) {
            cur_task_id = rdqueue[min_deadline_task]->task_id;
            //printf("time:%d process executing: %d\n", cur_time, cur_proc->pid);
            /*FILE *schedule_file = fopen("schedule.txt", "a+");
            int laxity = rdqueue[min_deadline_task]->task_ref->deadline - cur_time - rdqueue[min_deadline_task]->ret;
            fprintf(schedule_file, "time:%d process executing: %d actual execution time = %d laxity: %d\n", cur_time,
                     cur_proc->pid, cur_proc->wcet, laxity);
            printf("time:%d process executing: %d actual execution time = %d laxity: %d\n", cur_time, cur_proc->pid, cur_proc->aet, laxity);
            fclose(schedule_file);*/
            //find the next least slack time job
            float deadline_diff;
            if(next_least_deadline_task != -1)
                deadline_diff = rdqueue[next_least_deadline_task]->task_ref->deadline - rdqueue[min_deadline_task]->slack;
            else deadline_diff = 1<<30;
            float next_completion = rdqueue[min_deadline_task]->ret;
            float next_arrival = get_next_arrival(rdqueue, cur_time, nproc, global_tasks) -cur_time;
            //Processor claimed by the job for ∆ = next-min-slack-time - current-slack-time.
            printf("next completion %f", next_completion+cur_time);
            printf("next arrival %f \n", next_arrival+cur_time);
            float next_decision = min(deadline_diff, min(next_completion, next_arrival));
                rdqueue[min_deadline_task]->ret -= next_decision;
                cur_time += next_decision;
            printf("next decision pt%f", next_decision);
            //cur_time++;
            if(rdqueue[min_deadline_task]->ret == 0) {
                printf("deleting%d", min_deadline_task);
                /*FILE *log_file = fopen("sched-op-lst.txt", "a+");
                int response_time = cur_time - cur_proc->task_ref->next_release_time - cur_proc->task_ref->period; 
                fprintf(log_file, "task: %d pid:%d aet: %d RESPONSE TIME: %d ", cur_proc->task_id, cur_proc->pid,
                            cur_proc->aet, response_time);
                fclose(log_file);*/
                //updated anticipatedfloat deadline_diff_arrival list
                //arrival_list_add(cur_proc->task_ref->next_release_time);
                //pqueue_extract_process(rdqueue, cur_proc);
                rdqueue[min_deadline_task] = NULL;
                /**
                *free
                */
                //unlink from job lists
                //remove_job(cur_proc->task_ref, cur_proc);
            }
        }
        else {
            cur_time++;
            cpu_idle_time++;
        }
        /*FILE *log_file = fopen("sched-op-lst.txt", "a+");
            fprintf(log_file, "cache impact: %d", check_cache_impact(cur_task_id, prev_task_id));
        fclose(log_file);*/
        prev_task_id = cur_task_id;
        //update slacks
        update_slack(rdqueue, nproc, cur_time);
        prev_min_deadline_task = min_deadline_task;
    }
    /*FILE *log_file = fopen("sched-op-lst.txt", "a+");
    fprintf(log_file, "cpu idle time %d cpu time utilized %d/%d", cpu_idle_time, hyperperiod - cpu_idle_time, hyperperiod);
    fclose(log_file);*/
}

int main()
{
    submit_processes(&global_tasks, &task_count, &pid_count, &ready_queue);
    display_process(ready_queue, task_count);
    int lcm = get_lcm(global_tasks, task_count);
    printf("%d", get_lcm(global_tasks, task_count));
    //anticipated_arrival = NULL;
    schedule_lst(ready_queue, task_count, lcm);
    return 0;
}