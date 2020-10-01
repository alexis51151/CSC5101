//
// Created by alexis51151 on 01/10/2020.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <pthread.h>

#define MAX_ITEMS 100

struct node{
	struct node* next;
	int elt;
};


struct queue{
	struct node* head;
	struct node* tail;
	pthread_mutex_t lock;
};

// Global vars
struct queue q;
int K;

void enqueue(struct queue* q, int e){
	pthread_mutex_lock(&q->lock);
	struct node* n = malloc(sizeof(struct node));
	n->next = NULL;
	n->elt = e;
	if(q->tail != NULL){
		q->tail->next = n;
	}
	else{
		q->head = n;
	}
	q->tail = n;
	pthread_mutex_unlock(&q->lock);
}

int dequeue(struct queue* q){
	pthread_mutex_lock(&q->lock);
	struct node* n = q->head;
	q->head = q->head->next;
	if(q->head == NULL){
		q->tail = NULL;
	}
	pthread_mutex_unlock(&q->lock);
	return n->elt;
}

void* thread(){
	for(int i = 0; i < K;i++){
		enqueue(&q, 2);
	}
	for(int i = 0; i < K;i++){
		dequeue(&q);
	}
	pthread_exit(0);
}

int main(int argc, char** argv) {
	q.head = NULL;
	q.tail = NULL;
	pthread_mutex_init(&q.lock, NULL);

	if(argc < 3){
		printf("Please give one argument\n");
		return 1;
	}
	int N = atoi(argv[1]);
	K = atoi(argv[2]);
	printf("N = %d, K = %d\n", N, K);

	pthread_t threads[N];

	for(int i = 0; i < N; i++) {
		pthread_create(&threads[i], NULL, thread, NULL);
	}

	for(int i = 0; i < N; i++) {
		pthread_join(threads[i],NULL);
	}

	if(q.head == NULL){
		printf("Queue successful\n");
	}
	else{
		printf("Queue unsuccessful\n");
	}
	exit(0);
}
