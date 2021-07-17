/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32173058
*	    Student name : SeongJun-Lee
*
*   lab1_sched.c :
*       - Lab1 source file.
*       - Must contains scueduler algorithm function'definition.
*
*/

#include <aio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#include "lab1_sched_types.h"

/*
 * you need to implement FCFS, RR, SPN, SRT, HRRN, MLFQ scheduler. 
 */
 
/* print output */
void print_output(char* out) {
   for (int i=0; i < total_time; i++){
      printf("%c ", out[i]);
   }
   printf("\n");
}

/* delete data from queue */
void dele(Process *p) {
   int cnt = rq.num;
   Process *tmp;
   for (int i = 0; i < cnt; i++) {
      tmp = pop_queue();
      if (tmp != p) {
         push_queue(tmp);
      }
   }
}

/* push process to ready queue */
void push_queue(Process *p) {
   rq.rear = (rq.rear + 1) % rq.size;
   if (rq.rear == rq.front) {
      rq.rear--;
   }
   else {
      rq.queue[rq.rear] = p;
   }
   rq.num++;
}

/* pop data from ready queue */
Process *pop_queue() {
   if (rq.front == rq.rear) {
      return NULL;
   }
   rq.front = (rq.front + 1) % rq.size;
   rq.num--;
   return rq.queue[rq.front];
}

void input() {
	char p_id;
	int arr_t, exec_t;
	int i = 0;
	for (i = 0; i < 5; i++) {
		printf("%d Process ID: ",i+1);
		scanf("%c", &p_id);
		printf("%c arrival time, %c service time: ",p_id,p_id);
		scanf("%d,%d",&arr_t, &exec_t);
		getchar();
		temp[i].name = proc[i].name = p_id;
		temp[i].arr_t = proc[i].arr_t = arr_t;
		temp[i].exec_t = proc[i].exec_t = exec_t;
		temp[i].rem_t = proc[i].rem_t = exec_t;
		temp[i].q_level = proc[i].q_level = 0;
		temp[i].ticket = proc[i].ticket = 100;
	}
	ps_num = 5;
}

/* Initializing array of Process structure */
void init() {
   reset();
   sorting_arr(proc);
   total_exec_t();
   init_ready_queue(10);
}

/* Resetting process information */
void reset() {
	for (int i = 0; i < ps_num; i++) {

		proc[i].name = temp[i].name;
		proc[i].arr_t = temp[i].arr_t;
		proc[i].exec_t = temp[i].exec_t;
		proc[i].rem_t = temp[i].rem_t;
		proc[i].q_level = temp[i].q_level;
		proc[i].ticket = temp[i].ticket;
	}
}

/* sort task by arrival time */
void sorting_arr(Process *p) {
   Process tmp;
   for (int i = ps_num - 1; i > 0; i--) {
      for (int j = 0; j < i; j++) {
         if (p[j].arr_t > p[j + 1].arr_t) {
            tmp = p[j + 1];
            p[j + 1] = p[j];
            p[j] = tmp;
         }
         else if (p[j].arr_t == p[j + 1].arr_t && p[j].name > p[j + 1].name) {
            tmp = p[j + 1];
            p[j + 1] = p[j];
            p[j] = tmp;
         }
      }
   }
}

/* setting total exeution time */
void total_exec_t() {
   int sum = 0;
   for (int i = 0; i < ps_num; i++) {
      sum += proc[i].exec_t;
   }
   total_time = sum;
}

/* Initializing ready queue */
void init_ready_queue(int size) {
   rq.rear = size - 1;
   rq.front = size - 1;
   rq.num = 0;
   rq.size = size;
   rq.queue = (Process **)malloc(size * sizeof(Process *));
}

/* setting time quantum */
void set_timequantum(int q) {
   for (int i = 0; i < ps_num; i++) {
      proc[i].timequantum = q;
   }
}

/* first come first served */
void FCFS() {
   init();
   char out[total_time];
   int idx = 0, i = 0;
   int cpu_t = proc[0].arr_t;

   Process *current = &proc[idx++];
   while (i < total_time) {
      while (idx < ps_num && proc[idx].arr_t == cpu_t) {
         push_queue(&proc[idx++]);
      }
      out[i++] = current->name;
      cpu_t++;
      running(current);
      if (current->rem_t == 0) {
         current = pop_queue();
      }
   }

   printf("FCFS        : ");
   print_output(out);
}

/* shortest process next */
void SPN() {
   init();
   char out[total_time];
   int idx = 0, i = 0;
   int cpu_t = proc[0].arr_t;

   Process *current = &proc[idx++];
   while (i < total_time) {
      while (idx < ps_num && proc[idx].arr_t == cpu_t) {
         push_queue(&proc[idx++]);
      }
      sorting_exec();
      out[i++] = current->name;
      cpu_t++;
      running(current);
      if (current->rem_t == 0) {
         current = pop_queue();
      }
   }
   printf("SPN         : ");
   print_output(out);
}

/* round robin */
void RR(int q) {
   init();
   char out[total_time];
   int idx = 0, i = 0;
   int cpu_t = proc[0].arr_t;
   int flag = 0;   
   Process *current = &proc[idx++];
   Process *tmp;
   set_timequantum(q);

   while (i < total_time) {
      while (idx < ps_num && proc[idx].arr_t == cpu_t) {
         push_queue(&proc[idx++]);
      }
      if (flag == 1) { 
         push_queue(tmp);
         current = pop_queue();
         flag = 0;
      }
      out[i++] = current->name;
      cpu_t++;
      running(current);
      if (current->rem_t <= 0) {
         current = pop_queue();
      }
      else if (current->timequantum <= 0) {
         tmp = current;
         tmp->timequantum = q;
         flag = 1;
      }
   }
   printf("RR(q=%d)     : ", q);
   print_output(out);
}

