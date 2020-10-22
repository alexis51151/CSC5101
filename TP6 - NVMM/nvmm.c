#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "nvmm.h" // All the NVMM structs
#include "string.h"
#include <sys/mman.h>
#include <stdatomic.h>
#include <pthread.h>

#define DATA "nvmm.dat"
#define OUT  "out.dat"
#define DATASIZE (128*1024*1024+4)


struct nvmm*   nvmm; // pointer to NVMM
size_t _Atomic head; // where we add the entries in the log
int nvmm_fd;

void* thread_clean(){
	printf("Entering cleanup\n");
	int c = 0;
	while(c < 1024){
		/* Wait for an operation to read*/
		while(nvmm->operation[nvmm->tail].committed != true){
		}
		printf("Valeur n° %d\n", c/5);
		/* Propagate to the file */
		pwrite(nvmm->operation[nvmm->tail].fd, (char*) nvmm->operation[nvmm->tail].buf, nvmm->operation[nvmm->tail].n, nvmm->operation[nvmm->tail].off);
		fsync(nvmm->tail);
		nvmm->operation[nvmm->tail].committed = false;
		pfence();
		nvmm->tail = (nvmm->tail+1)%NB_ENTRIES;
		psync();
		c += 5;
	}
	return (void*) 0;
}


int my_pwrite(int fd, void* buf, size_t count, off_t offset){

	/* Wait while the log is full */
	while(head+1 == nvmm->tail){
		// Wait
		printf("Sleeping....\n");
	}

	/* Add the operation in the NVMM */
	if(nvmm->operation[head].committed == false){
		memcpy(nvmm->operation[head].buf, buf, count);
		pwb(&nvmm->operation[head].buf);
		nvmm->operation[head].n = count;
		pwb(&nvmm->operation[head].n);
		nvmm->operation[head].fd = fd;
		pwb(&nvmm->operation[head].fd);
		nvmm->operation[head].off = offset;
		pwb(&nvmm->operation[head].off);
		pfence();
		nvmm->operation[head].committed = true;
		pwb(&nvmm->operation[head].committed);
		psync();
	}
	printf("Valeur après pwrite %s \n", nvmm->operation[head].buf);
	head = (head+1)%NB_ENTRIES;
	printf("Head value : %ld\n", head);
	return EXIT_SUCCESS;
}


int main(int argc, char** argv){

	/* Initializing head */
	head = 0;

	/* Creating the NVMM file */
	nvmm_fd = open(DATA, O_CREAT | O_RDWR | O_TRUNC, 644);
	if(nvmm_fd < 0){
		printf("open : Could not access file %s", DATA);
		return EXIT_FAILURE;
	}
	if( ftruncate(nvmm_fd, sizeof(struct nvmm)) < 0){
		printf("truncate : Could not truncate file %s", DATA);
		return EXIT_FAILURE;
	}
	nvmm = mmap(NULL, sizeof(struct nvmm), PROT_READ | PROT_WRITE, MAP_SHARED, nvmm_fd, 0);
	if(nvmm == (void*) -1){
		printf("mmap failed");
		return EXIT_FAILURE;
	}
	nvmm->tail = head;
	pwb(&nvmm->tail);
	psync();
	pthread_t cleanup;
	pthread_create(&cleanup, NULL, thread_clean, NULL);


	/* Open the file to write to */
	int out_fd = open(OUT, O_CREAT | O_RDWR | O_TRUNC, 600);
	if(out_fd < 0){
		printf("open : Could not access file %s", DATA);
		return EXIT_FAILURE;
	}

	char msg[5] = "hello";

	for(int off = 0; off < 1024;off+=5){
		if(my_pwrite(out_fd, msg,  sizeof(msg), off) < 0){
			return EXIT_FAILURE;
		}
	}

	pthread_join(cleanup, NULL);
	munmap(nvmm, DATASIZE);
	close(nvmm_fd);
	close(out_fd);
	return EXIT_SUCCESS;
}