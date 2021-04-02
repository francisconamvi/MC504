/*
 * Criação de uma nova thread. A thread principal deve esperar!
 */
#include <pthread.h>
#include <stdio.h>


void* f_thread(void *v) {
  printf("Nova Thread.\n");
  return NULL;
}

int main() {
  pthread_t thr;

  for(int i=0; i<3; i++){
    if (pthread_create(&thr, NULL, f_thread, NULL))
    fprintf(stderr, "Erro na criação da thread. \n");
  }
  pthread_join(thr, NULL);
  
  return 0;
}
