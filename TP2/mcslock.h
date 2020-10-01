//
// Created by alexis51151 on 24/09/2020.
//

#ifndef CSC5101_MCSLOCK_H
#define CSC5101_MCSLOCK_H

#endif //CSC5101_MCSLOCK_H

#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdbool.h>

struct node {
	struct node* _Atomic next;
	bool _Atomic isFree;
};


void mcslock_acquire(struct node* _Atomic lock);

void mcslock_release(struct node* _Atomic lock);


