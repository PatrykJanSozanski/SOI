#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"fs.h"

int mkfs(void){

    FILE* fp;
    FS_Superblock fs_sb;
    FS_iNodeOccupancyBitmap inodeOBitmap;
    FS_dataOccupancyBitmap dataOBitmap;
    char nullSign;
    int i;
    char fs_buffer[FS_SIZE];
	
    fp = fopen("vsfs", "r");
    if(fp){
        printf("Filesystem already exists!\n");
        fclose(fp);
        return FSYSALREADYEXISTS;
    }


    nullSign = '\0';
    fp = fopen("vsfs", "a+b");
    if(!fp) {
        printf("No file system existing!\n");
        return COULDNTCREATEFSYS;
    }

    for(i = 0; i < FS_SIZE; i++){
        fwrite(&nullSign, sizeof(char), 1, fp);
    }
  
    fclose(fp);

    fs_sb.blockSize = FS_BLOCK_SIZE;

    fs_sb.iNodesBlockNum = 5;
    fs_sb.iNodesNum = FS_INODES;
    fs_sb.iNodeSize = FS_INODE_SIZE;
    fs_sb.iNodesOffset = 3;

    fs_sb.dataBlockNum = FS_DATA_BLOCKS;
    fs_sb.dataBlockOffset = 8;

    fs_sb.iNodeOccupancyBitmapOffset = 1;
    fs_sb.dataOccupancyBitmapOffset = 2;

    fp = fopen("vsfs", "r+b");
    if(!fp) {
        printf("No file system existing!\n");
        return COULDNTCREATEFSYS;
    }

    fwrite(&fs_sb, sizeof(FS_Superblock), 1, fp);
    fclose(fp);

    for(i = 0; i < FS_INODES; i++){
        inodeOBitmap.occupied[i] = '\0';
    }

    for(i = 0; i < FS_DATA_BLOCKS; i++){
        dataOBitmap.occupied[i] = '\0';
    }
 

    fp = fopen("vsfs", "r+b");
    if(!fp) {
        printf("No file system existing!\n");
        return COULDNTCREATEFSYS;
    }
    fread(fs_buffer, FS_SIZE, 1, fp);
    fclose(fp);

    memcpy(fs_buffer+(fs_sb.blockSize*fs_sb.iNodeOccupancyBitmapOffset),
           inodeOBitmap.occupied,
           sizeof(FS_iNodeOccupancyBitmap));

    memcpy(fs_buffer+(fs_sb.blockSize*fs_sb.dataOccupancyBitmapOffset),
           dataOBitmap.occupied,
           sizeof(FS_dataOccupancyBitmap));

    fp = fopen("vsfs", "wb");
    if(!fp) {
        printf("No file system existing!\n");
        return COULDNTCREATEFSYS;
    }
    fwrite(fs_buffer, FS_SIZE, 1, fp);
    fclose(fp);

    return 0;
}
