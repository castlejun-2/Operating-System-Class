/*
*   DKU Operating System Lab
*           Lab2 (Vehicle production Problem)
*           Student id :32173058
*           Student name :SeongJun-Lee
*
*   lab2_sync.c :
*       - lab2 main file.
*       - must contains Vehicle production Problem function's declations.
*
*/

#include <aio.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <asm/unistd.h>

#include "lab2_sync_types.h"

/*
* you need to implement Vehicle production Problem.
*/

//producer function
void *producer(void *arg) {
	int j = 0;
	for(j = 0; j<total_produce; j++) {
		pthread_mutex_lock(&mutex);
		while (exec_q.balance == MAX_BUF){
			pthread_cond_wait(&empty, &mutex);
		}
		pthread_mutex_unlock(&mutex);
		put();
		pthread_cond_broadcast(&fill);
	}
	// Loops for wake up consumers who are asleep if the producer finishes first(Deadlock protection)
	while (1) {
		pthread_cond_broadcast(&fill);
	}
}

//consumer function 
void *consumer(void *arg) {
	int i;
	char k = *((char *)arg);
	while(total_rem > 0) {
		//(tatal_rem != 0)is deadlock protection
		pthread_mutex_lock(&mutex);
		while ((exec_q.balance == 0 || exec_q.value[(exec_q.front+1) % (MAX_BUF + 1)]->name != k) && total_rem != 0){
			pthread_cond_wait(&fill, &mutex);
		}
		pthread_mutex_unlock(&mutex);
		char dt = get();
		//The consumer thread took the last data
		pthread_mutex_lock(&mutex);
		if(dt != NULL)
			total_rem--;	
		pthread_mutex_unlock(&mutex);
		pthread_cond_signal(&empty);
	}
}

//put function for producer
void put() {
	pthread_mutex_lock(&mutex);
	if((exec_q.rear+1) % (MAX_BUF+1) == exec_q.front){
		pthread_mutex_unlock(&mutex);	
		assert(0);
	}	
	exec_q.rear = (exec_q.rear+1) % (MAX_BUF+1);
	// get data for ready queue
	exec_q.value[exec_q.rear] = &rq[ready_idx++];
	exec_q.balance++;
	pthread_mutex_unlock(&mutex);
}

//get function for consumer
char get() {
	char d;
	pthread_mutex_lock(&mutex);	
	if(exec_q.front == exec_q.rear || total_rem == 0){
		pthread_mutex_unlock(&mutex);
		return NULL;
	}	
	exec_q.front = (exec_q.front+1) % (MAX_BUF+1);
	d = exec_q.value[exec_q.front]->name;
	exec_q.balance--;
	pthread_mutex_unlock(&mutex);
	return d;
}
