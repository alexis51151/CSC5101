//
// Created by Alexis Le Glaunec on 15/11/2020.
//
#define MAX 100

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdatomic.h>

// LIFO queue data type
struct queue {
	int tab[MAX];
	int* _Atomic head;
};

// global variables
struct queue q;
// global vars related to the queue (could be added to the struct)
pthread_mutex_t lock =    PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  empty =   PTHREAD_COND_INITIALIZER;
pthread_cond_t  full =    PTHREAD_COND_INITIALIZER;


int push(char e){
	int* head;
	do {
		while(*q.head == MAX){ // active waiting

		}
		head = q.head; // the last read head
	} while(!atomic_compare_exchange_strong(q.head, head, *head+1));
	return 1;
}

char pop() {
	int* head;
	char e;
	do {
		while(*q.head == 0){ // active waiting

		}
		head = q.head; // the last read head
		e = q.tab[*q.head];
	} while(!atomic_compare_exchange_strong(q.head, head, *head-1));
	return e;
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
	int head = 0;
	q.head = &head;
	int nb_threads = atoi(argv[1]);
	for(int i = 0; i < nb_threads; i++){
		pthread_t t;
		pthread_create(&t, NULL, &scenario, &i);
	}
	pthread_exit(0);
}