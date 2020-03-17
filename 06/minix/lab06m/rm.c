#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"fs.h"

int rm(char* name){
    FILE *fp, *cp_src;
    char* FREE = 0x00;
    FS_Superblock fs_sb;
    char iNodeBitmap[FS_INODES];
    char dataBitmap[FS_DATA_BLOCKS];
    FS_iNode iNodeTable[FS_INODES];

    int maxSize = FS_BLOCK_SIZE*FS_DATA_BLOCKS;
    char sourcebuffer[FS_BLOCK_SIZE*FS_DATA_BLOCKS];
    int bitmapBlockStart;
    int bitmapBlockNum;

    int i, z;

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

    fseek(fp, fs_sb.dataOccupancyBitmapOffset*fs_sb.blockSize, SEEK_SET);
    fread(dataBitmap, sizeof(char), fs_sb.dataBlockNum, fp);

    fseek(fp, fs_sb.iNodesOffset*fs_sb.blockSize, SEEK_SET);
    fread(iNodeTable, fs_sb.iNodeSize, fs_sb.iNodesNum, fp);

    fclose(fp);

    for(i = 0; i < fs_sb.iNodesNum; i++){
        if(iNodeBitmap[i] != '\0'){
            if(strcmp(iNodeTable[i].name, name) == 0){

                bitmapBlockStart = iNodeTable[i].startingBlock - fs_sb.dataBlockOffset;
                bitmapBlockNum = 1 + iNodeTable[i].size / fs_sb.blockSize;


                fp = fopen("vsfs", "r+b");
                if(!fp) {
                    printf("No file system existing!\n");
                    return NOFSYSTEMEXISTING;
                }
                fseek(fp, fs_sb.iNodeOccupancyBitmapOffset*fs_sb.blockSize + i*sizeof(char), SEEK_SET);
                fwrite(&FREE, sizeof(char), 1, fp);
                for(z = 0; z < bitmapBlockNum; z++){
                    fseek(fp, fs_sb.dataOccupancyBitmapOffset*fs_sb.blockSize + (z+i)*sizeof(char), SEEK_SET);
                    fwrite(&FREE, sizeof(char), 1, fp);
                }

                fflush(fp);
                fclose(fp);
                return 0;
            }
        }
    }
    printf("Could not find file\n");
    return FILENOTEXISTINGINFS;
}
