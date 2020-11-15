//
// Created by Alexis Le Glaunec on 15/11/2020.
//
#define MAX 100

#include <stdio.h>

// LIFO queue data type
struct queue {
	int tab[MAX];
	int head;
};

// global variables
struct queue q;


int push(char e){
	while(q.head == MAX){ // active waiting while the queue is full
	}
	// update the queue
	q.tab[q.head] = e;
	q.head++;
	return 1;
}

char pop() {
	while(q.head == 0){ // active waiting while the queue is empty

	}
	q.head--;
	return q.tab[q.head];
}


int main(int argc, char** argv){
	push(*"a");
	push(*"e");
	printf("pop : %c\n" ,pop());
	push(*"s");
	printf("pop : %c\n" ,pop());
	push(*"t");
	printf("pop : %c\n" ,pop());
	printf("pop : %c\n" ,pop());

}