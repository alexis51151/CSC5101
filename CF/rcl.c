//
// Created by Alexis Le Glaunec on 19/11/2020.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdatomic.h>


struct cs {
	void (*func) (int);
	int arg;
	int completed;
};

// global vars
struct cs* pendings;
int var;

// thread vars
__thread struct cs request;

void add(int v){
	var += v;
}

void* master(){
	printf("Master called\n");
	pthread_exit(0);
}


void* slave(void * arg) {
	int index = *((int*) arg);
	printf("Slave called with index %d\n", index);
	while(1) {
		// prepare to ask an execution
		// add 2
		request.func = add;
		request.arg = 2;
		request.completed = 0; // false

		// ask an execution
		pendings[index] = request;
		printf(" %d : Waiting for add(2)\n", index);
		// wait for the execution completion
		while(request.completed != 1);

		// add -1
		request.func = add;
		request.arg = -1;
		request.completed = 0; // false

		// ask an execution
		pendings[index] = request;

		printf("%d : Waiting for add(-1)\n", index);
		// wait for the execution completion
		while(request.completed != 1);
		// returns after execution of add(2) and add(-1)
	}
}

int main(int argc, char** argv){
	if(argc < 2){
		printf("use : synchro nbThreads \n");
		return 1;
	}
	int n = atoi(argv[1]);
	printf("n = %d\n", n);

	pendings = malloc(n*sizeof(struct cs));
	var = 0;

	pthread_t threads[n];
	for(int i = 0; i < n; i++){
		pthread_t thread;
		int* arg = malloc(sizeof(int));
		*arg = i;
		pthread_create(&thread, NULL, slave, (void*) arg);
		threads[i] = thread;
	}
	master();
	for(int i =0 ;i < n; i++){
		pthread_join(threads[i], NULL);
	}
	exit(0);
}