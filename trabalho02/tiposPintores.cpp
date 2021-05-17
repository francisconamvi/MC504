/*
 * Problema do pintor dorminhoco com vários pintores especializados.
 */ 
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

using namespace std;

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
sem_t sem_pintura_feita[N_PINTORES];
sem_t sem_cliente_cadeira[N_PINTORES];

sem_t sem_escreve_painel[N_TIPOS], sem_le_painel[N_TIPOS];
int painel[N_TIPOS]; /*painel dividido entre pintores de cada tipo*/
int cavalete[N_PINTORES]; /*cavaletes onde pintores realizam os trabalhos*/
int clientesCadeira[N_CADEIRAS];
int clientes_cavaletes[N_PINTORES];
struct Pintor id_pin[N_PINTORES];
struct Cliente id_cl[N_CLIENTES];

int ite=0;

void printAtelie(){
  refresh();
  int myY=1, myX=1;
  mvprintw(2, 30, "Bem vindo ao Atelie!!!");

  mvprintw(4, 3, "Entrada");
  myY=5;
  myX=3;
  int ce=0;
  for(int i=0; i<N_CLIENTES; i++){
    if(estadoCliente[i] == A){
      myY+=1;
      mvprintw(myY, myX, "%d(%d)", i, id_cl[i].tipo);
      ce++;
    }
  }
  for(int i=ce; i<N_CLIENTES; i++){
    myY+=1;
    mvprintw(myY, myX, "     ");
  }


  myY=3;
  myX=12;
  for(int i=0; i<10; i++){
    myY+=1;
    mvprintw(myY, myX, "|");
  }

  mvprintw(4, 14, "Sala de espera");
  myY=5;
  myX=14;
  for(int i=0; i<N_CADEIRAS; i++){
    myY+=1;
    mvprintw(myY, myX, "C%d -     ", i);
  }
  myY=5;
  myX=19;
  for(int i=0; i<N_CADEIRAS; i++){
    myY+=1;
    if(estadoCadeiraCliente[i] == B){
      mvprintw(myY, myX, "%d(%d)", clientesCadeira[i], id_cl[clientesCadeira[i]].tipo);
    }
  }

  myY=3;
  myX=29;
  for(int i=0; i<10; i++){
    myY+=1;
    mvprintw(myY, myX, "|");
  }
  
  mvprintw(4, 31, "Pintores restaurando");
  myY=5;
  myX=31;
  for(int i=0; i<N_PINTORES; i++){
    myY+=1;
    mvprintw(myY, myX, "P%d(%d) - ", i, id_pin[i].tipo);
  }
  myY=5;
  myX=39;
  for(int i=0; i<N_PINTORES; i++){
    myY+=1;
    if(clientes_cavaletes[i] != -1){
      mvprintw(myY, myX, "%d(%d)", clientes_cavaletes[i], id_cl[clientes_cavaletes[i]].tipo);
    }
    else{
      mvprintw(myY, myX, "      ");
    }
  }

  myY=3;
  myX=52;
  for(int i=0; i<10; i++){
    myY+=1;
    mvprintw(myY, myX, "|");
  }
  mvprintw(4, 54, "Clientes saindo");
  myY=5;
  myX=54;
  int cs=0;
  for(int i=0; i<N_CLIENTES; i++){
    if(estadoCliente[i] == E){
      myY+=1;
      mvprintw(myY, myX, "%d(%d)", i, id_cl[i].tipo);
      cs++;
    }
  }
  for(int i=cs; i<N_CLIENTES; i++){
    myY+=1;
    mvprintw(myY, myX, "     ");
  }

  refresh();
  sleep(1);
  return;
}

void* f_pintor(void *v) {
  struct Pintor pintor = *(struct Pintor*) v;

  while(1) {
    sem_wait(&sem_escreve_painel[pintor.tipo]);
    painel[pintor.tipo] = pintor.id;
    sem_post(&sem_le_painel[pintor.tipo]);
    sem_wait(&sem_cliente_cadeira[pintor.id]);

    //funcao processar imagem
    sleep(5);

    sem_post(&sem_pintura_feita[pintor.id]);
  }
  return NULL;
}

