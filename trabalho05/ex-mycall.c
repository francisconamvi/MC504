/* 
 * Teste da nova chamada de sistema
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	int r, uid, weight;
	uid=3;
	weight=4;
       	r = syscall(449, uid, weight);
	printf("Setando o peso do usuario %d para %d. Retorno: %d \n", uid, weight, r);
	
	r = syscall(448, uid);
	printf("Lendo o peso do usuario %d. Retorno: %d \n", uid, r);

	uid=5;
	r = syscall(448, uid);
	printf("Lendo o peso do usuario %d. Retorno: %d \n", uid, r);
	
	uid=101;
	weight=2;
       	r = syscall(449, uid, weight);
	printf("Setando o peso do usuario %d para %d. Retorno: %d \n", uid, weight, r);


	return r;
}
