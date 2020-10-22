//
// Created by alexis51151 on 22/10/2020.
//

#ifndef TP6_NVMM_NVMM_H
#define TP6_NVMM_NVMM_H

#endif //TP6_NVMM_NVMM_H

#include <stdbool.h>
#include <sys/types.h>

#define NB_ENTRIES 1024*1024
#define CACHE_LINE 64

struct operation {
	union {                // first cache line
		struct {
			bool   committed;
			int    fd;
			size_t n;
			off_t  off;
		};
		char     align[CACHE_LINE]; // Union with that to be aligned on a cache line
	};
	char       buf[CACHE_LINE]; // second cache line
};

struct nvmm {
	struct operation operation[NB_ENTRIES];
	size_t           tail;
};

