#include <stdio.h>
#include <stdlib.h>
#include <lib.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <math.h>

int setsched(int quantum);
int getgroup(int pid);
int getsched(void);

int setsched(int quantum){
	message m;
	m.m1_i1 = quantum;
	printf("Setting new time coefficient: %d\n", quantum);
	return _syscall(MM, SETSCHED, &m);
}
int getgroup(int pid){
	message m;
	m.m1_i1 = pid;
	return _syscall(MM, GETGROUP, &m);
}
int getsched(void){
	message m;
	return _syscall(MM, GETSCHED, &m);
}

int main(int argc, char* argv[]){

	int quantum;
	int result;
	pid_t pid = getpid();
	int group = getgroup(pid);
	int oldq, newq;

	if(argc != 2){
		printf("Invalid argument!\n");
		return 0;
	}

	oldq = getsched();
	printf("Coefficient value before: %d\n", oldq);

	quantum = atoi(argv[1]);
	printf("Entered time coefficient value: %d\n", quantum);

	result = setsched(quantum);

	if(result == 0)
		printf("PID: %d\nGroup: %d\nOperation permitted.\n", pid, group);
	else
		printf("PID: %d\nGroup: %d\nOperation banned.\n", pid, group);


	newq = getsched();
	printf("Coefficient value after: %d\n", newq);

	return 0;
}
