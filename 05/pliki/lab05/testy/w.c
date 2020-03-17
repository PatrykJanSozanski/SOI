#include<stdlib.h>
#include<unistd.h>
#include<lib.h>

#include"soi.h"


int main(int argc, char *argv[]){
	if(argc < 2)
		return 1;
	worst_fit(atoi(argv[1]));
	return 0;
}
