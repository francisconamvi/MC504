#include <stdio.h>
#include <stdlib.h>
#define M 10
#define N 10

void copy_matrix(int **a, int **b, int m, int n){
    for(int i=0; i<m; i++){
        for(int j=0; j<n; j++){
            b[i][j] = a[i][j];
        }
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

void next_step(int **h, int **h_, int m, int n){
    for(int i=0; i<m; i++){
        h_[i][0] = h[i][0];
        h_[i][n-1] = h[i][n-1];
    }
    for(int j=0; j<n; j++){
        h_[0][j] = h[0][j];
        h_[m-1][j] = h[m-1][j];
    }

    //middle
    for(int i=1; i<m-1; i++){
        for(int j=1; j<n-1; j++){
            int acc = 0;
            for(int x=i-1; x<i+2; x++){
                for(int y=j-1; y<j+2; y++){
                    acc = acc + h[x][y];
                }
            }
            h_[i][j] = acc / 9;
        }
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
        next_step(h, h_,m,n);
        copy_matrix(h_, h,m,n);
        printf("Step %d\n", s);
        print_h(h,m,n);
    }
    

    return 0;
}