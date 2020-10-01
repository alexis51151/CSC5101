//
// Created by alexis51151 on 24/09/2020.
//

#ifndef CSC5101_SPINLOCK_H
#define CSC5101_SPINLOCK_H

#endif //CSC5101_SPINLOCK_H

#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>


enum {FREE ,BUSY};

void spinlock_acquire(int _Atomic* lock);

void spinlock_release(int _Atomic* lock);