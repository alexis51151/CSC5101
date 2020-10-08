//
// Created by alexis51151 on 08/10/2020.
//
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "tm.h"
#include <stdatomic.h>

int N;
struct memory mem;

void init(){
	// Initialize the memory cells
	for(int i = 0; i < NB_CELLS; i++){
		struct cell* c = mem.cells[i];
		c->value = 0;
		c->counter = 0;
	}
	// Initialize the clock
	mem.clock = 0;
}


void* thread(){
	for(int i =0; i < 1024*1024;i++){
		atomic_fetch_add(mem.cells[0], 1);
	}
	pthread_exit(0);
}


int main(int argc, char** argv) {
	struct timespec start, stop;
	double total_time;

	clock_gettime(CLOCK_MONOTONIC, &start);

	if(argc < 2){
		printf("Please give the nb of threads\n");
		return 1;
	}

	N = atoi(argv[1]); // nb of threads
	printf("N = %d\n", N);

	pthread_t threads[N];
	// Creating and joining the threads
	for(int i = 0; i < N; i++) {
		int* local_i = malloc(sizeof(int));
		*local_i = i;
		pthread_create(&threads[i], NULL, thread, NULL);
	}
	for(int i = 0; i < N; i++) {
		pthread_join(threads[i],NULL);
	}

	// Check consistency of x
	if(mem[0] == 1024*1024*N){
		printf("SUCCESS : x has a consistent value\n");
	}
	else{
		printf("ERROR : x value is not consistent\n");
	}


	clock_gettime(CLOCK_MONOTONIC, &stop);
	total_time = ( stop.tv_sec - start.tv_sec )
	             + ( stop.tv_nsec - start.tv_nsec )
	               / 1000000000;
	printf("Total time = %fs", total_time);
	exit(0);
}
