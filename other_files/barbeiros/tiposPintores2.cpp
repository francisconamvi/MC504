/*
 * Problema do pintor dorminhoco com vários pintors.
 */ 
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_CLIENTES 10
#define N_PINTORES 4
#define N_CADEIRAS 5
#define N_TIPOS 3
#define TEMPOS 3

/*
 * Structs dos Clientes e Pintores
*/
struct Cliente{
    int id;
    int imagem;
    int tipo;
};

struct Pintor{
    int id;
    int tipo;
};

/*
 * Variaveis para guardar estado dos clientes/pintores.
 * A - arrived           (cliente);
 * P (C ou P) - painting  (cliente/pintor);
 * E - exit  		 (cliente);
 * I - idle              (cliente);
 * L - left    		 (cliente);
 * S - sleeping 	 (pintor);
 * W - waiting		 (cliente).
 */
typedef enum{A, PC, E, I, L, W} estado_c;
typedef enum{PP, S} estado_p;

estado_c estadoCliente[N_CLIENTES];
estado_p estadoPintor[N_PINTORES];

/*
 * Variaveis para guardar estado das cadeiras.
 * B - busy
 * F - free
 */
typedef enum {B, F} estado_cadeira;
estado_cadeira estadoCadeiraCliente[N_CADEIRAS];

/*
 * Semáforos
*/
sem_t sem_cadeiras;
sem_t sem_estados;
sem_t sem_cad_pintor[N_PINTORES];
sem_t sem_cabelo_cortado[N_PINTORES];
sem_t sem_cliente_cadeira[N_PINTORES];

sem_t sem_escreve_painel[N_TIPOS], sem_le_painel[N_TIPOS];
int painel[N_TIPOS]; /*painel dividido entre pintores de cada tipo*/
int cavalete[N_PINTORES]; /*cavaletes onde pintores realizam os trabalhos*/

int ite=0;

void printAtelie(){
  printf("Iteração %d\n", ite);
  ite++;
  printf("Cliente chegou: ");
  for(int i=0; i<N_CLIENTES; i++){
    if(estadoCliente[i] == A){
      printf("%d ", i);
    }
  }
  printf("\n");

  printf("Clientes na espera: ");
  for(int i=0; i<N_CLIENTES; i++){
    if(estadoCliente[i] == W){
      printf("%d ", i);
    }
  }
  printf("\n");

  printf("Pintor pintando qual cliente: ");
  for(int i=0; i<N_CLIENTES; i++){
    if(estadoCliente[i] == PC){
      printf("%d ", i);
    }
  }
  printf("\n");

  printf("----------------------------------\n");
  return;
}

void* f_pintor(void *v) {
  struct Pintor pintor = *(struct Pintor*) v;

  while(1) {
    sem_wait(&sem_escreve_painel[pintor.tipo]);
    painel[pintor.tipo] = pintor.id;
    sem_post(&sem_le_painel[pintor.tipo]);
    //! printf("Pintor %d (tipo %d) esperando.\n", pintor.id, pintor.tipo);
    sem_wait(&sem_cliente_cadeira[pintor.id]);

    //funcao processar imagem
    sleep(1);

    // ! printf("Pintor %d (tipo %d) restaurou a imagem %d.\n", pintor.id, pintor.tipo, cavalete[pintor.id]);
    sem_post(&sem_cabelo_cortado[pintor.id]);
    sleep(random()%TEMPOS); //? PRECISA DISSO? 
  }
  return NULL;
}