void* f_cliente(void* v) {
  sleep(1); /* Para esperar criar todos os clientes primeiro */
  struct Cliente cliente = *(struct Cliente*) v;
  int minha_cadeira;
  int minhaCadeiraCliente;

  sleep((random()%TEMPOS)*5); /* multiplica por 5 pra dar tempo de pintar */

  /* Cliente chegou no ateliê */
  sem_wait(&sem_estados);
  estadoCliente[cliente.id] = A;
  printAtelie();
  sem_post(&sem_estados);

  if (sem_trywait(&sem_cadeiras) == 0) {
    /* Cliente está esperando pra ser atendido e pegando uma cadeira */
    sem_wait(&sem_estados);
    estadoCliente[cliente.id] = W;
    for (int i = 0; i < N_CADEIRAS; i++) {
      if (estadoCadeiraCliente[i] == F) {
        estadoCadeiraCliente[i] = B;
        clientesCadeira[i] = cliente.id;
        minhaCadeiraCliente = i;
        break;
      }
    }

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
    estadoCadeiraCliente[minhaCadeiraCliente] = F;

    sem_post(&sem_cadeiras);

    /* Fazendo pintura*/
    clientes_cavaletes[minha_cadeira] = cliente.id;
    estadoCliente[cliente.id] = PC;
    estadoPintor[cliente.tipo] = PP;
    printAtelie();
    sem_post(&sem_estados);
    
    /* Cliente está esperando trabalho finalizar */
    sem_wait(&sem_pintura_feita[minha_cadeira]);

    sem_wait(&sem_estados);
    /* Cliente saiu do atelie e pintor está disponível */
    estadoCliente[cliente.id] = E;
    estadoPintor[minha_cadeira] = S;
    clientes_cavaletes[minha_cadeira] = -1;
    printAtelie();
    estadoCliente[cliente.id] = L;
    sem_post(&sem_estados);
    sem_post(&sem_cad_pintor[minha_cadeira]);
  }
  else {
    /* Não tinha cadeira para o cliente */
    sem_wait(&sem_estados);
    estadoCliente[cliente.id] = L;
    sem_post(&sem_estados);
  }
  return NULL;
}

int main() {
  srand(time(0));

  initscr();

  pthread_t thr_clientes[N_CLIENTES], thr_pintors[N_PINTORES];
  int i; 

  sem_init(&sem_cadeiras, 0, N_CADEIRAS);
  sem_init(&sem_estados, 0, 1);
  for (i = 0; i < N_CADEIRAS; i++)
    estadoCadeiraCliente[i] = F;

  for (i = 0; i < N_PINTORES; i++) {
    sem_init(&sem_cad_pintor[i], 0, 1);
    sem_init(&sem_cliente_cadeira[i], 0, 0);
    sem_init(&sem_pintura_feita[i], 0, 0);
    sem_init(&sem_escreve_painel[i], 0, 1);
    sem_init(&sem_le_painel[i], 0, 0);
    clientes_cavaletes[i] = -1;
  }

  for (i = 0; i < N_TIPOS; i++) {
    sem_init(&sem_escreve_painel[i], 0, 1);
    sem_init(&sem_le_painel[i], 0, 0);
  }

  for (i = 0; i < N_TIPOS; i++) {
    id_pin[i].id = i;
    id_pin[i].tipo = i;
    pthread_create(&thr_pintors[i], NULL, f_pintor, (void*) &id_pin[i]);
  }

  for (i = N_TIPOS; i < N_PINTORES; i++) {
    id_pin[i].id = i;
    id_pin[i].tipo = random()%(N_TIPOS);
    estadoPintor[i] = S;
    pthread_create(&thr_pintors[i], NULL, f_pintor, (void*) &id_pin[i]);
  }
  
  for (i = 0; i < N_CLIENTES; i++) {
    id_cl[i].id = i;
    id_cl[i].tipo = rand()%(N_TIPOS);
    estadoCliente[i] = I;
    //Colocar a imagem.
    id_cl[i].imagem = random()%(100);
    pthread_create(&thr_clientes[i], NULL, f_cliente, (void*) &id_cl[i]);
  }
  printf("\n");
  
  for (i = 0; i < N_CLIENTES; i++) 
    pthread_join(thr_clientes[i], NULL);

  /* Pintores são assassinados */
  endwin();
  
  return 0;
}