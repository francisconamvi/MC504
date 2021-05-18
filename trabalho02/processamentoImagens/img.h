#ifndef FOO_H_   /* Include guard */
#define FOO_H_

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <stdlib.h> 
#include <vector>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>

#define MEDIAN 3
#define LIMIAR 4
#define EDGE 5
#define ANIMATION_DELAY 10
#define N_CLIENTES 15


struct Cliente{
    int id;
    cv::Mat img;
    int tipo;
};

extern struct Cliente id_cl[N_CLIENTES];
extern sem_t semLine;

typedef struct args_thrd{
  cv::Mat imgOrig;
  cv::Mat *imgModfy;
  cv::Mat kernel;
  int offset;
  std::vector<int> *remainin;
  int type;
}args_thrd;


int cmpfunc (const void * a, const void * b);
int apply_kernel2pixel(cv::Mat img, cv::Mat kernel, int l, int c, int t);
void* thr_func(void* args);
int filtering_median_animation(cv::Mat img, int kernelSize);
int limiarization_animation(cv::Mat img, int kernelSize);
int edge_detection_animation(cv::Mat img);
struct Cliente* criar_cliente(int i);
int atender_cliente(struct Cliente c);


#endif 
