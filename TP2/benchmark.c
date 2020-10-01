//
// Created by alexis51151 on 24/09/2020.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "spinlock.h"
#include "ticketlock.h"
#include "mcslock.h"
#include "posixlock.h"
#include <string.h>

// For POSIX lock
pthread_mutex_t m;
// For spinlock
int _Atomic lock;
// For ticket lock
struct ticket_lock t;
// For MCS lock
struct node* _Atomic mcs_lock;
// For myposix lock
struct posix_lock myposix_lock;


double time_lock;
double time_unlock;

void acquire(char* algorithm){
	if(!strcmp(algorithm, "posix")){
		pthread_mutex_lock(&m);
	}
	else if(!strcmp(algorithm, "spinlock")){
		spinlock_acquire(&lock);
	}
	else if(!strcmp(algorithm, "ticket")){
		ticketlock_acquire(&t);
	}
	else if(!strcmp(algorithm, "mcs")){
		mcslock_acquire(mcs_lock);
	}
	else if(!strcmp(algorithm, "myposix")){
		posixlock_acquire(&myposix_lock);
	}

	else {

	}
}

void release(char* algorithm){
	if(!strcmp(algorithm, "posix")){
		pthread_mutex_unlock(&m);
	}
	else if(!strcmp(algorithm, "spinlock")){
		spinlock_release(&lock);
	}
	else if(!strcmp(algorithm, "ticket")){
		ticketlock_release(&t);
	}
	else if(!strcmp(algorithm, "mcs")){
		mcslock_release(mcs_lock);
	}
	else if(!strcmp(algorithm, "myposix")){
		posixlock_release(&myposix_lock);
	}
	else {

	}
}



void* benchmark(void* args){

	// Get args
	char** argv = (char**) args;
	int it = atoi(argv[2]);
	int csd = atoi(argv[3]);
	int cs = atoi(argv[4]);
	char* algorithm = argv[5];

	for(int i = 0; i < it;i++){

		clock_t begin_lock = clock();
		acquire(algorithm);
		clock_t end_lock = clock();

		time_lock += (double)(end_lock - begin_lock) / CLOCKS_PER_SEC;

		struct timespec t = {0, csd};
		clock_gettime(CLOCK_MONOTONIC, &t);

		clock_t begin_unlock = clock();
		release(algorithm);
		clock_t end_unlock = clock();

		time_unlock += (double)(end_unlock - begin_unlock) / CLOCKS_PER_SEC;

		t.tv_nsec = cs;
		clock_gettime(CLOCK_MONOTONIC, &t);
	}

	return NULL;
}


int main(int argc, char** argv){
	if(argc < 6){
		printf("Please give 5 args\n");
		return -1;
	}

	// For spinlock
	lock = FREE;
	// For MCS lock
	mcs_lock = NULL;
	// For myposix lock
	myposix_lock.state = FREE_POSIX;

	time_lock = 0;
	time_unlock = 0;

	int n = atoi(argv[1]);
	char* algo = argv[5];
	pthread_t threads[n];

	for(int i = 0; i < n;i++){
		pthread_create(&threads[i], NULL, benchmark, (void *) argv);
	}

	for(int i = 0; i < n;i++){
		pthread_join(threads[i], NULL);
	}

	printf("In average : locking for %f sec, unlocking for %f sec\n", time_lock, time_unlock);

	exit(0);
}