//
// Created by alexis51151 on 01/10/2020.
//

//
// Created by alexis51151 on 01/10/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>
#include <time.h>

#define MAX_ITEMS 100



struct node{
	struct node* next;
	int elt;
};

struct stack {
	struct node* _Atomic head;
};

struct queue{
	struct node* _Atomic head;
	struct node* _Atomic tail;
};

// Global vars
struct queue q;
int K;

void enqueue(struct queue* q, int e){
	struct node* _Atomic n = malloc(sizeof(struct node));
	struct node* _Atomic old;
	struct node** null_node = malloc(sizeof(struct node*));
	*null_node = 0;
	n->elt = e;
	do {
		old = q->tail;
		while(old->next != NULL){ // old is no more the tail (or empty queue)
			atomic_compare_exchange_strong(&q->tail, &old, old->next); // we try to change the tail
			old = q->tail;
		}
	} while (!atomic_compare_exchange_strong(&old->next, null_node, n)); // as long as we haven't inserted n

	// then we finally try to update the tail (if old is still the tail)
	// (if we cannot, someone else has already done it for us)
	atomic_compare_exchange_strong(&q->tail, &old, n);
}

int dequeue(struct queue* q){
	struct node* res;
	do {
		res = q->head;
		if(res->next == NULL){ // empty queue
			return 0;
		}
		// not empty queue
	} while(!atomic_compare_exchange_strong(&q->head, &res, res->next)); // as long as we can't dequeue
	free(res->next);
	return res->next->elt;
}


void queue_state(struct queue* q){
	struct node* curr = q->head;
	while(curr != NULL){
		printf("-%d-", curr->elt);
		curr = curr->next;
	}
	printf("\n");
}

void* thread(){
	for(int i = 0; i < K;i++){
		enqueue(&q, 2);
		dequeue(&q);
	}
	/*
	for(int i = 0; i < K;i++){
		dequeue(&q);
	}
	*/
	pthread_exit(0);
}

int main(int argc, char** argv) {
	struct timespec start, stop;
	double total_time;

	clock_gettime(CLOCK_MONOTONIC, &start);
	// queue initialization
	struct node* fake = malloc(sizeof(struct node)); // fake node to handle empty queue
	fake->next = NULL;
	fake->elt = -1;
	q.head = fake;
	q.tail = fake;


	if(argc < 3){
		printf("Please give 2 argument\n");
		return 1;
	}

	int N = atoi(argv[1]); // nb of threads
	K = atoi(argv[2]); // nb of iterations
	printf("N = %d, K = %d\n", N, K);

	pthread_t threads[N];
	// Creating and joining the threads
	for(int i = 0; i < N; i++) {
		pthread_create(&threads[i], NULL, thread, NULL);
	}

	for(int i = 0; i < N; i++) {
		pthread_join(threads[i],NULL);
	}

	//Debugging
	queue_state(&q);

	// Little check on consistency
	if(q.head == q.tail){
		printf("Queue successful\n");
	}
	else{
		printf("Queue unsuccessful\n");
	}
	clock_gettime(CLOCK_MONOTONIC, &stop);
	total_time = ( stop.tv_sec - start.tv_sec )
	+ ( stop.tv_nsec - start.tv_nsec )
	  / 1000000000;
	printf("Total time = %fs", total_time);
	exit(0);
}
