//
// Created by Alexis Le Glaunec on 18/11/2020.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdatomic.h>

union rwlock {
	struct {
		uint64_t nb_readers : 63;
		uint64_t has_writer : 1;
	};
	uint64_t state;
};

// Stack data structure
struct node {
	int val;
	struct node* next;
};


// global variables
union rwlock lock;
struct node* stack;

void init(union rwlock* lock, struct node* stack){
	// lock init
	lock->state = 0;
	// stack init
	stack = malloc(sizeof(struct node));
	stack->val = 0;
	stack->next = 0;

}

// called by reader before CS
void pread(union rwlock* lock){
	int added = 0;
	while(atomic_load(lock->nb_readers) == 0){
		// take a "lock" on writer
		if(atomic_compare_exchange_strong(lock->has_writer, 0 ,1)) {
			atomic_fetch_add(lock->nb_readers,1);
			added = 1;
		}
	}
	if(added == 0){
		atomic_fetch_add(lock->nb_readers,1);
	}
}

// called by reader after CS
void vread(union rwlock* lock){
	if(atomic_fetch_add(lock->nb_readers,-1) == 1) {
		atomic_store(lock->has_writer,0);
	}
}

// called by writer before CS
void pwrite(union  rwlock* lock) {
	while (!atomic_compare_exchange_strong(lock->has_writer, 0, 1)) {
		// wait
	}
}
// called by writer after CS
void vwrite(union  rwlock* lock) {
	atomic_store(lock->has_writer,0);
}


void push(int n){
	struct node* node = malloc(sizeof(struct node));
	node->val = n;
	pwrite(&lock);
	node->next = stack;
	stack = node;
	vwrite(&lock);
}

int pop() {
	pwrite(&lock);
	int value = stack->val;
	struct node* node = stack;
	stack = stack->next;
	free(node);
	vwrite(&lock);
	return value;
}

int size(){
	int count = 0;
	pread(&lock);
	struct node* cur = stack;
	while(cur != 0){
		count++;
		cur = cur->next;
	}
	vread(&lock);
	return count;
}

int main(int argc, char** argv) {
	// lock & stack init
	init(&lock, stack);


	return 0;
}
