//
// Created by alexis51151 on 08/10/2020.
//

#include <sys/types.h>
#include <stdint.h>

#ifndef TP4_TRANSACTIONAL_MEMORY_TM_H
#define TP4_TRANSACTIONAL_MEMORY_TM_H

#endif //TP4_TRANSACTIONAL_MEMORY_TM_H

#define NB_CELLS 65536

struct cell {
	uintptr_t value;
	size_t    counter;
};

struct memory {
	struct cell* cells[NB_CELLS];
	size_t       clock;
};
