//
// Created by Alexis Le Glaunec on 19/11/2020.
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
pthread_cond_t cond;

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
		// Setting up the new command
		struct node* node = malloc(sizeof(struct node));
		memcpy(node->cmd, cmd, 256);

		// Add it to the jobs queue
		pthread_mutex_lock(&lock);
		jobs.tail->next = node;
		jobs.tail = node;
		// tells the slaves that we've added a new command
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&lock);
		// we put it here because it's still a command to be considered by slaves
		if(strncmp(cmd, "quit",4) == 0){
			printf("Quitting master function\n");
			return;
		}
	}
}


void* slave() {
	printf("Slave called\n");
	char cmd[256];
	while(1) {
		memset(cmd, 0, 256);
		pthread_mutex_lock(&lock);
		// wait until master wakes it up if the list is empty
		while(!jobs.head->next) {
			pthread_cond_wait(&cond, &lock);
		}
		// Remove the last command (at head)
		memcpy(cmd, jobs.head->next->cmd, 256);
		jobs.head = jobs.head->next;
		pthread_mutex_unlock(&lock);
		if(strncmp(cmd, "quit",4) == 0){
			printf("Quitting slave function\n");
			pthread_exit(0);
		}
		printf("System call on %s\n", cmd);
		system(cmd);
	}
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