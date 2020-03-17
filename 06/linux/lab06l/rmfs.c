#include<stdio.h>
#include<unistd.h>

#include"fs.h"

int rmfs(){ /* usuwanie wirtualnego systemu plikow */
    FILE* fp;
    fp = fopen(FS_NAME, "r");
    if(fp){ /* sprawdza, wirtualny system plikow istnieje */
        fclose(fp);
        unlink(FS_NAME); /* usuwa wirtualny system plikow */
        return 0;
    }
    printf("No filesystem exists!\n");
    return NOFSYSTEMEXISTING;
}

