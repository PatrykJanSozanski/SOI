#include<stdio.h>
#include<unistd.h>

#include"fs.h"

int rmfs(void){
    FILE* fp;
    fp = fopen("vsfs", "r");
    if(fp){
        fclose(fp);
        unlink("vsfs");
        return 0;
    }
    printf("No filesystem exists!\n");
    return NOFSYSTEMEXISTING;
}

