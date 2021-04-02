/*
 * Passagem de parâmetros para a thread. 
 * Um inteiro pode ser recebido disfarçado de apontador.
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_THR 10

struct arg_struct {
    int **mat;
    int **mat2;
    int i;
};

void* f_thread(void *args) {
  struct arg_struct *a = (struct arg_struct *)args;
  int i = (int) a->i;
  for(int y=0; y<N_THR; y++){
    a->mat2[i][y] = i;
  }

  printf("Thread %d\n", i);  
  return NULL; 
} 

int main() {
  pthread_t thr[N_THR];
  int i;
  int **mat = (int**)malloc(N_THR*sizeof(int*));
  for(int x=0; x<N_THR; x++){
    mat[x] = (int*)malloc(N_THR*sizeof(int));
  }
  for(int x=0; x<N_THR; x++){
    for(int y=0; y<N_THR; y++){
      mat[x][y] = 0;
    }
  }

  int **mat2 = (int**)malloc(N_THR*sizeof(int*));
  for(int x=0; x<N_THR; x++){
    mat2[x] = (int*)malloc(N_THR*sizeof(int));
  }
  for(int x=0; x<N_THR; x++){
    for(int y=0; y<N_THR; y++){
      mat2[x][y] = 0;
    }
  }

  struct arg_struct args[N_THR];
  for (i = 0; i < N_THR; i++){
    args[i].mat = mat;
    args[i].mat2 = mat2;
    args[i].i = i;
    pthread_create(&thr[i], NULL, f_thread, (void*)&args[i]);
  }

  for (i = 0; i < N_THR; i++) 
    pthread_join(thr[i], NULL); 

  for(int x=0; x<N_THR; x++){
    for(int y=0; y<N_THR; y++){
      printf("%d ", mat[x][y]);
    }
    printf("\n");
  }
  printf("\n");

  for(int x=0; x<N_THR; x++){
    for(int y=0; y<N_THR; y++){
      printf("%d ", mat2[x][y]);
    }
    printf("\n");
  }

  return 0;
}
