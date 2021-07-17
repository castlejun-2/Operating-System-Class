/*
*   DKU Operating System Lab
*           Lab2 (Vehicle Production Problem)
*           Student id : 32173058
*           Student name : SeongJun-Lee
*
*   lab1_sync_types.h :
*       - lab2 header file.
*       - must contains Vehicle Production Problem's declations.
*
*/

#ifndef _LAB2_HEADER_H
#define _LAB2_HEADER_H

#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#define MAX_SIZE 10000
#define MAX_BUF 10

/*
* You need to Declare functions in  here
*/

typedef struct vehicle {
	char name;		//producer name
	int start_t;		//object production start time
	int produce;		//object produce number
	int rem_pd;		//object remain produce number
	int timequantum;	//object timequantum
}Vehicle;

typedef struct car_queue {
	Vehicle **value;	//Vehicle object value
	int balance;		//queue remain balance
	int front, rear;	//index
}CQ;

int total_produce = 0;		//total produce number
int total_rem = 0;		//remain produce number
int ready_idx = 0;		//for readyqueue index, using round robin
int g_timequantum;		//timequantum

Vehicle rq[MAX_SIZE];		//queue sorted in RR order
Vehicle vh[5];			//producer object
CQ exec_q;			//queue acting as garage(real queue)
CQ c_q;			//queue for RR order

pthread_cond_t fill;	 //consumer flag
pthread_cond_t empty;	 //producer flag
pthread_mutex_t mutex;  //global lock

//Function for queue init
void init();

//Function for input data
void input();

//Function for Producer/Consumer
void *producer(void *arg);
void *consumer(void *arg);
void put();
char get();

//Function to set readyqueue in RR order
void push_queue(Vehicle *v);
Vehicle *pop_queue();
void sorting();
void put_q();

#endif /* LAB2_HEADER_H*/
