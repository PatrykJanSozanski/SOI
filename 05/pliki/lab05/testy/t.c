#include<stdio.h>
#include<unistd.h>
#include<lib.h>

#include"soi.h"

int main(int argc, char *argv[]){
	unsigned int b[1024];
	unsigned int *p, a, l;
	int res;

	res = hole_map(b, sizeof(b));
	printf("[%d]\t", res);
	p = b;
	while(*p){
		l = *p++;
		a = *p++;
		printf("%d:", l);
		printf("%d\t", a);
	}
	printf("\n");
	return 0;

}
