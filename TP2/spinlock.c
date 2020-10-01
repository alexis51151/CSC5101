//
// Created by alexis51151 on 24/09/2020.
//

#include "spinlock.h"

void spinlock_acquire(int _Atomic* lock){
	while(atomic_exchange(lock, BUSY) != FREE) {}
}

void spinlock_release(int _Atomic* lock){
	atomic_store(lock, FREE);
}