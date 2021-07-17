/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 32173058
*	    Student name : SeongJun-Lee
*
*   lab1_sched_types.h :
*       - lab1 header file.
*       - must contains scueduler algorithm function's declations.
*
*/

#ifndef _LAB1_HEADER_H
#define _LAB1_HEADER_H

/* Process structure */
typedef struct {
   char name;      // process name 
   int arr_t;      // arrival time
   int rem_t;      // remain time
   int exec_t;     // execution time
   int timequantum;// time quatum
   int q_level;    // queue level
   int ticket;     // ticket num
} Process;

/* Ready Queue structure*/
typedef struct {
   Process **queue;    // Queue buffer
   int size;           // Queue size
   int num;            // Number of processes in the queue
   int front;          // Front index of queue
   int rear;           // Rear index of queue
} ready_queue;

/* Global variables */
int ps_num;                 // number of process
int total_time;             // total execution time
char *output;               // record running process
Process proc[10];           // running process arrays
Process temp[10];	     // variables for resetting
ready_queue rq;             // ready queue variables

/*
 * You need to Declare functions in  here
 */
 
/* About queue Push and Pop functions */

/* Print functions */
void print_output(char *out);

void dele(Process *p);                  // delete data from queue
void push_queue(Process *p);            // push process to ready queue
Process *pop_queue();                   // pop data from ready queue

/* Initializing functions */
void input();		            // input value
void init_ready_queue(int size);   // Initializing ready queue
void init();                       // Initializing process
void reset();		            // resetting process information
void sorting_arr(Process *p);      // sort task by arrival time
void total_exec_t();               // setting total_time variable
void set_timequantum(int q);       // setting time quantum

/* Scheduler */
void FCFS();            // first come first serve
void SPN();             // shortest process next
void RR(int q);         // round robin
void MLFQ_1();          // multi level feedback queue (q = 1)
void MLFQ_2i();         // multi lesvel feedback queue (q = 2^i)
void LOTTERY();         // implement at lottery.c

/* For Scheduler functions */
int power(int p);                 // power function
int total_tickets();              // get total tickets
int get_random();                 // get random number
void sorting_exec();              // Sort ready queue by execution time
void sorting_prio();              // Sort ready queue by priority level
void next_prio(Process *p, int i);// priority goes down level (q = 1, q = 2^i)
void running(Process *run);       // Run Program 
void swap(Process *a, Process *b);// Swap Process
Process* winner();                // winner index at lottery scheduling

#endif /* LAB1_HEADER_H*/



