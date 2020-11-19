//
// Created by Alexis Le Glaunec on 18/11/2020.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdatomic.h>
#include <unistd.h>
#include <string.h>


struct node {
	struct node* next;
	char cmd[256];
};

// FIFO queue
struct queue {
	struct node* head;
	struct node* tail;
};

// global variables
struct queue jobs;
pthread_mutex_t lock;

// FIFO initialization
void init() {
	jobs.head = jobs.tail = malloc(sizeof(struct node));  // fake node
	jobs.head->next = 0;
}

void master(){
	printf("Master called\n");
	char cmd[256];
	while(1){
		memset(cmd, 0, 256);
		read(0, cmd, 256);
		if(strncmp(cmd, "quit",4) == 0){
			printf("Quitting master function\n");
			return;
		}
		// Setting up the new command
		struct node* node = malloc(sizeof(struct node));
		printf("cmd = %s\n", cmd);
		memcpy(node->cmd, cmd, 256);

		// Add it to the jobs queue
		pthread_mutex_lock(&lock);
		jobs.tail->next = node;
		jobs.tail = node;
		pthread_mutex_unlock(&lock);

	}
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

	init();

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