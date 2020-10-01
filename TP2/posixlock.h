//
// Created by alexis51151 on 24/09/2020.
//

#ifndef CSC5101_POSIXLOCK_H
#define CSC5101_POSIXLOCK_H

#endif //CSC5101_POSIXLOCK_H

#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdbool.h>

#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>

enum {FREE_POSIX, BUSY_POSIX};

struct posix_lock{
	int _Atomic state;
};

struct posix_cond{
	int _Atomic state;
};

void posixlock_acquire(struct posix_lock* lock);
void posixlock_release(struct posix_lock* lock);