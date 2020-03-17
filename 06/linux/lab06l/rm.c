#include<stdio.h>
#include<string.h>

#include"fs.h"

int rm(char* name){
    FILE *fp, *cp_src;
    char* FREE = 0x00;
    FS_Superblock fs_sb;

    fp = fopen(FS_NAME, "r+b"); /* sprawdzanie... */
    if(!fp) {
        printf("No file system existing!\n");
        return NOFSYSTEMEXISTING;
    }
    fread(&fs_sb, sizeof(FS_Superblock), 1, fp); /* czytanie danych z Superblocku */
    fclose(fp);

    char iNodeBitmap[fs_sb.iNodesNum];
    char dataBitmap[fs_sb.dataBlockNum];
    FS_iNode iNodeTable[fs_sb.iNodesNum];

    fp = fopen(FS_NAME, "a+b");
    if(!fp) {
        printf("No file system existing!\n");
        return NOFSYSTEMEXISTING;
    }

    /* wczytywanie mapy iNodow */
    fseek(fp, fs_sb.iNodeOccupancyBitmapOffset*fs_sb.blockSize, SEEK_SET);
    fread(iNodeBitmap, sizeof(char), fs_sb.iNodesNum, fp);

    /* wczytywanie mapy user data */
    fseek(fp, fs_sb.dataOccupancyBitmapOffset*fs_sb.blockSize, SEEK_SET);
    fread(dataBitmap, sizeof(char), fs_sb.dataBlockNum, fp);

    /* wczytywanie tablicy iNodow */
    fseek(fp, fs_sb.iNodesOffset*fs_sb.blockSize, SEEK_SET);
    fread(iNodeTable, fs_sb.iNodeSize, fs_sb.iNodesNum, fp);

    fclose(fp);

    int maxSize = fs_sb.blockSize*fs_sb.dataBlockNum;
    char sourcebuffer[maxSize];
    int i;
    for(i = 0; i < fs_sb.iNodesNum; i++){ /* sprawdzanie, czy plik znajduje sie w pamieci */
        if(iNodeBitmap[i] != '\0'){
            if(strcmp(iNodeTable[i].name, name) == 0){

                /* znajdowanie bloku poczatku i ilosci blokow zjamowanych przez plik */
                int bitmapBlockStart = iNodeTable[i].startingBlock - fs_sb.dataBlockOffset;
                int bitmapBlockNum = 1 + iNodeTable[i].size / fs_sb.blockSize;


                fp = fopen(FS_NAME, "r+b");
                if(!fp) {
                    printf("No file system existing!\n");
                    return NOFSYSTEMEXISTING;
                }

                /* zwalnianie pamieci */
                fseek(fp, fs_sb.iNodeOccupancyBitmapOffset*fs_sb.blockSize + i*sizeof(char), SEEK_SET);
                fwrite(&FREE, sizeof(char), 1, fp);
                int z;
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
