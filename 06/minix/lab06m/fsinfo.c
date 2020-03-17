#include<stdio.h>
#include<stdlib.h>

#include"fs.h"

int fsinfo(void){
    FILE *fp, *cp_src;
    const char OCCUPIED = 0xff;

    FS_Superblock fs_sb;

    char iNodeBitmap[FS_INODES];
    char dataBitmap[FS_DATA_BLOCKS];
    FS_iNode iNodeTable[FS_INODES];

    int iNodeCounter;
    int realSpaceUsage;
    int dataBlockCounter;
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

    fseek(fp, fs_sb.dataOccupancyBitmapOffset*fs_sb.blockSize, SEEK_SET);
    fread(dataBitmap, sizeof(char), fs_sb.dataBlockNum, fp);

    fseek(fp, fs_sb.iNodesOffset*fs_sb.blockSize, SEEK_SET);
    fread(iNodeTable, fs_sb.iNodeSize, fs_sb.iNodesNum, fp);

    fclose(fp);

    iNodeCounter = 0;
    realSpaceUsage = 0;
    for(i = 0; i < fs_sb.iNodesNum; i++){
        if(iNodeBitmap[i] != 0x00){
            ++iNodeCounter;
            realSpaceUsage += iNodeTable[i].size;
        }
    }

    printf("iNode table starts at 0x%x and its occupied in %.2f%%\n", fs_sb.iNodesOffset*fs_sb.blockSize, 100*(float)iNodeCounter/(float)fs_sb.iNodesNum);
    printf("File system stores %d %s\n", iNodeCounter, iNodeCounter == 1 ? "file" : "files");
    printf("iNode bitmap: \n");
    for(i = 0; i < fs_sb.iNodesNum; i++){
        printf("|%c", iNodeBitmap[i] == 0x00 ? 'O' : 'X');
    }
    printf("|\n");

    dataBlockCounter = 0;
    for(i = 0; i < fs_sb.dataBlockNum; i++){
        if(dataBitmap[i] != 0x00) ++dataBlockCounter;
    }

    printf("Data space starts at 0x%x and its occupied in %.2f%%\n", fs_sb.dataBlockOffset*fs_sb.blockSize, 100*(float)dataBlockCounter/(float)fs_sb.dataBlockNum);
    printf("Real data space usage is equal %.2f%% (%d bytes)\n", 100*(float)realSpaceUsage/((float)fs_sb.dataBlockNum*fs_sb.blockSize), realSpaceUsage);
    printf("Data bitmap: \n");
    for(i = 0; i < fs_sb.dataBlockNum; i++){
        printf("|%c", dataBitmap[i] == 0x00 ? 'O' : 'X');
    }
    printf("|\n");

    return 0;
}
