#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define STR_MAX 100


// Structure de liste chainée
struct list{
	char* msg;
	struct list* next;
	int n;
};

// Variables globales initialisées
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_rev = PTHREAD_COND_INITIALIZER;
struct list* l = 0;
struct list* last = 0;

// Ecrit à la position last de la liste
void send(char* str){
	pthread_mutex_lock(&m);

	// Découpage du str en messages
	char* msg = strtok(str, " ");

	// On écrit les lignes de stdin ici (séparées par des espaces ici)
	while(msg != NULL){
		// On rajoute le noeud en queue de liste chainée (en utilisant last)
		struct list* node = malloc(sizeof(struct list));
		node->msg = msg;
		node->next = 0;
		node->n = 0;
		if(last != 0){ // Liste non vide
			last->next = node;
		}
		else { // Liste vide
			l = node;
		}
		last = node;

		// on lit le mot suivant
		msg = strtok(NULL, " ");
	}
	// On réveille tous les consumers
	pthread_cond_broadcast(&c);
	pthread_mutex_unlock(&m);

}

void recv(int N, struct list* aux_l){
	pthread_mutex_lock(&m);
	// On a décomposé en messages atomiques
	pthread_cond_wait(&c, &m);
	printf("New message : %s", l->msg);
	while(aux_l != 0) {
		aux_l->n++;
		printf("Message : %s\n", aux_l->msg);

		if(aux_l->n == N){
			struct list* node = aux_l;
			if(aux_l == l){ // On n'a plus d'éléments dans la liste
				last = 0;
			}
			free(node);
		}
		aux_l = aux_l->next;

	}
	pthread_mutex_unlock(&m);
}

void* consumer(void* arg){
	int N = *(int*) arg;
	struct list* aux_l = l;
	while(1){
		recv(N, aux_l);
	}
}

int main(int argc, char** argv) {

	// On créer les consommateurs
	if(argc < 2){
		printf("Please give one argument\n");
		return 1;
	}
	int N = atoi(argv[1]);
	for(int i = 0; i < N; i++) {
		pthread_t c;
		pthread_create(&c, NULL, consumer, &N);
	}

	size_t n = 0;
	char* msg = NULL;
	// On lit sur stdin
	while(1){
		if(getline(&msg, &n, stdin) > 0){
			send(msg);
		}
	}
	pthread_exit(0);
}
