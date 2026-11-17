#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define TAM_BUFFER 5
#define ITEMS_POR_HILO 6
#define NUM_PRODUCTORES 2
#define NUM_CONSUMIDORES 2

int buffer[TAM_BUFFER];
int contador = 0;
int entrada = 0;
int salida = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t no_lleno = PTHREAD_COND_INITIALIZER;
pthread_cond_t no_vacio = PTHREAD_COND_INITIALIZER;

void* productor(void* arg) {
    int id = *(int*)arg;
    for (int i = 1; i <= ITEMS_POR_HILO; i++) {
        int item = (id * 100) + i;
        pthread_mutex_lock(&mutex);
        while (contador == TAM_BUFFER) {
            printf("[Productor %d] Bufer lleno. Esperando...\n", id);
            pthread_cond_wait(&no_lleno, &mutex);
        }
        buffer[entrada] = item;
        entrada = (entrada + 1) % TAM_BUFFER;
        contador++;
        printf("[Productor %d] Produjo: %d | Elementos en bufer: %d\n", id, item, contador);
        pthread_cond_signal(&no_vacio);
        pthread_mutex_unlock(&mutex);
        usleep(100000);
    }
    pthread_exit(NULL);
}

void* consumidor(void* arg) {
    int id = *(int*)arg;
    for (int i = 1; i <= ITEMS_POR_HILO; i++) {
        pthread_mutex_lock(&mutex);
        while (contador == 0) {
            printf("[Consumidor %d] Bufer vacio. Esperando...\n", id);
            pthread_cond_wait(&no_vacio, &mutex);
        }
        int item = buffer[salida];
        salida = (salida + 1) % TAM_BUFFER;
        contador--;
        printf("[Consumidor %d] Consumio: %d | Elementos en bufer: %d\n", id, item, contador);
        pthread_cond_signal(&no_lleno);
        pthread_mutex_unlock(&mutex);
        usleep(150000);
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t prod[NUM_PRODUCTORES], cons[NUM_CONSUMIDORES];
    int ids_prod[NUM_PRODUCTORES] = {1, 2};
    int ids_cons[NUM_CONSUMIDORES] = {1, 2};

    printf("=== Iniciando simulacion Productor-Consumidor (2 Productores, 2 Consumidores) ===\n\n");

    for (int i = 0; i < NUM_PRODUCTORES; i++) {
        pthread_create(&prod[i], NULL, productor, &ids_prod[i]);
    }
    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        pthread_create(&cons[i], NULL, consumidor, &ids_cons[i]);
    }

    for (int i = 0; i < NUM_PRODUCTORES; i++) {
        pthread_join(prod[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        pthread_join(cons[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&no_lleno);
    pthread_cond_destroy(&no_vacio);

    printf("\n=== Ejecucion finalizada con exito ===\n");
    return 0;
}
