//
// Created by alexis51151 on 24/09/2020.
//

#include "mcslock.h"

_Thread_local struct node my;

void mcslock_acquire(struct node* _Atomic lock){
	my.next = NULL;
	my.isFree = false;
	struct node* p = atomic_exchange(&lock, &my);
	if(p){
		atomic_store(&p->next, &my);
		while(!atomic_load(&my.isFree)) { }
	}
}

void mcslock_release(struct node* _Atomic lock){
	struct node* expected = &my;
	if(!atomic_load(&my.next) && atomic_compare_exchange_strong(&lock, &expected, NULL)){
		return;
	}
	while(!atomic_load(&my.next)) { }
	atomic_store(&my.next->isFree, true);
}

