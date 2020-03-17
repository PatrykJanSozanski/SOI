#include <stdio.h>
#include <stdlib.h>
#include <lib.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <math.h>

int main(int argc, char* argv[]){

	clock_t start, end;
	time_t s, e;
	double dtime, dt;
	int i;

	start = clock();
	time (&s);
	for(i = 0; i < 2200; i++){
		sqrt(i);
	}
	end = clock();
	time (&e);
	dtime = ((double)(end - start))/CLOCKS_PER_SEC;
	dt = difftime(e,s);

	printf("PID: %d\nTime: %fs\n", getpid(), dtime);
	printf("Time: %fs\n", dt);

	return 0;
}
