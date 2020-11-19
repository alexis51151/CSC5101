//
// Created by Alexis Le Glaunec on 18/11/2020.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdatomic.h>


struct node {
	struct node* next;
	char cmd[256];
};

// FIFO queue
struct queue {
	struct node head;
	struct node tail;
};

// global variables
struct queue jobs;


void* master(){
	printf("Master called\n");
	pthread_exit(0);
}


void* slave() {
	printf("Slave called\n");
	pthread_exit(0);
}

int main(int argc, char** argv){
	if(argc < 2){
		printf("use : synchro nbThreads \n");
		return 1;
	}
	int n = atoi(argv[1]);
	printf("n = %d\n", n);

	pthread_t threads[n];
	for(int i = 0; i < n; i++){
		pthread_t thread;
		pthread_create(&thread, NULL, slave,NULL);
		threads[i] = thread;
	}
	master();
	for(int i =0 ;i < n; i++){
		pthread_join(threads[i], NULL);
	}
	exit(0);
}