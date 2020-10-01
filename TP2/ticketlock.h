//
// Created by alexis51151 on 24/09/2020.
//

#ifndef CSC5101_TICKETLOCK_H
#define CSC5101_TICKETLOCK_H

#endif //CSC5101_TICKETLOCK_H

#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdbool.h>


struct ticket_lock {
	int _Atomic ticket;
	int _Atomic screen;
};

void ticketlock_acquire(struct ticket_lock* t);
void ticketlock_release(struct ticket_lock* t);
