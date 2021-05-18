#include "img.h"
#include <X11/Xlib.h>   
struct Cliente id_cl[N_CLIENTES];
sem_t semLine;
sem_t semPrint0;
sem_t semPrint1;
sem_t semPrint2;
sem_t semCria;

void* tTeste(void* v){
  struct Cliente *cTeste = new struct Cliente;
  cTeste = ((struct Cliente*)v);

  if(cTeste->tipo == EDGE){
    sem_wait(&semPrint0);
    int aux = atender_cliente(*cTeste);
    sem_post(&semPrint0);
  }
  if(cTeste->tipo == MEDIAN){
    sem_wait(&semPrint1);
    int aux = atender_cliente(*cTeste);
    sem_post(&semPrint1);
  }
  if(cTeste->tipo == LIMIAR){
    sem_wait(&semPrint2);
    int aux = atender_cliente(*cTeste);
    sem_post(&semPrint2);
  }
}

int main( int argc, char** argv ) {


  XInitThreads();
  srand(time(0));
  sem_init(&semLine, 0, 1);
  sem_init(&semPrint0, 0, 1);
  sem_init(&semPrint1, 0, 1);
  sem_init(&semPrint2, 0, 1);

  cv::namedWindow( "Borda", cv::WINDOW_AUTOSIZE );
  cv::namedWindow( "Limiar", cv::WINDOW_AUTOSIZE );
  cv::namedWindow( "Filtro", cv::WINDOW_AUTOSIZE );



  pthread_t *thr_clientes = new pthread_t[N_CLIENTES];


  for(int i = 0; i < N_CLIENTES; i++){
    struct Cliente *cTeste = new struct Cliente;
    cTeste = criar_cliente(i);

    pthread_create(&thr_clientes[i], NULL, tTeste, (void*)cTeste);
  }

  for(int i = 0; i < N_CLIENTES; i++){
    pthread_join(thr_clientes[i], NULL);

  }
  return 0;
}
