#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DATA "out.dat"


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
	return pwrite(fd, buf, count, offset);
}

int main(int argc, char** argv){
	int fd = open(DATA, O_CREAT | O_TRUNC | O_RDWR);
	if(fd < 0){
		printf("open : Could not access file %s", DATA);
		return EXIT_FAILURE;
	}
	char msg[10] = "hello";
	/* Write hello in the DATA file 1024 times */
	for(int off = 0; off < 1024;off+=5){
		if(my_pwrite(fd, msg, 10, off) < 0 ){
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}