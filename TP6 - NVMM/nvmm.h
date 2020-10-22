//
// Created by alexis51151 on 22/10/2020.
//

#ifndef _NVMM_H_
#define _NVMM_H_

// note: compile with the option -march=native

#if defined(__CLWB__)
//#pragma message "Using clwb as pwb implementation"
static void pwb(void *p) { asm volatile("clwb (%0)\n\t" : : "r"(p) : "memory"); }
static void pfence()     { asm volatile("sfence" ::: "memory"); }
static void psync()      { asm volatile("sfence" ::: "memory"); }
#elif defined(__CLFLUSHOPT__)
//#pragma message "Using clflushopt as pwb implementation"
static void pwb(void *p) { asm volatile("clflushopt (%0)\n\t" : : "r"(p) : "memory"); }
static void pfence()     { asm volatile("sfence" ::: "memory"); }
static void psync()      { asm volatile("sfence" ::: "memory"); }
#else
//#pragma message "Using default clflush as pwb implementation, try to compile with -march=native"
static void pwb(void *p) { asm volatile("clflush (%0)\n\t" : : "r"(p) : "memory"); }
static void pfence()     { asm volatile("" ::: "memory"); }
static void psync()      { asm volatile("" ::: "memory"); }
#endif

#endif

#include <stdbool.h>
#include <sys/types.h>

#define NB_ENTRIES (1024*1024)
#define CACHE_LINE 64

// note: compile with the option -march=native



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

