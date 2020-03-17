#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"fs.h"

int load(char* name){
    FILE *fp, *cp_src;
    const char OCCUPIED = 0xff;

    FS_Superblock fs_sb;

    FS_iNode iNodeTable[FS_INODES];
    char iNodeBitmap[FS_INODES];

    int startingAddress;
    int size = FS_INODE_SIZE;
    char fileBuffer[FS_INODE_SIZE+1];

    int i;

    fp = fopen("vsfs", "r+b");
    if(!fp) {
        printf("No file system existing!\n");
        return NOFSYSTEMEXISTING;
    }
    fread(&fs_sb, sizeof(FS_Superblock), 1, fp);
    fclose(fp);

    fp = fopen("vsfs", "a+b");
    if(!fp) {
        printf("No file system existing!\n");
        return NOFSYSTEMEXISTING;
    }

    fseek(fp, fs_sb.iNodeOccupancyBitmapOffset*fs_sb.blockSize, SEEK_SET);
    fread(iNodeBitmap, sizeof(char), fs_sb.iNodesNum, fp);

    fseek(fp, fs_sb.iNodesOffset*fs_sb.blockSize, SEEK_SET);
    fread(iNodeTable, fs_sb.iNodeSize, fs_sb.iNodesNum, fp);

    fclose(fp);
    for(i = 0; i < fs_sb.iNodesNum; i++){
        if(iNodeBitmap[i] != '\0'){
            if(strcmp(iNodeTable[i].name, name) == 0){

                startingAddress = iNodeTable[i].startingBlock*fs_sb.blockSize;

                fp = fopen("vsfs", "a+b");
                if(!fp) {
                    printf("No file system existing!\n");
                    return NOFSYSTEMEXISTING;
                }

                fseek(fp, startingAddress, SEEK_SET);
                fread(fileBuffer, sizeof(char), size, fp);

                fclose(fp);
                fileBuffer[size] = '\0';

                fp = fopen(name, "w");
                fwrite(fileBuffer, strlen(fileBuffer), 1, fp);
                fclose(fp);
                return 0;
            }
        }
    }
    printf("File doesnt exist\n");
    return FILENOTEXISTINGINFS;
}
