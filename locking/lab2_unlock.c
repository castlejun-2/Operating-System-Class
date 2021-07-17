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

void *producer(void *arg) {
	int j = 0;
	while(ready_idx < MAX_SIZE) {
		while (1){
			if(exec_q.balance == MAX_BUF){
				printf("\n);
				continue;
			}
			else
				break;
		}	
			put();
	}
}

//consumer function 
void *consumer(void *arg) {
	int i;
	char k = *((char *)arg);
	while(ready_idx < MAX_SIZE || exec_q.balance != 0){
		while(1){
			if((exec_q.balance == 0 || exec_q.value([exec_q.front+1) % (MAX_BUF + 1)]->name != k) && total_rem != 0){
				printf("\n");
				continue;
			}
			else
				break;
		}			
		char dt = get();
		if(dt != NULL)
			total_rem--;
	}
}	
