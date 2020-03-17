#include <stdio.h>
#include <stdlib.h>
#include <lib.h>
#include <unistd.h>
#include <limits.h>

int getgroup(int pid);
void setgroup(int pid, int group);

int getgroup(int pid){
	message m;
	m.m1_i1 = pid;
	printf("PID: %d \n", pid);
	return _syscall(MM, GETGROUP, &m);
}

void setgroup(int pid, int group){
	message m;
	m.m1_i1 = pid;
	m.m1_i2 = group;
	_syscall(MM, SETGROUP, &m);
}

int main(int argc, char* argv[]){

	int group;
	int newgroup;
	pid_t pid = getpid();
	
	group = getgroup(pid);
	printf("Group: %d\n", group);

	if(group){
		setgroup(pid, 0);
		newgroup = getgroup(pid);
		printf("Previous group: %d\nCurrent group: %d\n", group, newgroup);
	}
	else if(!group){
		setgroup(pid, 1);
		newgroup = getgroup(pid);
		printf("Previous group: %d\nCurrent group: %d\n", group, newgroup);
	}

	return 0;
}
