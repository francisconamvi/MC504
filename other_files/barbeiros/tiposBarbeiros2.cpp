/*
 * Problema do barbeiro dorminhoco com vários barbeiros.
 */ 
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_CLIENTES 10
#define N_BARBEIROS 4
#define N_CADEIRAS 5
#define N_TIPOS 3
#define TEMPOS 3

struct Cliente{
    int id;
    int imagem;
    int tipo;
};

struct Barbeiro{
    int id;
    int tipo;
};

sem_t sem_cadeiras;
sem_t sem_cad_barbeiro[N_BARBEIROS];
sem_t sem_cabelo_cortado[N_BARBEIROS];
sem_t sem_cliente_cadeira[N_BARBEIROS];

sem_t sem_escreve_painel[N_TIPOS], sem_le_painel[N_TIPOS];
int painel[N_TIPOS];
int cavalete[N_BARBEIROS];

void* f_barbeiro(void *v) {
  struct Barbeiro barbeiro = *(struct Barbeiro*) v;

  while(1) {
    sem_wait(&sem_escreve_painel[barbeiro.tipo]);
    painel[barbeiro.tipo] = barbeiro.id;
    sem_post(&sem_le_painel[barbeiro.tipo]);
    sem_wait(&sem_cliente_cadeira[barbeiro.id]);
    //funcao processar imagem
    printf("Barbeiro %d do tipo %d cortou o cabelo de um cliente imagem %d.\n", barbeiro.id, barbeiro.tipo, cavalete[barbeiro.id]);
    sem_post(&sem_cabelo_cortado[barbeiro.id]);
    sleep(random()%TEMPOS);
  }
  return NULL;
}

void* f_cliente(void* v) {
  struct Cliente cliente = *(struct Cliente*) v;
  int minha_cadeira;

  sleep(random()%TEMPOS);
  if (sem_trywait(&sem_cadeiras) == 0) {
    printf("Cliente %d entrou na barbearia.\n", cliente.id);

    sem_wait(&sem_le_painel[cliente.tipo]);
    minha_cadeira = painel[cliente.tipo];
    printf("Cliente %d encontrou um barbeiro do tipo %d que é o barbeiro %d livre que procuravas\n", cliente.id, cliente.tipo, minha_cadeira);
    sem_post(&sem_escreve_painel[cliente.tipo]);
    sem_wait(&sem_cad_barbeiro[minha_cadeira]);
    //entregar imagem
    cavalete[minha_cadeira] = cliente.imagem;
    printf("Cliente %d sentou na cadeira do barbeiro %d.\n", cliente.id, minha_cadeira);
    sem_post(&sem_cliente_cadeira[minha_cadeira]);
    sem_post(&sem_cadeiras);
    sem_wait(&sem_cabelo_cortado[minha_cadeira]);
    sem_post(&sem_cad_barbeiro[minha_cadeira]);
    printf("Cliente %d deixou a barbearia.\n", cliente.id);
  } else
    printf("Cliente %d não entrou na barbearia.\n", cliente.id);
  return NULL;
}

int main() {
  pthread_t thr_clientes[N_CLIENTES], thr_barbeiros[N_BARBEIROS];
  int i; 
  struct Barbeiro id_bar[N_BARBEIROS];
  struct Cliente id_cl[N_CLIENTES];

  sem_init(&sem_cadeiras, 0, N_CADEIRAS);

  for (i = 0; i < N_BARBEIROS; i++) {
    sem_init(&sem_cad_barbeiro[i], 0, 1);
    sem_init(&sem_cliente_cadeira[i], 0, 0);
    sem_init(&sem_cabelo_cortado[i], 0, 0);
    sem_init(&sem_escreve_painel[i], 0, 1);
    sem_init(&sem_le_painel[i], 0, 0);
  }

  for (i = 0; i < N_TIPOS; i++) {
    id_bar[i].id = i;
    id_bar[i].tipo = i;
    printf("barbeiro %d: tipo %d\n", i, id_bar[i].tipo);
    pthread_create(&thr_barbeiros[i], NULL, f_barbeiro, (void*) &id_bar[i]);
  }
  for (i = N_TIPOS; i < N_BARBEIROS; i++) {
    id_bar[i].id = i;
    id_bar[i].tipo = random()%(N_TIPOS);
    printf("barbeiro %d: tipo %d\n", i, id_bar[i].tipo);
    pthread_create(&thr_barbeiros[i], NULL, f_barbeiro, (void*) &id_bar[i]);
  }
  
  for (i = 0; i < N_CLIENTES; i++) {
    id_cl[i].id = i;
    id_cl[i].tipo = random()%(N_TIPOS);
    //Colocar a imagem.
    // id_cl[i].imagem = NULL;
    id_cl[i].imagem = random()%(100);
    printf("cliente %d: tipo %d - imagem %d\n", i, id_cl[i].tipo, id_cl[i].imagem);
    pthread_create(&thr_clientes[i], NULL, f_cliente, (void*) &id_cl[i]);
  }
  
  for (i = 0; i < N_CLIENTES; i++) 
    pthread_join(thr_clientes[i], NULL);

  /* Barbeiros são assassinados */
  
  return 0;
}