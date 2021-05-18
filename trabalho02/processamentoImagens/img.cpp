
#include "img.h"


int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int apply_kernel2pixel(cv::Mat img, cv::Mat kernel, int l, int c, int t){
  int height = kernel.size().height;
  int width = kernel.size().width;
  int offset = (int)width/2;
  float* array = new float[height*width];

  int count = 0;

  for(int i = l-offset; i <= l+offset; i++ ){
    for(int j = c-offset; j <= c+offset; j++ ){
      array[count++] = (float)kernel.at<float>(i-l+offset,j-c+offset)*(float)img.at<uchar>(i,j);
    }
  }
  qsort(array, height*width, sizeof(float), cmpfunc);
  if(t == MEDIAN){
    return (int)array[(int)height*width/2];
  }
  else if(t == LIMIAR){
    return (int)img.at<uchar>(l,c) > (array[height*width-1] + array[0])/2 ? 0 : 255;
  }
  else if(t == EDGE){
    int sum = 0;
    for(int i = 0; i < height*width; i++){
      sum+=array[i];
    }
    if(sum >= 255) return 255;
    else if(sum <= 0) return 0;
    else return sum; 
  }
  else{
    return 0;
  }
}

void* thr_func(void* args){
  args_thrd *a = (args_thrd *)args;
  int width = a->imgOrig.size().width;
  int offset = a->offset;
  cv::Mat img = a->imgOrig;
  cv::Mat *imgClone = a->imgModfy;
  cv::Mat kernel = a->kernel;
  std::vector<int> *v = a->remainin;

  int iRand = rand()%v->size();

  sem_wait(&semLine);
  int line = (*v)[iRand];
  v->erase(v->begin()+iRand);
  sem_post(&semLine);

  for(int j = offset; j < width - 1 - offset; j++){
    imgClone->at<uchar>(line,j) = apply_kernel2pixel(img, kernel, line, j, a->type);
  }
  return NULL;
}


int filtering_median_animation(cv::Mat img, int kernelSize){
  cv::Mat imgClone = img.clone();
  cv::Mat kernel;
  kernel = cv::Mat::ones( kernelSize, kernelSize, CV_32F );
  int height = img.size().height;
  int width = img.size().width;
  int offset = (int)kernelSize/2;
  std::vector<int> v;
  for(int i = offset; i<height-1-offset; i++){
    v.push_back(i);
  }
  args_thrd *args = new args_thrd[height-1-offset];
  pthread_t *thr = new pthread_t[height-1-offset];

  for(int i = offset; i < height - 1 - offset; i++){
    args[i].imgOrig = img;
    args[i].imgModfy = &imgClone;
    args[i].kernel = kernel;
    args[i].offset = offset;
    args[i].remainin = &v;
    args[i].type = MEDIAN;
    pthread_create(&thr[i-offset], NULL, thr_func, (void*)&args[i]);
    cv::imshow( "Filtro", imgClone );
    cv::waitKey(ANIMATION_DELAY);

  }
  for(int i = offset; i < height - 1 - 2*offset; i++){
    pthread_join(thr[i-offset], NULL);
  }

  return 0;
}

int limiarization_animation(cv::Mat img, int kernelSize){
  cv::Mat imgClone = img.clone();
  cv::Mat kernel;
  kernel = cv::Mat::ones( kernelSize, kernelSize, CV_32F );
  int height = img.size().height;
  int width = img.size().width;
  int offset = (int)kernelSize/2;
  std::vector<int> v;
  for(int i = offset; i<height-1-offset; i++){
    v.push_back(i);
  }
  args_thrd *args = new args_thrd[height-1-offset];
  pthread_t *thr = new pthread_t[height-1-offset];

  for(int i = offset; i < height - 1 - offset; i++){
    args[i].imgOrig = img;
    args[i].imgModfy = &imgClone;
    args[i].kernel = kernel;
    args[i].offset = offset;
    args[i].remainin = &v;
    args[i].type = LIMIAR;
    pthread_create(&thr[i-offset], NULL, thr_func, (void*)&args[i]);
    pthread_join(thr[i], NULL);
    cv::imshow( "Limiar", imgClone );
    cv::waitKey(ANIMATION_DELAY);

  }
  for(int i = offset; i < height - 1 - 2*offset; i++){
    pthread_join(thr[i-offset], NULL);
  }

  return 0;
}


int edge_detection_animation(cv::Mat img){
  cv::Mat imgClone = img.clone();
  cv::Mat kernel;
  float kernelAux[9] = {-1,-1,-1,-1,8,-1,-1,-1,-1};
  kernel = cv::Mat( 3, 3, CV_32F, kernelAux );
  int height = img.size().height;
  int width = img.size().width;
  int offset = (int)3/2;
  std::vector<int> v;
  for(int i = offset; i<height-1-offset; i++){
    v.push_back(i);
  }
  args_thrd *args = new args_thrd[height-1-offset];
  pthread_t *thr = new pthread_t[height-1-offset];


  for(int i = offset; i < height - 1 - offset; i++){
    args[i].imgOrig = img;
    args[i].imgModfy = &imgClone;
    args[i].kernel = kernel;
    args[i].offset = offset;
    args[i].remainin = &v;
    args[i].type = EDGE;

    pthread_create(&thr[i-offset], NULL, thr_func, (void*)&args[i]);
    cv::imshow( "Borda", imgClone );
    cv::waitKey(ANIMATION_DELAY);
  }
  for(int i = offset; i < height - 1 - 2*offset; i++){
    pthread_join(thr[i-offset], NULL);
  }

  return 0;
}

struct Cliente* criar_cliente(int i){
  struct Cliente *c = new struct Cliente;
  c->id = i;
  c->tipo = (i%3)+3;
  int imgName = i%5;
  std::string file = std::to_string(imgName);
  file = file+ ".png";
  std::string filePath;
  if(c->tipo == EDGE){
    filePath = "borda/"+file;
  }
  else if(c->tipo == MEDIAN){
    filePath = "ruido/"+file;
  }
  else if(c->tipo == LIMIAR){
    filePath = "limiar/"+file;
  }
  cv::Mat image1, image2;
  image1 = cv::imread(filePath , CV_LOAD_IMAGE_GRAYSCALE);
  if(! (image1.data) ) {
    std::cout <<  "Could not open or find the image" << std::endl ;
    exit(1);
  }
  cv::resize(image1, image2, cv::Size(350,350));
  c->img = image2;
  return c;
}

int atender_cliente(struct Cliente c){
  if(c.tipo == LIMIAR){
    return limiarization_animation(c.img, 9);
  }
  else if(c.tipo == MEDIAN){
    return filtering_median_animation(c.img, 9);
  }
  else if(c.tipo == EDGE){
    return edge_detection_animation(c.img);
  }
}