/* Multi Level Feedback Queue */
void MLFQ_1() {
   init();
   char out[total_time];
   int idx = 0, i = 0;
   int cpu_t = proc[0].arr_t;
   int flag = 0; 
   Process *current = &proc[idx++];
   set_timequantum(1);

   while (i < total_time) {
      while (idx < ps_num && proc[idx].arr_t == cpu_t) {
         push_queue(&proc[idx++]);
      }
      if (flag == 1) {
         if (rq.num > 0) {
            next_prio(current, 1);
            push_queue(current);
            sorting_prio();
            current = pop_queue();
         }
         else {
            current->timequantum = 1;
         }
         flag = 0;
      }
      if (flag == 2) {
         sorting_prio();
         current = pop_queue();
         flag = 0;
      }
      out[i++] = current->name;
      cpu_t++;
      running(current);
      if (current->rem_t <= 0) {
         flag = 2;
      }
      else if (current->timequantum <= 0) {
         flag = 1;
      }
   }
   printf("MLFQ(q=1)   : ");
   print_output(out);
}

void MLFQ_2i() {
   init();
   char out[total_time];
   int idx = 0, i = 0;
   int cpu_t = proc[0].arr_t;
   int flag = 0;
   Process *current = &proc[idx++];
   set_timequantum(1);

   while (i < total_time) {
      while (idx < ps_num && proc[idx].arr_t == cpu_t) {
         push_queue(&proc[idx++]);
      }
      if (flag == 1) {
         if (rq.num > 0) {
            next_prio(current, 2);
            push_queue(current);
            sorting_prio();
            current = pop_queue();
         }
         else {
            current->timequantum = power(current->q_level);
         }
         flag = 0;
      }
      if (flag == 2) {
         sorting_prio();
         current = pop_queue();
         flag = 0;
      }
      out[i++] = current->name;
      cpu_t++;
      running(current);
      if (current->rem_t <= 0) {
         flag = 2;
      }
      else if (current->timequantum <= 0) {
         flag = 1;
      }
   }
   printf("MLFQ(q=2^i) : ");
   print_output(out);
}

/* LOTTERY Scheduler */
void LOTTERY() {
   init();
   char out[total_time];
   int idx = 0, i = 0;
   int cpu_t = proc[0].arr_t;
   int complete = 0;

   Process *current;
   push_queue(&proc[0]);
   while (i < total_time) {
      while (idx < ps_num && proc[idx].arr_t == cpu_t) {
         push_queue(&proc[idx++]);
      }
      current = winner();
      running(current);
      out[i++] = current->name;
      cpu_t++;
      if (current->rem_t <= 0) dele(current);
   }
   printf("LOTTERY     : ");
   print_output(out);
}

int power(int p) {
   int res = 1;
   for (int i = 0; i < p; i++) {
      res *= 2;
   }
   return res;
}

/* Get total tickets of current ready queue */
int total_tickets() {
   int total = 0;
   for (int i = 1; i <= rq.num; i++) {
      int index = (rq.front + i) % rq.size;
      total += rq.queue[index]->ticket;
   }
   return total;
}

/* Get random number from a to b*/
int get_random() {
   int r_num;
   int total = total_tickets();
   if (total == 0) return 0;
   r_num = rand() % total;

   return r_num;
}

/* Sort ready queue by execution time */
void sorting_exec() {
   int exec_temp1;
   int exec_temp2;
   int *idx = (int*)malloc(sizeof(int)*rq.num);

   for (int i = 0; i < rq.num; i++) {
      idx[i] = (rq.front + i + 1) % rq.size;
   }

   for (int i = rq.num - 1; i > 0; i--) {
      for (int j = 0; j < i; j++) {
         exec_temp1 = rq.queue[idx[j]]->exec_t;
         exec_temp2 = rq.queue[idx[j + 1]]->exec_t;
         
         if (exec_temp1 > exec_temp2) {
            swap(rq.queue[idx[j]], rq.queue[idx[j + 1]]);
         }
      }
   }
   free(idx);
}

/* Sort ready queue by priority level */
void sorting_prio() {
   Process *temp_arr[rq.num + 1];
   int next = 1;
   int index = 0;
   while (next <= rq.num) {
      for (int i = 0; i < 10; i++) {
         for (int j = 1; j <= rq.num; j++) {
            index = (rq.front + j) % rq.size;
            
            if (i == rq.queue[index]->q_level) {
               temp_arr[next++] = rq.queue[index];
            }
         }
      }
   }

   for (int i = 1; i <= rq.num; i++) {
      index = (rq.front + i) % rq.size;
      rq.queue[index] = temp_arr[i];
   }

}

/* priority goes down level (q = 1, q = 2^i) */
void next_prio(Process *p, int i) {
   Process *tmp = p;
   p->q_level++;
   if (i == 1) {
      p->timequantum = 1;
   }
   else if (i == 2) {
      p->timequantum = power(p->q_level);
   }
   else {
      printf("error");
      return;
   }
}

/* For Run Program */
void running(Process *run) {
   run->rem_t--;
   run->timequantum--;
}

/* Swapping function */
void swap(Process *a, Process *b) {
   Process tmp = *a;
   *a = *b;
   *b = tmp;
}

/* winner index at lottery scheduling */
Process* winner() {
   int cnt = 0;
   int index = 0;
   int winner = get_random();
   for (int i = 1; i <= rq.num; i++) {
      index = (rq.front + i) % rq.size;
      cnt += rq.queue[index]->ticket;
      if (cnt > winner) {
         return rq.queue[index];
      }
   }
}


