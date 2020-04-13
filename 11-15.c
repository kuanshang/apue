#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>



pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t con = PTHREAD_MUTEX_INITIALIZER;

static int msg = 0;


void process_msg(){

	printf("thread process_msg enter.\n");
	pthread_mutex_lock(&mut);
	while(msg == 0){
		printf("thread while.\n");
		pthread_cond_wait(&con, &mut);
	}

	printf("thread msg:%d.\n", msg);
	msg = 20;
	pthread_mutex_unlock(&mut);
	pthread_cond_signal(&con);
}

void process_msg2(){

	printf("thread2 process_msg enter.\n");
	pthread_mutex_lock(&mut);
	while(msg == 0){
		printf("thread2 while.\n");
		pthread_cond_wait(&con, &mut);
	}

	printf("thread2 msg:%d.\n", msg);

	pthread_mutex_unlock(&mut);
}


void queue_msg (){

	printf("thread queue_msg enter.\n");
	pthread_mutex_lock(&mut);
	msg = 50;
	pthread_mutex_unlock(&mut);
	//pthread_cond_broadcast(&con);
	pthread_cond_signal(&con);
}

int main(){

	pthread_t tid1, tid2, tid3;
	printf("main enter\n");
	
	pthread_create(&tid1, NULL, process_msg, NULL);
	sleep(2);
	pthread_create(&tid3, NULL, process_msg2, NULL);
	sleep(2);
	pthread_create(&tid2, NULL, queue_msg, NULL);

	sleep(2);
	


	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	pthread_join(tid3, NULL);
	pthread_mutex_destroy(&mut);
	pthread_cond_destroy(&con);
	
	exit(0);
}

