#include<stdio.h>
#include<string.h>

#include"fs.h"

int cp(char* name){
    FILE *fp, *cp_src; /* pliki pamieci i zrodlowy */
    const char OCCUPIED = 0xff;

    FS_Superblock fs_sb;

    fp = fopen(FS_NAME, "r+b");
    if(!fp) {
        printf("No file system existing!\n");
        return NOFSYSTEMEXISTING;
    }
    fread(&fs_sb, sizeof(FS_Superblock), 1, fp); /* czytanie Superblocku */
    fclose(fp);

    char iNodeBitmap[fs_sb.iNodesNum];
    char dataBitmap[fs_sb.dataBlockNum];
    FS_iNode iNodeTable[fs_sb.iNodesNum];

    fp = fopen(FS_NAME, "a+b");
    if(!fp) {
        printf("No file system existing!\n");
        return NOFSYSTEMEXISTING;
    }

    fseek(fp, fs_sb.iNodeOccupancyBitmapOffset*fs_sb.blockSize, SEEK_SET); /* wczywtywanie mapy iNodow */
    fread(iNodeBitmap, sizeof(char), fs_sb.iNodesNum, fp);

    fseek(fp, fs_sb.dataOccupancyBitmapOffset*fs_sb.blockSize, SEEK_SET); /* wczytywanie mapy blockow user data */
    fread(dataBitmap, sizeof(char), fs_sb.dataBlockNum, fp);

    fseek(fp, fs_sb.iNodesOffset*fs_sb.blockSize, SEEK_SET); /* wczytywanie tablicy iNodow */
    fread(iNodeTable, fs_sb.iNodeSize, fs_sb.iNodesNum, fp);

    fclose(fp);

    int maxSize = fs_sb.blockSize*fs_sb.dataBlockNum;
    char sourcebuffer[maxSize];

    int i;
    for(i = 0; i < fs_sb.iNodesNum; i++){ /* sprawdzanie, czy plik o zadanej nazwie juz istnieje */
        if(iNodeBitmap[i] != '\0'){
            if(strcmp(iNodeTable[i].name, name) == 0){
                printf("File already exists!\n");
                return FILEALREADYEXISTS;
            }
        }
    }

    cp_src = fopen(name, "r"); /* wczytywanie pliku */
    if(!fp) {
        printf("Target file do not exist!\n");
        return TARGETFILENOTEXISTING;
    }
    fread(sourcebuffer, maxSize, 1, cp_src);
    int fileSize = strlen(sourcebuffer);
    fclose(cp_src);

    int requiredBlocks = 1 + (fileSize / fs_sb.blockSize); /* wyznaczanie ilosci blokow */

    for(i = 0; i < fs_sb.iNodesNum; i++){
        if(iNodeBitmap[i] == '\0'){
            int j;
            for(j = 0; j < fs_sb.dataBlockNum; j++){
                if(dataBitmap[j] == '\0'){ /* zapiwywanie plikow w znalezionym miejscu */
                    int notAvailable = 0;
                    int k;
                    for(k = 0; k < requiredBlocks; k++){ /* szukanie ciaglej wystarczajacej pamieci */
                        if(dataBitmap[j + k] == '\0'){
                            continue;
                        }
                        notAvailable = 1;
                    }

                    if(notAvailable){ /* ponowna proba w przypadku niepowodzenia (nieodnalezienie wystaeczajacej ilosci blokow) - przejscie dalej */
                        continue;
                    }

                    /* zapisywanie iNoda */
                    iNodeTable[i].startingBlock = fs_sb.dataBlockOffset + j;
                    iNodeTable[i].accessRights = 0x07;
                    iNodeTable[i].size = fileSize;
                    time_t now;
                    time(&now);
                    iNodeTable[i].lastAccessed = now;
                    iNodeTable[i].lastModified = now;
                    strcpy(iNodeTable[i].name, name);
                    /* koniec zapisywania iNoda */

                    fp = fopen(FS_NAME, "r+b");
                    if(!fp) {
                        printf("No file system existing!\n");
                        return NOFSYSTEMEXISTING;
                    }

                    /* blokowanie iNoda */
                    fseek(fp, fs_sb.iNodeOccupancyBitmapOffset*fs_sb.blockSize + i*sizeof(char), SEEK_SET);
                    fwrite(&OCCUPIED, sizeof(char), 1, fp);
                    int z;

                    /* blokowanie pamieci */
                    for(z = 0; z < requiredBlocks; z++){
                        fseek(fp, fs_sb.dataOccupancyBitmapOffset*fs_sb.blockSize + (j+z)*sizeof(char), SEEK_SET);
                        fwrite(&OCCUPIED, sizeof(char), 1, fp);
                    }

                    /* zapisywanie danych iNoda */
                    fseek(fp, fs_sb.iNodesOffset*fs_sb.blockSize + i*fs_sb.iNodeSize, SEEK_SET);
                    fwrite(iNodeTable+i, fs_sb.iNodeSize, 1, fp);

                    /* zapisywanie pliku */
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

