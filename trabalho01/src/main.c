#include <stdio.h>
#include <stdlib.h>

struct arg_struct {
    int **mat;
    int **mat2;
    int i;
    int x;
    int y;
};

void* copy_matrix_thr(void *args) {
    struct arg_struct *a = (struct arg_struct *)args;
    for(int u=0; u<a->y; u++){
        a->mat2[a->i][u] = a->mat[a->i][u];
    }
}

void copy_matrix(int **a, int **b, int m, int n){
    int N_THR = m;
    pthread_t thr[N_THR];
    int i;
    struct arg_struct args[N_THR];
    for (i = 0; i < N_THR; i++){
        args[i].mat = a;
        args[i].mat2 = b;
        args[i].i = i;
        args[i].x = m;
        args[i].y = n;
        pthread_create(&thr[i], NULL, copy_matrix_thr, (void*)&args[i]);
    }
    for (i = 0; i < N_THR; i++){
        pthread_join(thr[i], NULL); 
    }
}

void print_h(int **h, int m, int n){
    for(int i=0; i<m; i++){
        for(int j=0; j<n; j++){
            printf("%d ", h[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void inicialize_h(int **h, int m, int n, int a, int b, int c, int d){
    for(int i=0; i<m; i++){
        for(int j=0; j<n; j++){
            if(j==0){
                h[i][j] = a;
            }
            else if(j==n-1){
                h[i][j] = b;
            }
            else if(i==0){
                h[i][j] = c;
            }
            else if(i==m-1){
                h[i][j] = d;
            }
            else{
                h[i][j] = 0;
            }
        }
    }
    h[0][0] = (a+c)/2;
    h[0][n-1] = (b+c)/2;
    h[m-1][0] = (a+d)/2;
    h[m-1][n-1] = (b+d)/2;
}

void* next_step_thr(void *args) {
    struct arg_struct *a = (struct arg_struct *)args;
    //First line or last
    if(a->i == 0 || a->i == (a->x)-1){
        for(int u=0; u< a->y; u++){
            a->mat2[a->i][u] = a->mat[a->i][u];
        }
    }
    //everything else
    else{
        for(int u=0; u< a->y; u++){
            if(u==0 || u==(a->y)-1){
                a->mat2[a->i][u] = a->mat[a->i][u];
            }
            else{
                int acc = 0;
                for(int x=(a->i)-1; x<(a->i)+2; x++){
                    for(int y=u-1; y<u+2; y++){
                        acc = acc + a->mat[x][y];
                    }
                }
                a->mat2[a->i][u] = acc / 9;
            }
        }
    }

    return NULL; 
}

void next_step(int **h, int **h_, int m, int n){
    int N_THR = m;
    pthread_t thr[N_THR];
    int i;
    struct arg_struct args[N_THR];
    for (i = 0; i < N_THR; i++){
        args[i].mat = h;
        args[i].mat2 = h_;
        args[i].i = i;
        args[i].x = m;
        args[i].y = n;
        pthread_create(&thr[i], NULL, next_step_thr, (void*)&args[i]);
    }

    for (i = 0; i < N_THR; i++){
        pthread_join(thr[i], NULL); 
    }
}

int main(){
    int m, n;
    scanf(" %d%d", &m,&n);
    int **h;
    int **h_;
    h = (int**) malloc(m*sizeof(int*));
    for(int i=0; i<m; i++){
        h[i] = (int*) malloc(n*sizeof(int));
    }

    h_ = (int**) malloc(m*sizeof(int*));
    for(int i=0; i<m; i++){
        h_[i] = (int*) malloc(n*sizeof(int));
    }
    int a, b, c, d;
    scanf(" %d%d%d%d", &a,&b,&c,&d);
    int S;
    scanf(" %d", &S);
    printf("%d %d %d %d %d %d %d\n", m,n,a,b,c,d,S);

    inicialize_h(h,m,n,a,b,c,d);
    printf("Step 0\n");
    print_h(h,m,n);

    for(int s=1; s<=S; s++){
        next_step(h, h_, m, n);
        copy_matrix(h_, h,m,n);
        printf("Step %d\n", s);
        print_h(h,m,n);
    }
    
    return 0;
}