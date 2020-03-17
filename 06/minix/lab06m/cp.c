#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"fs.h"

int cp(char* name){
    FILE *fp, *cp_src;
    const char OCCUPIED = 0xff;

    FS_Superblock fs_sb;

    char iNodeBitmap[FS_INODES];
    char dataBitmap[FS_DATA_BLOCKS];
    FS_iNode iNodeTable[FS_INODES];

    int maxSize = FS_BLOCK_SIZE*FS_DATA_BLOCKS;
    char sourcebuffer[FS_BLOCK_SIZE*FS_DATA_BLOCKS];

    int fileSize;
    int requiredBlocks;

    time_t now;

    int i, j, k, z;

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
                printf("File already exists!\n");
                return FILEALREADYEXISTS;
            }
        }
    }

    cp_src = fopen(name, "r");
    if(!fp) {
        printf("Target file do not exist!\n");
        return TARGETFILENOTEXISTING;
    }
    fread(sourcebuffer, maxSize, 1, cp_src);
    fileSize = strlen(sourcebuffer);
    fclose(cp_src);

    requiredBlocks = 1 + (fileSize / fs_sb.blockSize);

    for(i = 0; i < fs_sb.iNodesNum; i++){
        if(iNodeBitmap[i] == '\0'){
            for(j = 0; j < fs_sb.dataBlockNum; j++){
                if(dataBitmap[j] == '\0'){
                    int notAvailable = 0;
                    for(k = 0; k < requiredBlocks; k++){
                        if(dataBitmap[j + k] == '\0'){
                            continue;
                        }
                        notAvailable = 1;
                    }

                    if(notAvailable){
                        continue;
                    }

                    iNodeTable[i].startingBlock = fs_sb.dataBlockOffset + j;
                    iNodeTable[i].accessRights = 0x07;
                    iNodeTable[i].size = fileSize;
                    time(&now);
                    iNodeTable[i].lastAccessed = now;
                    iNodeTable[i].lastModified = now;
                    strcpy(iNodeTable[i].name, name);

                    fp = fopen("vsfs", "r+b");
                    if(!fp) {
                        printf("No file system existing!\n");
                        return NOFSYSTEMEXISTING;
                    }

                    fseek(fp, fs_sb.iNodeOccupancyBitmapOffset*fs_sb.blockSize + i*sizeof(char), SEEK_SET);
                    fwrite(&OCCUPIED, sizeof(char), 1, fp);
                    for(z = 0; z < requiredBlocks; z++){
                        fseek(fp, fs_sb.dataOccupancyBitmapOffset*fs_sb.blockSize + (j+z)*sizeof(char), SEEK_SET);
                        fwrite(&OCCUPIED, sizeof(char), 1, fp);
                    }

                    fseek(fp, fs_sb.iNodesOffset*fs_sb.blockSize + i*fs_sb.iNodeSize, SEEK_SET);
                    fwrite(iNodeTable+i, fs_sb.iNodeSize, 1, fp);

                    fseek(fp, (fs_sb.dataBlockOffset+j)*fs_sb.blockSize, SEEK_SET);
                    fwrite(sourcebuffer, fileSize, 1, fp);

                    fflush(fp);
                    fclose(fp);
                    return 0;
                }
            }

        }
    }
    return NOAVAILABLEINODES;
}

