#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int counter = 0;
pthread_mutex_t lock;


void* hello_world(void* arg){
	int* val = (int*) arg;
	printf("[%ld]Hello, World %d!!!\n", pthread_self(), *val);
	free(val);
	pthread_mutex_lock(&lock);
	counter++;
	pthread_mutex_unlock(&lock);
	pthread_exit(NULL);
}


int main(int argc, char** argv) {
	if(argc < 2){
		printf("Please give one argument\n");
		return 1;
	}
	int N = atoi(argv[1]);
	for(int i = 0; i < N; i++){
		int* val = malloc(sizeof(int));
		*val = i;
		pthread_t t;
		pthread_create(&t, NULL, hello_world, (void *) val);
		//pthread_join(t, NULL);
	}
	while(counter < N){
	}
	printf("Counter value = %d\n", counter);
	pthread_exit(0);
	return 0;
}
