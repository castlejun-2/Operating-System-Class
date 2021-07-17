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

void lab2_sync_usage(char *cmd) {
	printf("\n Usage for %s : \n", cmd);
	printf("    -c: Total number of vehicles produced, must be bigger than 0 ( e.g. 100 )\n");
	printf("    -q: RoundRobin Time Quantum, must be bigger than 0 ( e.g. 1, 4 ) \n");
}

void lab2_sync_example(char *cmd) {
	printf("\n Example : \n");
	printf("    #sudo %s -c=100 -q=1 \n", cmd);
	printf("    #sudo %s -c=10000 -q=4 \n", cmd);
}

int main(int argc, char* argv[]) {
	pthread_t producerthread;
	pthread_t consumerthread[5];
	int sum = 0;
	int n; char junk;
	char g_name[5] = { 'a','b','c','d','e' };
	char *a = g_name;
	struct timeval start, end;
	double result_T;

	if (argc <= 1) {
		lab2_sync_usage(argv[0]);
		lab2_sync_example(argv[0]);
		exit(0);
	}

	for (int i = 1; i < argc; i++) {
		if (sscanf(argv[i], "-c=%d%c", &n, &junk) == 1) {
			total_rem = total_produce = n;
		}
		else if (sscanf(argv[i], "-q=%d%c", &n, &junk) == 1) {
			g_timequantum = n;
		}
		else {
			lab2_sync_usage(argv[0]);
			lab2_sync_example(argv[0]);
			exit(0);
		}
	}
	//get timequantum value in vh[]
	for (int j = 0; j < 5; j++)
		vh[j].timequantum = g_timequantum;
	n = total_produce;
	
	init(n);
	input();
	sorting();
	put_q();
	
	for (int j = 0; j < 5; j++)
		sum+=vh[j].produce;
	if(sum!=total_produce){
		printf("usage: input vh[] produce == total produce\n");
		return 0;
	}
	printf("======Before Information======\n");
	printf("Total produce number = %d\n", total_produce);
	printf("Remain Produce number = %d\n", total_rem);
	gettimeofday(&start, NULL);
	pthread_create(&producerthread, NULL, producer, NULL);
	for (int i = 0; i < 5; i++)
		pthread_create(&consumerthread[i], NULL, consumer, a+i);
	
	for (int i = 0; i < 5; i++)
		pthread_join(consumerthread[i], NULL);
	gettimeofday(&end, NULL);

	result_T = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) / 1000000);
	printf("\n==Finish Coarse-grained Lock Experiment==\n");
	printf("Total produce number = %d\n", total_produce);
	printf("Final Balance Value = %d\n", exec_q.balance);
	printf("Remain Produce number = %d\n", total_rem);
	printf("Execution time = %f\n", result_T);

	return 0;
}


/*
* You need to implement Vehicle production Problem
* If you want to use other header file, it's ok!
* If you have any question for this task, please contact me (ghwls03s@gmail.com)
* You may be need to use pthread_create function
*/

// Init function
void init(){
	//for ready queue init
	c_q.value = (Vehicle **)malloc(MAX_BUF * sizeof(Vehicle *));
	c_q.rear = c_q.front = MAX_BUF - 1;
	c_q.balance = 0;
	
	//for execution queue init
	exec_q.value = (Vehicle **)malloc((MAX_BUF + 1) * sizeof(Vehicle *));
	exec_q.rear = exec_q.front = MAX_BUF;
	exec_q.balance = 0;
}

// Input Value for vh[] function
void input() {
	int i;
	char name;
	int s_time=0, pd_num=0;
	printf("====Input Production Object Value===\n");
	for (i = 0; i < 5; i++) {
		printf("vh[%d] Name: ",i);
		scanf("%c", &name);
		printf("vh[%d] Start Time, Produce Number: ",i);
		scanf("%d %d", &s_time, &pd_num);
		getchar();
		vh[i].name = name;
		vh[i].start_t = s_time;
		vh[i].produce = vh[i].rem_pd = pd_num;
	}	
}

//producer function
void *producer(void *arg) {
	int j = 0;
	for(j = 0; j<total_produce; j++) {
		pthread_mutex_lock(&mutex);
		while (exec_q.balance == MAX_BUF)
			pthread_cond_wait(&empty, &mutex);
		put();
		pthread_cond_broadcast(&fill);
		pthread_mutex_unlock(&mutex);
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
		pthread_mutex_lock(&mutex);
		//(tatal_rem != 0)is deadlock protection
		while ((exec_q.balance == 0 || exec_q.value[(exec_q.front+1) % (MAX_BUF + 1)]->name != k) && total_rem != 0)
			pthread_cond_wait(&fill, &mutex);
		char dt = get();
		//The consumer thread took the last data
		if(dt != NULL)
			total_rem--;
		pthread_cond_signal(&empty);
		pthread_mutex_unlock(&mutex);
	}
}

//put function for producer
void put() {
	if((exec_q.rear+1) % (MAX_BUF+1) == exec_q.front)
		assert(0);
	exec_q.rear = (exec_q.rear+1) % (MAX_BUF+1);
	// get data for ready queue
	exec_q.value[exec_q.rear] = &rq[ready_idx++];
	exec_q.balance++;
}

//get function for consumer
char get() {
	char d;
	if(exec_q.front == exec_q.rear || total_rem == 0)
		return NULL;
	exec_q.front = (exec_q.front+1) % (MAX_BUF+1);
	d = exec_q.value[exec_q.front]->name;
	exec_q.balance--;
	return d;
}

//Function to set readyqueue in RR order
void push_queue(Vehicle *v) {
   c_q.rear = (c_q.rear + 1) % MAX_BUF;
   if (c_q.rear == c_q.front)
      c_q.rear--;
   else 
      c_q.value[c_q.rear] = v;
}

//pop data from ready queue
Vehicle *pop_queue() {
   if (c_q.front == c_q.rear) {
      return NULL;
   }
   c_q.front = (c_q.front + 1) % MAX_BUF;
   return c_q.value[c_q.front];
}
void put_q() {
   int idx = 0 ,i = 0;
   int cpu_t = vh[0].start_t;
   int flag = 0;   
   Vehicle *current = &vh[idx++];
   Vehicle *tmp;
   while (i < total_produce) {
      while (idx < 5 && vh[idx].start_t == cpu_t) {
	    push_queue(&vh[idx++]);
      }
      if (flag == 1) { 
	    push_queue(tmp);
	    current = pop_queue();
	    flag = 0;    
      }
      rq[i++] = *current;
      cpu_t++;
      current->rem_pd--;
      current->timequantum--;
      if (current->rem_pd <= 0) {
         current = pop_queue();
      }
      else if (current->timequantum <= 0) {
         tmp = current;
         tmp->timequantum = g_timequantum;
         flag = 1;
      }
   }
}

//order by production start time
void sorting() {
	Vehicle tmp;
	for (int i = 0; i < 4; i++) {
		for (int j = i + 1; j < 5; j++) {
			if (vh[i].start_t > vh[j].start_t) {
				tmp = vh[i];
				vh[i] = vh[j];
				vh[j] = tmp;
			}
			else if (vh[i].start_t == vh[j].start_t) {
				if (vh[i].name < vh[j].name) {
					tmp = vh[i];
					vh[i] = vh[j];
					vh[j] = tmp;
				}
			}
		}
	}	
}
