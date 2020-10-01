#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define STR_MAX 100


// Structure de liste chainée
struct list{
	char* msg;
	struct list* next;
};

// Variables globales initialisées
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_rev = PTHREAD_COND_INITIALIZER;
struct list* l = 0;
struct list* last = 0;
int nb = 0;

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
		if(last != 0){ // Liste non vide
			last->next = node;
		}
		else { // Liste vide
			l = node;
		}
		last = node;

		// on lit le mot suivant
		msg = strtok(NULL, " ");
		// On réveille tous les consumers
		pthread_cond_broadcast(&c);
		pthread_cond_wait(&c_rev, &m);
	}
	pthread_mutex_unlock(&m);

}

void recv(int N){
	pthread_mutex_lock(&m);
	// On a décomposé en messages atomiques
	pthread_cond_wait(&c, &m);


	// Est-on le dernier consumer à lire ??
	nb++;

	if(nb == N){
		// On lit les messages
		while(l != 0){
			// On lit le message le plus ancien
			printf("Message : %s\n", l->msg);

			// Est-ce qu'on va supprimer last ?
			struct list* node = l;
			l = l->next;

			if(l == 0){ // On n'a plus d'éléments dans la liste
				last = 0;
			}

			// Libération de la mémoire du noeud
			free(node);
			// On permet au producteur d'écrire à nouveau des messages
			nb = 0;
			pthread_cond_signal(&c_rev);
		}

	}
	else{ // sinon on ne supprime rien
		struct list* l_aux = l;
		while(l_aux !=0){
			// On lit le message le plus ancien
			printf("Message : %s\n", l_aux->msg);
			l_aux = l_aux->next;
		}
	}
	pthread_mutex_unlock(&m);
}

void* consumer(void* arg){
	int N = *(int*) arg;
	while(1){
		recv(N);
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

	/*
	scanf("%[^\n]s", msg);
	send(msg);
	 */
	pthread_exit(0);
}
