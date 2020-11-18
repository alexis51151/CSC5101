//
// Created by Alexis Le Glaunec on 18/11/2020.
//
#include <stdio.h>
#include <pthread.h>

struct rwlock {
	pthread_mutex_t wlock;      /* verrou bloquant les écrivains */
	pthread_mutex_t rlock;      /* verrou pour synchroniser les lecteurs */
	int             nb_readers; /* donne le nombre de lecteurs dans la section critique */
};

// global variables
struct rwlock lock;


// lock init
void init(struct rwlock* lock){
	pthread_mutex_init(&lock->wlock, NULL);
	pthread_mutex_init(&lock->rlock, NULL);
	lock->nb_readers = 0;
}

// called by reader before CS
void pread(struct rwlock* lock){
	pthread_mutex_lock(&lock->rlock);
	// no reader in the CS
	if(lock->nb_readers == 0){
		pthread_mutex_lock(&lock->wlock);
	}
	lock->nb_readers++;
	pthread_mutex_unlock(&lock->rlock);
}

// called by reader after CS
void vread(struct rwlock* lock){
	pthread_mutex_lock(&lock->rlock);
	lock->nb_readers--;
	if(lock->nb_readers == 0){
		pthread_mutex_unlock(&lock->wlock);
	}
	pthread_mutex_unlock(&lock->rlock);
}

// called by writer before CS
void pwrite(struct  rwlock* lock) {
	pthread_mutex_lock(&lock->rlock);
	pthread_mutex_lock(&lock->wlock);
	pthread_mutex_unlock(&lock->rlock);
}

// called by writer after CS
void vwrite(struct  rwlock* lock) {
	pthread_mutex_unlock(&lock->wlock);
}



int main(int argc, char** argv) {
	// lock init
	init(&lock);


	return 0;
}
