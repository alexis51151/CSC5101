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

struct hazard_ptr{
	struct node* _Atomic ptr;
};

struct queue{
	struct node* _Atomic head;
	struct node* _Atomic tail;
};

// Global vars
struct queue q;
struct stack free_nodes;
struct hazard_ptr** hazards;
int N, K;

// Thread-local vars
_Thread_local struct hazard_ptr h_ptr;

void push(struct stack* s, struct node* _Atomic n){
	do{
		n->next = s->head;
	} while(!atomic_compare_exchange_strong(&s->head, &n->next, n));
}

struct node* _Atomic pop(struct stack* s){
	struct node* _Atomic n;
	do {
		n = s->head;
		if(n == (struct node* _Atomic)0){
			return n;
		}
	} while(!atomic_compare_exchange_strong(&s->head, &n, n->next));
	return s->head;
}

void enqueue(struct queue* q, int e){

	// Trying to allocate a node from free_nodes stack (a simple optimization)
	struct node* _Atomic n = pop(&free_nodes);
	if(n == NULL){ // nothing on the stack
		n = malloc(sizeof(struct node));
	}
	else {
		for(int i =0; i < N;i++){
			if(hazards[i]->ptr = n){
				push(&free_nodes,n);
				n = malloc(sizeof(struct node));
			}
		}
	}
	struct node** null_node = malloc(sizeof(struct node*)); // useful in enqueue/dequeue for comparison
	*null_node = 0;
	struct node* _Atomic old;
	// We reset n values (no matter if we reuse a preallocated free node)
	n->next = (struct node* _Atomic) 0;
	n->elt = e;
	do {
		old = q->tail;
		h_ptr.ptr = q->tail;
		while(old->next != NULL){ // old is no more the tail (or empty queue)
			atomic_compare_exchange_strong(&q->tail, &old, old->next); // we try to change the tail
			old = q->tail;
			h_ptr.ptr = q->tail;
		}
	} while (!atomic_compare_exchange_strong(&old->next, null_node, n)); // as long as we haven't inserted n
	h_ptr.ptr = (struct node* _Atomic) 0; // no more used in this thread
	// then we finally try to update the tail (if old is still the tail)
	// (if we cannot, someone else has already done it for us)
	atomic_compare_exchange_strong(&q->tail, &old, n);
}

int dequeue(struct queue* q){
	struct node* res;
	do {
		res = q->head;
		h_ptr.ptr = res;

		if(res->next == NULL){ // empty queue
			return 0;
		}
		// not empty queue
	} while(!atomic_compare_exchange_strong(&q->head, &res, res->next)); // as long as we can't dequeue
	//free(res->next);
	h_ptr.ptr = (struct node* _Atomic) 0; // no more used in this thread
	push(&free_nodes, res); // needs to be freed later (in enqueue)
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

void* thread(void* args){

	// Setting up the hazard_ptr for ABA problem
	int* local_i = (int *) args;
	hazards[*local_i] = &h_ptr;

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

	N = atoi(argv[1]); // nb of threads
	K = atoi(argv[2]); // nb of iterations
	printf("N = %d, K = %d\n", N, K);

	hazards = malloc(sizeof(struct hazard_ptr)*N);


	pthread_t threads[N];
	// Creating and joining the threads
	for(int i = 0; i < N; i++) {
		int* local_i = malloc(sizeof(int));
		*local_i = i;
		pthread_create(&threads[i], NULL, thread, (void*) local_i);
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
