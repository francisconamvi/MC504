#include <ncurses.h>
using namespace std;

void init(){
    initscr();
    refresh();
    move(1,40);

    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    mvprintw(1, xMax/2, "Atelie foi aberto!!!");
    refresh();
    return;
}

void printAtelie(){
  printf("Iteração %d\n", ite);
  ite++;
  printf("Cliente chegou: ");
  for(int i=0; i<N_CLIENTES; i++){
    if(estadoCliente[i] == A){
      printf("%d(%d) ", i, id_cl[i].tipo);
    }
  }
  printf("\n");

  printf("Clientes na espera: ");
  for(int i=0; i<N_CLIENTES; i++){
    if(estadoCliente[i] == W){
      printf("%d(%d) ", i, id_cl[i].tipo);
    }
  }
  printf("\n");

  printf("Pintor pintando qual cliente: ");
  for(int i=0; i<N_PINTORES; i++){
    if(clientes_cavaletes[i] != -1){
      printf("%d(%d)-%d(%d) ", i, id_bar[i].tipo, clientes_cavaletes[i], id_cl[clientes_cavaletes[i]].tipo);
    }
    else{
      printf("%d(%d)- N   ", i, id_bar[i].tipo);
    }
  }
  printf("\n");

  printf("Cliente saiu depois de ter pintado: ");
  for(int i=0; i<N_CLIENTES; i++){
    if(estadoCliente[i] == E){
      printf("%d(%d) ", i, id_cl[i].tipo);
    }
  }
  printf("\n");

  printf("----------------------------------\n");
  return;
}