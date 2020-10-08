//
// Created by alexis51151 on 08/10/2020.
//
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "tm.h"
#include <stdatomic.h> // atomic ops (FAA, CAS)
#include <string.h> //strcmp

int N;
struct memory mem;
pthread_mutex_t lock;
//double total_time;
uint64_t total_time;

void (*loop)();


void atomic_loop(){
	for(int i =0; i < 1024*1024;i++){
		atomic_fetch_add(&mem.cells[0]->value, 1);
	}
}

void lock_loop(){
	for(int i =0; i < 1024*1024;i++){
		pthread_mutex_lock(&lock);
		(mem.cells[0]->value)++;
		pthread_mutex_unlock(&lock);
	}
}





void init(){
	// Initialize the memory cells
	for(int i = 0; i < NB_CELLS; i++){
		struct cell* c = malloc(sizeof(struct cell));
		c->value = 0;
		c->counter = 0;
		mem.cells[i] = c;
	}
	// Initialize the clock
	mem.clock = 0;
}

void* thread(){
	uint64_t start, stop;
	start = read_tscp();
	loop();
	stop = read_tscp();
	total_time += stop - start;
	pthread_exit(0);
}

int main(int argc, char** argv) {


	if(argc < 3){
		printf("Please give 2 args\n");
		return 1;
	}

	N = atoi(argv[1]); // nb of threads
	char * algo = argv[2]; // algorithm used to protect x var

	// data structures init
	pthread_mutex_init(&lock, NULL);
	init();

	// Check which algorithm we wanna use
	if(!strcmp(algo, "atomic")){
		loop = atomic_loop;
	}
	else if (!strcmp(algo, "lock")){
		loop = lock_loop;
	}
	else {
		fprintf(stderr, "unknown algorithm : %s\n", algo);
		exit(1);
	}

	printf("N = %d, using %s algorithm.\n",N, algo);

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
	if(mem.cells[0]->value == 1024*1024*N){
		printf("SUCCESS : x has a consistent value\n");
	}
	else{
		printf("ERROR : x value is not consistent\n");
	}
	printf("Average time per thread : = %lu", total_time/N);
	exit(0);
}
