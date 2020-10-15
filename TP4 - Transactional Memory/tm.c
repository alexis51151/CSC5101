//
// Created by alexis51151 on 08/10/2020.
//
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdatomic.h> // atomic ops (FAA, CAS)
#include <string.h> //strcmp
#include "tm.h"

int N;
struct memory mem;
pthread_mutex_t lock; // used both by lock and trnasaction algorithms
//double total_time;
uint64_t total_time;

// Thread local vars : local memory used in the transaction
// All initialized in startTX
_Thread_local int thread_clock;
_Thread_local int readSet[NB_CELLS];
_Thread_local uintptr_t writeSet[NB_CELLS];

// Transactional memory API

void startTX(){
	thread_clock = mem.clock; // get the time (to unvalidate future changes by other transactions)
	// we reset our local memory
	for(int i = 0; i < NB_CELLS; i++){ // bitarray full of zeros : nothing accessed
		readSet[i] = 0;
		writeSet[i] = (uintptr_t) 0;
	}

}

uintptr_t read(int n){
	// if we've already modified this var in our thread local memory
	if(writeSet[n] !=  0){ // not null pointer
		return writeSet[n];
	}

	// if we've never written in this variable
	struct cell* _Atomic c = atomic_load(&mem.cells[n]);
	if(c->counter >= thread_clock){ // was modified during the transaction execution time
		return MAX_INT;
	}

	readSet[n] = 1; // we keep a flag to show that we have accessed this variable
	return c->value;
}

void write(int n, uintptr_t value){
	writeSet[n] = value;
}

int commitTX(){
	pthread_mutex_lock(&lock);
	for(int n = 0; n < NB_CELLS; n++){
		if(mem.cells[n]->counter >= thread_clock){ // if it was modified during the transaction execution time
			pthread_mutex_unlock(&lock);
			return -1; // abort
		}
	}

	// no conflict found : we can transfer the local thread memory in memory

	for(int i = 0; i < NB_CELLS; i++){
		if(writeSet[i] != 0){ // if we've modified this var
			// create a new pointer

			struct cell* c = malloc(sizeof(struct cell));

			c->value = writeSet[i]; // new value : what we've calculated
			c->counter = mem.clock;
			atomic_store(&mem.cells[i], c);
		}
	}

	mem.clock++;
	pthread_mutex_unlock(&lock);
	return 0;
}


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

void transaction_loop(){
	for(size_t i = 0; i < 1000;i++){
		restart:
		startTX();
		int value = read(0);
		if(value == MAX_INT){
			goto restart;
		}
		write(0, value + 1);
		if(commitTX() != 0){
			goto restart;
		}
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
	mem.clock = 1;
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
	else if(!strcmp(algo, "transaction")){
		loop = transaction_loop;
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
	if(mem.cells[0]->value == 1000*N){
		printf("SUCCESS : x has a consistent value\n");
	}
	else{
		printf("ERROR : x value is not consistent\n");
	}
	printf("Average time per thread : = %lu", total_time/N);
	exit(0);
}
