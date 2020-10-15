//
// Created by alexis51151 on 08/10/2020.
//

#include <sys/types.h>
#include <stdint.h>

#ifndef TP4_TRANSACTIONAL_MEMORY_TM_H
#define TP4_TRANSACTIONAL_MEMORY_TM_H

#endif //TP4_TRANSACTIONAL_MEMORY_TM_H

#define NB_CELLS 65536
#define MAX_INT 1000000

static inline uint64_t read_tscp(){
	uint32_t eax, edx;
	asm volatile("rdtscp" : "=a" (eax), "=d" (edx));
	return eax | ((uint64_t)edx)<<32;
}

struct cell {
	uintptr_t value;
	size_t    counter;
};

struct memory {
	struct cell* _Atomic cells[NB_CELLS];
	size_t       clock;
};
