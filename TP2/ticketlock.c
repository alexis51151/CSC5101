//
// Created by alexis51151 on 24/09/2020.
//

#include "ticketlock.h"


void ticketlock_acquire(struct ticket_lock* t){
	int my = atomic_fetch_add(&t->ticket, 1);
	while(atomic_load(&t->screen) < my) { }
}

void ticketlock_release(struct ticket_lock* t){
	atomic_fetch_add(&t->screen, 1);
}