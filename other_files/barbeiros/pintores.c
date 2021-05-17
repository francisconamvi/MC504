#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define n_clientes 10
#define n_pintores 3
#define n_cadeiras 5

struct cliente{
    int id;
    int x;
    int y;
};

sem_t sem_cadeiras;
sem_t sem_cad_pintores[n_pintores];
sem_t sem_pintura_finalizada[n_pintores];
sem_t sem_cliente_cadeira[n_pintores];

sem_t sem_escreve_sinal, sem_le_sinal;
int sinal;

void *f_pintor(void *v){
    int id = *(int*) v;

    while(1){
        sem_wait(&sem_escreve_sinal);
        sinal = id;
        sem_post(&sem_le_sinal);
        sem_wait(&sem_cliente_cadeira[id]);
        //?????????? função Hitalo ?????????????????
        printf("Pintor %d acabou a pintura.\n", id);
        sem_post(&sem_pintura_finalizada[id]);
        sleep(random()%3);
    }
    return NULL;
}

void *f_cliente(void *v){
    struct cliente *client = (struct cliente * ) v;
    int cadeira;

    sleep(random()%3);
    if(sem_trywait(&sem_cadeiras) == 0){
        printf("Cliente %d entrou na galeria.\n", client->id);
        sem_wait(&sem_le_sinal);
        cadeira = sinal;
        sem_post(&sem_escreve_sinal);
        sem_wait(&sem_cad_pintores[cadeira]);
        printf("Cliente %d pediu uma arte de tamanho %d x %d\n", client->id, client->x, client->y);
        sem_post(&sem_cliente_cadeira[cadeira]);
        sem_post(&sem_cadeiras);
        sem_wait(&sem_pintura_finalizada[cadeira]);
        sem_post(&sem_cad_pintores[cadeira]);
        printf("Cliente %d deixou o atelie.\n", client->id);


    } else
        printf("Galeria estava fechada para o cliente %d\n", client->id);
    return NULL;
}



int main(){
    pthread_t thr_clientes[n_clientes], thr_pintores[n_pintores];
    int i, id_pintores[n_pintores];
    struct cliente clientes[n_clientes];
    
    for(i=0; i < n_clientes; i++){
        scanf("%d %d", &clientes[i].x, &clientes[i].y);
    }
    
    sem_init(&sem_cadeiras, 0, n_cadeiras);
    sem_init(&sem_escreve_sinal, 0, 1);
    sem_init(&sem_le_sinal, 0, 0);
    
    for (i = 0; i < n_pintores; i++) {
        sem_init(&sem_cad_pintores[i], 0, 1);
        sem_init(&sem_cliente_cadeira[i], 0, 0);
        sem_init(&sem_pintura_finalizada[i], 0, 0);
    }
    
    for (i = 0; i < n_clientes; i++) {
        clientes[i].id = i;
        pthread_create(&thr_clientes[i], NULL, f_cliente, (void*) &clientes[i]);
    }
    
    for (i = 0; i < n_pintores; i++) {
        id_pintores[i] = i;
        pthread_create(&thr_pintores[i], NULL, f_pintor, (void*) &id_pintores[i]);
    }

    for (i = 0; i < n_clientes; i++) 
        pthread_join(thr_clientes[i], NULL);
    
    
    return 0;
}
