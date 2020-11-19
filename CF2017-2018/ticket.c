//
// Created by Alexis Le Glaunec on 18/11/2020.
//

#include <stdlib.h>
#include <stdatomic.h>

struct ticket_lock {
	int _Atomic ticket;
	int _Atomic guichet;
};

struct ticket_lock* t;

void init() {
	t->guichet = 0;
	t->ticket = 0;
}


void lock() {
	int ticket = atomic_fetch_add(&t->ticket, 1);
	while(atomic_load(&t->guichet) < ticket);
}

void unlock() {
	atomic_fetch_add(&t->guichet,1);
}



int main() {
	return 0;
}