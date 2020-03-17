#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"fs.h"

int mkfs(){
    FILE* fp; /* sprawdzanie, czy wirtualny sysyem plikow juz istnieje */
    fp = fopen(FS_NAME, "r");
    if(fp){
        printf("Filesystem already exists!\n");
        fclose(fp);
        return FSYSALREADYEXISTS;
    }

    char nullSign = '\0';
    fp = fopen(FS_NAME, "a+b"); /* tworzenie wirtualnego systemu plikow */
    if(!fp) {
        printf("No file system existing!\n");
        return COULDNTCREATEFSYS;
    }
    int i;
    for(i = 0; i < FS_SIZE; i++){ /* czyszczenie pamieci */
        fwrite(&nullSign, sizeof(char), 1, fp);
    }
    fclose(fp);

    FS_Superblock fs_sb; /* tworzenie Superblocku */
    fs_sb.blockSize = 4096;

    fs_sb.iNodesBlockNum = 5;
    fs_sb.iNodesNum = 80;
    fs_sb.iNodeSize = 128;
    fs_sb.iNodesOffset = 3;

    fs_sb.dataBlockNum = 56;
    fs_sb.dataBlockOffset = 8;

    fs_sb.iNodeOccupancyBitmapOffset = 1;
    fs_sb.dataOccupancyBitmapOffset = 2;

    fp = fopen(FS_NAME, "r+b"); /* proba otworzenia i zapis Sssuperblocku */
    if(!fp) {
        printf("No file system existing!\n");
        return COULDNTCREATEFSYS;
    }
    fwrite(&fs_sb, sizeof(FS_Superblock), 1, fp);
    fclose(fp);

    FS_iNodeOccupancyBitmap inodeOBitmap; /* tworzenie bitowych map zajetosci */
    for(i = 0; i < FS_INODES; i++){
        inodeOBitmap.occupied[i] = '\0';
    }

    FS_dataOccupancyBitmap dataOBitmap;
    for(i = 0; i < FS_DATA_BLOCKS; i++){
        dataOBitmap.occupied[i] = '\0';
    }

    char fs_buffer[FS_SIZE];
    fp = fopen(FS_NAME, "r+b");
    if(!fp) {
        printf("No file system existing!\n");
        return COULDNTCREATEFSYS;
    }
    fread(fs_buffer, FS_SIZE, 1, fp); /* czytanie zawartosci pamieci do bufora */
    fclose(fp);

    memcpy(fs_buffer+(fs_sb.blockSize*fs_sb.iNodeOccupancyBitmapOffset), /* zapisywanie mapy do bufora */
           inodeOBitmap.occupied,
           sizeof(FS_iNodeOccupancyBitmap));

    memcpy(fs_buffer+(fs_sb.blockSize*fs_sb.dataOccupancyBitmapOffset), /*zapisywanie mapy do bufora */
           dataOBitmap.occupied,
           sizeof(FS_dataOccupancyBitmap));

    fp = fopen(FS_NAME, "wb");
    if(!fp) {
        printf("No file system existing!\n");
        return COULDNTCREATEFSYS;
    }
    fwrite(fs_buffer, FS_SIZE, 1, fp); /* zapisywanie map w pamieci */
    fclose(fp);

    return 0;
}