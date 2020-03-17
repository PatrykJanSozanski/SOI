#ifndef __SOI_FS_H__
#define __SOI_FS_H__

#include<time.h>

#define FS_NAME "vsfs" /* nazwa wirtualnego systemu plikow */
#define FS_SIZE 262144 /* rozmiar wirtualnego systemu plikow - 256 kB */
#define FS_INODES 80 /* ilosc iNodow - wiecej niz blokow user data */
#define FS_DATA_BLOCKS 56 /* bloki user data o wiekosci 4kB */
#define FS_INODE_SIZE 128 /* rozmiar iNoda dobrany tak, aby bylo ich wiecej od blokow user data */

int mkfs(void); /* tworzenie nowego wirtualnego systemu plikow */
int ls(void); /* podglad plikow znajdujacych sie w folderze wirtualnego systemu plikow*/
int rm(char* name); /* usuwanie pliku z katalogu wirtualnego systemu plikow */
int cp(char* name); /* kopiowanie pliku z systemu hostujacego do folderu wirtualnego systemu plikow */
int rmfs(void); /* usuwanie starego wirtualnego systemu plikow */
int fsinfo(void); /* wypisywanie informacji o wirtualnym systemie plikow */
int load(char* name); /* kopiowanie pliku z wirtualnego systemu plikow do systemu hostujacego*/

typedef struct FS_Superblock { /*struktura Superblocku */
    int blockSize; /* rozmiar bloku */

    int iNodeSize; /* rozmiar iNodu */
    int iNodesNum; /* liczba iNodow */
    int iNodesBlockNum; /* liczba blokow iNodow */
    int iNodesOffset; /* pozycja ostatniego iNoda */

    int dataBlockNum; /* liczba blokow */
    int dataBlockOffset; /* pozycja ostatniego bloku */

    int iNodeOccupancyBitmapOffset; /* ostatnia pozycja na mapie iNodow */
    int dataOccupancyBitmapOffset; /* ostatnia pozycja na mapie blokow uzytkownika */

} FS_Superblock;

typedef struct FS_iNodeOccupancyBitmap { /* struktura mapy zajetosci iNodow */
    char occupied[FS_INODES]; /* tablica zajetosci iNodow */
} FS_iNodeOccupancyBitmap;

typedef struct FS_dataOccupancyBitmap { /* struktura mapy zajetosci blokow uzytkownika */
    char occupied[FS_DATA_BLOCKS]; /* tablica zajetosci blokow uzytkownika*/
} FS_dataOccupancyBitmap;

typedef struct FS_iNode { /* struktura iNoda */
    int startingBlock; /*  wskaznik pierwszego bloku */
    char accessRights; /* prawa dostepu  */
    int size; /* rozmiar pliku */
    time_t lastAccessed; /* czas ostatniego dostepu */
    time_t lastModified; /*czas ostatniej modyfikacji */
    char name[96]; /* nazwa pliku */
} FS_iNode;

/* rozne definicje bledow */
#define FILEALREADYEXISTS -1
#define NOAVAILABLEINODES -2
#define FSYSALREADYEXISTS -3
#define NOFSYSTEMEXISTING -4
#define COULDNTCREATEFSYS -5
#define INSUFFICIENTARGS -6
#define INCORRECTARGUMENT -7
#define TARGETFILENOTEXISTING -8
#define FILENOTEXISTINGINFS -9

#endif
