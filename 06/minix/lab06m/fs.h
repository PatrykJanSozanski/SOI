#ifndef __SOI_FS_H__
#define __SOI_FS_H__

#include<time.h>

#define FS_NAME "vsfs"
#define FS_SIZE 8192
#define FS_INODES 20
#define FS_DATA_BLOCKS 14
#define FS_INODE_SIZE 32
#define FS_BLOCK_SIZE 512

int mkfs(void); /* tworzenie nowego wirtualnego systemu plikow */
int ls(void); /* podglad plikow znajdujacych sie w folderze wirtualnego systemu plikow*/
int rm(char* name); /* usuwanie pliku z katalogu wirtualnego systemu plikow */
int cp(char* name); /* kopiowanie pliku z systemu hostujacego do folderu wirtualnego systemu plikow */
int rmfs(void); /* usuwanie starego wirtualnego systemu plikow */
int fsinfo(void); /* wypisywanie informacji o wirtualnym systemie plikow */
int load(char* name); /* kopiowanie pliku z wirtualnego systemu plikow do systemu hostujacego*/

typedef struct FS_Superblock {
    int blockSize;

    int iNodeSize;
    int iNodesNum;
    int iNodesBlockNum;
    int iNodesOffset;

    int dataBlockNum;
    int dataBlockOffset;

    int iNodeOccupancyBitmapOffset;
    int dataOccupancyBitmapOffset;

} FS_Superblock;

typedef struct FS_iNodeOccupancyBitmap {
    char occupied[FS_INODES];
} FS_iNodeOccupancyBitmap;

typedef struct FS_dataOccupancyBitmap {
    char occupied[FS_DATA_BLOCKS];
} FS_dataOccupancyBitmap;

typedef struct FS_iNode {
    int startingBlock;
    char accessRights;
    int size;
    time_t lastAccessed;
    time_t lastModified;
    char name[96];
} FS_iNode;


#define FILEALREADYEXISTS -1
#define NOAVAILABLEINODES -2
#define FSYSALREADYEXISTS -3
#define NOFSYSTEMEXISTING -4
#define COULDNTCREATEFSYS -5
#define INSUFFICIENTARGS -6
#define INCORRECTARGUMENT -7
#define TARGETFILENOTEXISTING -8
#define FILENOTEXISTINGINFS -9

#endif /* __SOI_FS_H__ */

