#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "nvmm.h" // All the NVMM structs
#include "string.h"
#include <sys/mman.h>

#define DATA "test.dat"
#define DATASIZE 128*1024*1024

struct nvmm* nvmm; // pointer to NVMM
size_t       head; // where we add the entries in the log

void pwb(void *p) {
	asm volatile("clwb (%0)\n\t" : : "r"(p) : "memory");
}

void pfence() {
	asm volatile("sfence" ::: "memory");
}

void psync() {
	asm volatile("sfence" ::: "memory");
}

int my_pwrite(int fd, void* buf, size_t count, off_t offset){
	/* Wait while the log is full */
	/*
	while(head+1 == nvmm->tail){
	}
	struct operation* op = malloc(sizeof(struct operation));
	// Fill the operation structure
	memcpy(op->buf, buf, count);
	op->committed = true;
	op->fd = fd;
	op->n = count;
	op->off = offset;
	*/
	return pwrite(fd, buf, count, offset);
}

int main(int argc, char** argv){
	int fd = open(DATA, O_CREAT | O_TRUNC | O_RDWR);
	if(fd < 0){
		printf("open : Could not access file %s", DATA);
		return EXIT_FAILURE;
	}
	if( ftruncate(fd, DATASIZE) < 0){
		printf("truncate : Could not truncate file %s", DATA);
		return EXIT_FAILURE;
	}
	int* addr = mmap(NULL, DATASIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 128*1024);
	if(addr == (void*) -1){
		printf("mmap failed");
		return EXIT_FAILURE;
	}
	addr[0]++;

	/*
	char msg[10] = "hello";
	/* Write hello in the DATA file 1024 times */
	/*
	for(int off = 0; off < 1024;off+=5){
		if(my_pwrite(fd, msg, 10, off) < 0 ){
			return EXIT_FAILURE;
		}
	}
	*/
	return EXIT_SUCCESS;
}