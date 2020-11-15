//
// Created by Alexis Le Glaunec on 15/11/2020.
//
#define MAX 100

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

// LIFO queue data type
struct queue {
	int tab[MAX];
	int head;
};

// global variables
struct queue q;
// global vars related to the queue (could be added to the struct)
pthread_mutex_t lock =    PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  empty =   PTHREAD_COND_INITIALIZER;
pthread_cond_t  full =    PTHREAD_COND_INITIALIZER;


int push(char e){
	pthread_mutex_lock(&lock);
	while(q.head == MAX){ // active waiting while the queue is full
		pthread_cond_wait(&full, &lock);
	}
	// update the queue
	q.tab[q.head] = e;
	q.head++;
	// send a signal if a pop was waiting
	pthread_cond_signal(&empty);
	pthread_mutex_unlock(&lock);
	return 1;
}

char pop() {
	while(q.head == 0){ // active waiting while the queue is empty
		pthread_cond_wait(&empty, &lock);
	}
	q.head--;
	// send a signal if a push was waiting
	pthread_cond_signal(&full);
	return q.tab[q.head];
}

void* scenario(void* arg) {
	int* i = (int*) arg;
	if(*i == 0){
		pop();
	}
	push(*"a");
	sleep(1);
	printf("pop : %c\n" ,pop());
	pthread_exit(0);
}


int main(int argc, char** argv){

	if(argc < 2){
		printf("Correct call : queue nb_threads\n");
		return 1;
	}

	int nb_threads = atoi(argv[1]);
	for(int i = 0; i < nb_threads; i++){
		pthread_t t;
		pthread_create(&t, NULL, &scenario, &i);
	}

	pthread_exit(0);
}