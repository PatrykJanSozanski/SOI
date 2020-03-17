#include<unistd.h>
#include<lib.h>

#include"soi.h"

#define FIRST_FIT 0
#define WORST_FIT 1

PUBLIC int hole_map(void *buffer, size_t nbytes){
	message input;
	input.m1_p1 = buffer;
	input.m1_i1 = nbytes;
	return _syscall(MM, 78, &input);
}

PUBLIC int worst_fit(int w){
	message input;
	input.m1_i1 = w;
	return _syscall(MM, 79, &input);
}
