#include<stdio.h>
#include<stdlib.h>

int main(){
    srand(time(0));
    for(int i=0; i<5; i++){
        printf("%d\n", rand());
    }
    return 0;
}