void* f_cliente(void* v) {
  sleep(1);
  struct Cliente cliente = *(struct Cliente*) v;
  int minha_cadeira;

  sleep(random()%TEMPOS);

  /* Cliente chegou no ateliê */
  sem_wait(&sem_estados);
  estadoCliente[cliente.id] = A;
  printAtelie();
  sem_post(&sem_estados);

  if (sem_trywait(&sem_cadeiras) == 0) {

    /* Cliente está esperando pra ser atendido e pegando uma cadeira */
    sem_wait(&sem_estados);
    estadoCliente[cliente.id] = W;
    //TODO - PEGAR CADEIRA
    printAtelie();
    sem_post(&sem_estados);

    /* Cliente está esperando pra ler painel do tipo de restauração que precisa */
    sem_wait(&sem_le_painel[cliente.tipo]);
    minha_cadeira = painel[cliente.tipo];
    /* Cliente encontrou um pintor livre do tipo que procurava */

    /* Cliente libera escrita no painel para outro pintor */
    sem_post(&sem_escreve_painel[cliente.tipo]);

    /* Cliente espera pintor liberar pra colocar pintura no cavalete */
    sem_wait(&sem_cad_pintor[minha_cadeira]);
    cavalete[minha_cadeira] = cliente.imagem;
    /* Cliente colocou pintura no cavalete do pintor */
    sem_post(&sem_cliente_cadeira[minha_cadeira]);

    sem_wait(&sem_estados);
    estadoCadeiraCliente[minha_cadeira] = B;
    sem_post(&sem_cadeiras);

    /* Fazendo pintura*/
    estadoCliente[cliente.id] = PC;
    estadoPintor[cliente.tipo] = PP;
    printAtelie();
    sem_post(&sem_estados);
    
    /* Cliente está esperando trabalho finalizar */
    sem_wait(&sem_cabelo_cortado[minha_cadeira]);
    sem_post(&sem_cad_pintor[minha_cadeira]);

    sem_wait(&sem_estados);
    /* Cliente saiu do atelie e pintor está disponível */
    estadoCliente[cliente.id] = E;
    estadoPintor[cliente.tipo] = S;
    printAtelie();
    estadoCliente[cliente.id] = L;
    sem_post(&sem_estados);
  }
  else {
    /* Não tinha cadeira para o cliente */
    sem_wait(&sem_estados);
    estadoCliente[cliente.id] = L;
    printAtelie();
    sem_post(&sem_estados);
  }
  return NULL;
}

int main() {
  srand(time(0));

  pthread_t thr_clientes[N_CLIENTES], thr_pintors[N_PINTORES];
  int i; 
  struct Pintor id_bar[N_PINTORES];
  struct Cliente id_cl[N_CLIENTES];

  sem_init(&sem_cadeiras, 0, N_CADEIRAS);
  sem_init(&sem_estados, 0, 1);
  for (i = 0; i < N_CADEIRAS; i++){
    sem_wait(&sem_estados);
    estadoCadeiraCliente[i] = F;
    sem_post(&sem_estados);
  }

  for (i = 0; i < N_CLIENTES; i++){
    sem_wait(&sem_estados);
    estadoCliente[i] = I;
    sem_post(&sem_estados);
  }

  for (i = 0; i < N_PINTORES; i++){
    sem_wait(&sem_estados);
    estadoPintor[i] = S;
    sem_post(&sem_estados);
  }

  for (i = 0; i < N_PINTORES; i++) {
    sem_init(&sem_cad_pintor[i], 0, 1);
    sem_init(&sem_cliente_cadeira[i], 0, 0);
    sem_init(&sem_cabelo_cortado[i], 0, 0);
    sem_init(&sem_escreve_painel[i], 0, 1);
    sem_init(&sem_le_painel[i], 0, 0);
  }

  for (i = 0; i < N_TIPOS; i++) {
    id_bar[i].id = i;
    id_bar[i].tipo = i;
    printf("pintor %d: tipo %d\n", i, id_bar[i].tipo);
    pthread_create(&thr_pintors[i], NULL, f_pintor, (void*) &id_bar[i]);
  }
  for (i = N_TIPOS; i < N_PINTORES; i++) {
    id_bar[i].id = i;
    id_bar[i].tipo = random()%(N_TIPOS);
    printf("pintor %d: tipo %d\n", i, id_bar[i].tipo);
    pthread_create(&thr_pintors[i], NULL, f_pintor, (void*) &id_bar[i]);
  }
  
  for (i = 0; i < N_CLIENTES; i++) {
    id_cl[i].id = i;
    id_cl[i].tipo = rand()%(N_TIPOS);
    //Colocar a imagem.
    // id_cl[i].imagem = NULL;
    id_cl[i].imagem = random()%(100);
    printf("cliente %d: tipo %d - imagem %d\n", i, id_cl[i].tipo, id_cl[i].imagem);
    pthread_create(&thr_clientes[i], NULL, f_cliente, (void*) &id_cl[i]);
  }
  printf("\n");
  
  for (i = 0; i < N_CLIENTES; i++) 
    pthread_join(thr_clientes[i], NULL);

  /* Pintores são assassinados */
  
  return 0;
}