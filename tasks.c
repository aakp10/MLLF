#include "tasks.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

task *
task_init(int task_id, int wcet, int period, int deadline)
{
    task *temp = (task *)malloc(sizeof(task));
    temp->task_id = task_id;
    temp->wcet = wcet;
    temp->period = period;
    temp->deadline = deadline;
    temp->next_release_time = period;
    temp->job = NULL;
    temp->job_index = 1;
    //phase difference ignored
    temp->arrival[0] = 0;
    return temp;
}

process *
process_init(int pid_v, int wcet_v, int task_id, task *task_ref)
{
    process *temp = (process *)malloc(sizeof(process));
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

void submit_processes(task ***global_tasks, int *task_count, int *pid_count, process ***rq, float *util)
{
    FILE *task_file = fopen("input5", "r");
    int wcet, period, deadline;
    int task_no = 0;
    int process_count;
    fscanf(task_file, "%d", &process_count);
    printf("%d", process_count);
    //pqueue *ready_queue = pqueue_init(process_count, MAXPROCESS);
    task **taskset = (task **)malloc(process_count * sizeof(task *));
    process **ready_queue = (process **)malloc(process_count * sizeof(process *));
    //wcet, period, deadline
    while (fscanf(task_file, "%d, %d, %d", &wcet, &period, &deadline) == 3)
    {
        printf("%d, %d, %d\n", wcet, period, deadline);
        task *t = task_init((*task_count)++, wcet, period, deadline);
        process *p = process_init((*pid_count)++, wcet, (*task_count) - 1, t);
        //no phase
        t->job = p;
        taskset[(*task_count) - 1] = t;
        ready_queue[(*task_count) - 1] = p;
        *util += (float)wcet / period;
    }
    fclose(task_file);
    *global_tasks = taskset;
    *rq = ready_queue;
}

void display_process(process **ready_queue, int size)
{
    printf("READY PROCESSES\n");
    for (int i = 0; i < size; i++)
    {
        printf("pid:%d deadline:%d\n", ready_queue[i]->pid, ready_queue[i]->task_ref->deadline);
    }
    printf("END\n");
}

static int
get_gcd(int num1, int num2)
{
    if (num2 == 0)
        return num1;
    return get_gcd(num2, num1 % num2);
}

int get_lcm(task **global_tasks, int task_count)
{
    int hyper_period = 1;
    for (int i = 0; i < task_count; i++)
    {
        task *p2 = global_tasks[i];
        //printf("%d, ", p2->period);
        hyper_period = (hyper_period * p2->period) / get_gcd(hyper_period, p2->period);
    }
    return hyper_period;
}

float get_min_lax(process **ready_queue, int task_count)
{
    float min_lax = 1 << 30;
    for (int i = 0; i < task_count; i++)
    {
        if (ready_queue[i])
        {
            printf("lax %d = %d", i, ready_queue[i]->slack);
            min_lax = min(min_lax, ready_queue[i]->slack);
        }
    }
    return min_lax;
}

int get_min_lax_procs(process **ready_queue, int task_count)
{
    float min_lax = get_min_lax(ready_queue, task_count);
    int *proc_list = (int *)malloc(task_count * sizeof(int));
    int min_deadline = 1 << 30;
    int to_return = -1;
    for (int i = 0, j = 0; i < task_count; i++)
    {
        if (ready_queue[i] != NULL && ready_queue[i]->slack == min_lax)
        {

            //proc_list[j++] = i;
            min_deadline = min(min_deadline, ready_queue[i]->task_ref->deadline);
            if (min_deadline == ready_queue[i]->task_ref->deadline)
                to_return = i;
        }
    }
    printf("Min lax proc is %d", to_return);
    return to_return;
}

int get_min_deadline_lax(int least_lax_proc[])
{
    //int size
    //for(int i = 0;)
}

int get_next_edf(int min_deadline_task, process **rdqueue, int nproc)
{
    int min_lax = 1 << 30;
    int min_index = -1;
    for (int i = 0; i < nproc; i++)
    {
        if (rdqueue[i] && rdqueue[i]->slack > rdqueue[min_deadline_task]->slack)
        {
            if (min_lax > rdqueue[i]->task_ref->deadline)
            {
                min_lax = rdqueue[i]->task_ref->deadline;
                min_index = i;
            }
        }
    }
    return min_index;
}

float get_next_arrival(process **rdqueue, int cur_time, int nproc, task **taskset)
{
    float min_arrival = 1 << 30;
    for (int i = 0; i < nproc; i++)
    {
        if (rdqueue[i] == NULL)
        {
            int arr = ceilf((float)cur_time / taskset[i]->period);
            printf("arr in arrival %d", arr);
            arr *= taskset[i]->period;
            min_arrival = min(min_arrival, arr);
        }
    }
    return min_arrival;
}

cache check_cache_impact(int cur_task_id, int prev_task_id)
{
    if (cur_task_id == prev_task_id)
        return NO_CACHE_IMPACT;
    else
        return CACHE_IMPACT;
}

void update_slack(process **rdqueue, int nproc, int cur_time)
{
    int prev_slack;
    for (int i = 0; i < nproc; i++)
    {
        process *cur_proc = rdqueue[i];
        if (cur_proc == NULL)
            continue;
        prev_slack = cur_proc->slack;
        cur_proc->slack = cur_proc->task_ref->deadline - cur_time - cur_proc->ret;
        printf("pid:%d priority: %d slack %d \n", cur_proc->pid, cur_proc->priority, cur_proc->slack);
    }
    printf("END\n");
}

void check_arrivals(process **ready_queue, task **global_tasks, int cur_time, int nproc, int *pid_count)
{
    for (int i = 0; i < nproc; i++)
    {
        if (ready_queue[i] == NULL && global_tasks[i]->next_release_time /*period*/ <= cur_time)
        {
            printf("Entered check_arrivals for %d at %d", i, cur_time);
            //update release time
            global_tasks[i]->next_release_time += global_tasks[i]->period;
            //deadline is considered same as period
            global_tasks[i]->deadline += global_tasks[i]->period;
            global_tasks[i]->job_index += 1;
            global_tasks[i]->arrival[global_tasks[i]->job_index - 1] = cur_time;
            printf("Deadline updated for task %d is %d", i, global_tasks[i]->deadline);
            //recreate the process .
            process *p = process_init((*pid_count), global_tasks[i]->wcet,
                                      global_tasks[i]->task_id, global_tasks[i]);
            //decrease the slack by cur_time
            *pid_count += 1;
            p->slack -= cur_time;
            ready_queue[i] = p;
        }
    }
}

void schedule_mllf(process **rdqueue, int nproc, int hyperperiod, task **global_tasks, int *pid_count)
{
    int cur_time = 0;
    int prev_task_id = -1;
    int cur_task_id = -1;
    int cpu_idle_time = 0;
    int prev_min_deadline_task = -1;
    int preemption_count = 0;
    int cache_impact = 0;
    int prev_pid = -1;
    int context_switches = 0;
    while (cur_time < hyperperiod)
    {
        /**
        * FIXME: new arrivals
        */
        //preemption_count++;
        check_arrivals(rdqueue, global_tasks, cur_time, nproc, pid_count);
        //int *least_lax_procs = get_min_lax_procs(rdqueue, nproc);
        //find min deadline job among least_lax_procs
        int min_deadline_task = get_min_lax_procs(rdqueue, nproc);
        //printf("\nMin deadline task : %d",min_deadline_task);
        if (min_deadline_task != -1)
        {
            if (prev_min_deadline_task != -1 && rdqueue[prev_min_deadline_task] && rdqueue[min_deadline_task]->slack == rdqueue[prev_min_deadline_task]->slack && rdqueue[min_deadline_task]->task_ref->deadline > rdqueue[prev_min_deadline_task]->task_ref->deadline)
            {
                min_deadline_task = prev_min_deadline_task;
            }
        }

        //find next least deadline more than this task and laxity more than this
        int next_least_deadline_task = get_next_edf(min_deadline_task, rdqueue, nproc);
        if (min_deadline_task != -1)
        {
            int cur_pid = rdqueue[min_deadline_task]->pid;
            if (prev_pid != -1 && cur_pid != prev_pid)
            {
                preemption_count++;
            }
            prev_pid = cur_pid;
            cur_task_id = rdqueue[min_deadline_task]->task_id;
            //printf("time:%d process executing: %d\n", cur_time, cur_proc->pid);
            FILE *schedule_file = fopen("schedule.txt", "a+");
            //int laxity = rdqueue[min_deadline_task]->task_ref->deadline - cur_time - rdqueue[min_deadline_task]->ret;
            fprintf(schedule_file, "\ntime:%d job executing: T%d-%d\n", cur_time, cur_task_id, global_tasks[cur_task_id]->job_index);
            //printf("time:%d process executing: %d actual execution time = %d laxity: %d\n", cur_time, cur_proc->pid, cur_proc->aet, laxity);
            fclose(schedule_file);
            //find the next least slack time job
            float deadline_diff;
            printf(" next edf%d\n", next_least_deadline_task);
            if (next_least_deadline_task != -1)
                deadline_diff = rdqueue[next_least_deadline_task]->slack - rdqueue[min_deadline_task]->slack;
            else
                deadline_diff = 1 << 30;
            float next_completion = rdqueue[min_deadline_task]->ret;
            float next_arrival = get_next_arrival(rdqueue, cur_time, nproc, global_tasks) - cur_time;
            //Processor claimed by the job for ∆ = next-min-slack-time - current-slack-time.
            //printf("\n min deadline task %d",min_deadline_task);
            //printf("\n prev min deadline task %d",prev_min_deadline_task);
            printf(" next completion %f", next_completion + cur_time);
            printf(" next arrival %f \n", next_arrival + cur_time);

            //printf(" \nnext least deadline task deadline %d \n", rdqueue[next_least_deadline_task]->task_ref->deadline);
            //printf(" \nmin deadline task %d and its slack %d \n",min_deadline_task,rdqueue[min_deadline_task]->slack);
            //printf(" \ndeadline diff %f \n", deadline_diff);
            //printf(" \nnext arrival for min %f \n", next_arrival);
            //printf(" \nnext completion for min %f \n", next_completion);
            float next_decision = min(deadline_diff, min(next_completion, next_arrival));
            rdqueue[min_deadline_task]->ret -= next_decision;
            cur_time += next_decision;
            printf(" next decision pt%f", next_decision);
            //cur_time++;
            if (rdqueue[min_deadline_task]->ret == 0)
            {
                printf(" deleting%d", min_deadline_task);
                /*FILE *log_file = fopen("sched-op-lst.txt", "a+");
                int response_time = cur_time - cur_proc->task_ref->next_release_time - cur_proc->task_ref->period; 
                fprintf(log_file, "task: %d pid:%d aet: %d RESPONSE TIME: %d ", cur_proc->task_id, cur_proc->pid,
                            cur_proc->aet, response_time);
                fclose(log_file);*/
                //updated anticipatedfloat deadline_diff_arrival list
                //arrival_list_add(cur_proc->task_ref->next_release_time);
                //pqueue_extract_process(rdqueue, cur_proc);
                int job_index = global_tasks[cur_task_id]->job_index;
                global_tasks[cur_task_id]->response_time[job_index - 1] = cur_time - global_tasks[cur_task_id]->arrival[job_index - 1];
                FILE *schedule_file = fopen("schedule.txt", "a+");
                //int laxity = rdqueue[min_deadline_task]->task_ref->deadline - cur_time - rdqueue[min_deadline_task]->ret;
                fprintf(schedule_file, "job : T%d-%d response time %f", cur_task_id, global_tasks[cur_task_id]->job_index, global_tasks[cur_task_id]->response_time[job_index - 1]);
                //printf("time:%d process executing: %d actual execution time = %d laxity: %d\n", cur_time, cur_proc->pid, cur_proc->aet, laxity);
                fclose(schedule_file);
                rdqueue[min_deadline_task] = NULL;
                context_switches++;
                //calculate_parameters(global_tasks, cur_task_id, cur_time);
                /**
                *free
                */
                //unlink from job lists
                //remove_job(cur_proc->task_ref, cur_proc);
            }
        }
        else
        {
            FILE *schedule_file = fopen("schedule.txt", "a+");
            //int laxity = rdqueue[min_deadline_task]->task_ref->deadline - cur_time - rdqueue[min_deadline_task]->ret;
            fprintf(schedule_file, "\ntime:%d job executing: Idle\n", cur_time);
            //printf("time:%d process executing: %d actual execution time = %d laxity: %d\n", cur_time, cur_proc->pid, cur_proc->aet, laxity);
            fclose(schedule_file);
            cur_time++;
            cpu_idle_time++;
        }
        /*FILE *log_file = fopen("sched-op-lst.txt", "a+");
            fprintf(log_file, "cache impact: %d", check_cache_impact(cur_task_id, prev_task_id));
        fclose(log_file);*/
        if (prev_task_id != -1 && check_cache_impact(prev_task_id, cur_task_id) == CACHE_IMPACT)
        {
            cache_impact++;
        }
        prev_task_id = cur_task_id;
        //update slacks
        update_slack(rdqueue, nproc, cur_time);
        prev_min_deadline_task = min_deadline_task;
    }
    FILE *log_file = fopen("sched-op-mllf.txt", "a+");
    fprintf(log_file, "cpu idle time %d cpu time utilized %d/%d preemption point %d context switches %d\n cache impact %d\n", cpu_idle_time, hyperperiod - cpu_idle_time, hyperperiod, preemption_count, context_switches, cache_impact);
    fclose(log_file);
}

static void
get_absolute_response_jitter(task *t)
{
    float max_response_time = t->wcet;
    float min_response_time = 1 << 30;
    float *response_time = t->response_time;
    for (int i = 0; i < t->job_index; i++)
    {
        if (max_response_time < response_time[i])
            max_response_time = response_time[i];
        if (min_response_time > response_time[i])
            min_response_time = response_time[i];
    }
    //printf("response of T%d max %f min %f", t->task_id, max_response_time, min_response_time);
    FILE *log_file = fopen("sched-op-mllf.txt", "a+");
    fprintf(log_file, "Task %d absolute response time jitter %f\n", t->task_id, max_response_time - min_response_time);
    fclose(log_file);
}

static void
get_relative_response_jitter(task *t)
{
    float *response_time = t->response_time;
    float max_jitter = abs(response_time[0] - response_time[t->job_index - 1]);

    for (int i = 0; i < t->job_index - 1; i++)
    {
        float response_diff = abs(response_time[i] - response_time[i + 1]);
        max_jitter = max_jitter < response_diff ? response_diff : max_jitter;
    }
    FILE *log_file = fopen("sched-op-mllf.txt", "a+");
    fprintf(log_file, "Task %d relative response time jitter %f\n", t->task_id, max_jitter);
    fclose(log_file);
}

static void
get_avergae_wait_time(task *t)
{
    float *response_time = t->response_time;
    float avg = 0;
    float wcet = t->wcet;
    for (int i = 0; i < t->job_index - 1; i++)
    {
        avg += response_time[i] - wcet;
    }
    avg /= t->job_index;
    FILE *log_file = fopen("sched-op-mllf.txt", "a+");
    fprintf(log_file, "Task %d average wait time %f\n", t->task_id, avg);
    fclose(log_file);
}

void calculate_parameters(task **global_tasks, int task_count)
{
    for (int i = 0; i < task_count; i++)
    {
        get_absolute_response_jitter(global_tasks[i]);
        get_relative_response_jitter(global_tasks[i]);
        get_avergae_wait_time(global_tasks[i]);
    }
}
