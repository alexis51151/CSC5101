//
// Created by alexis51151 on 24/09/2020.
//

#include "posixlock.h"

static long futex(void *addr1, int op, int val1, struct timespec *timeout,
                  void *addr2, int val3) {
	return syscall(SYS_futex, addr1, op, val1, timeout, addr2, val3);
}

void posixlock_acquire(struct posix_lock* lock){
	while(1){
		int expected = FREE_POSIX;
		if(atomic_compare_exchange_strong(&lock->state, &expected, BUSY_POSIX)){
			return;
		}
		if(futex(&lock->state, FUTEX_WAIT, BUSY_POSIX, NULL, NULL, 0) == -1 && errno != EAGAIN){
			return;
		}
	}
}

void posixlock_release(struct posix_lock* lock){
	int expected = BUSY_POSIX;
	atomic_store(&lock->state, FREE_POSIX);
	futex(&lock->state, FUTEX_WAKE, 1, NULL, NULL, 0);
}
