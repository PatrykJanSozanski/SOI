CC = cc

all: main.o mkfs.o ls.o rm.o cp.o rmfs.o fsinfo.o load.o
	$(CC) main.o mkfs.o ls.o rm.o cp.o rmfs.o fsinfo.o load.o -o fs

main.o: main.c 
	$(CC) -c main.c -o main.o

mkfs.o: mkfs.c 
	$(CC) -c mkfs.c -o mkfs.o

ls.o: ls.c 
	$(CC) -c ls.c -o ls.o

rm.o: rm.c 
	$(CC) -c rm.c -o rm.o

cp.o: cp.c 
	$(CC) -c cp.c -o cp.o

rmfs.o: rmfs.c 
	$(CC) -c rmfs.c -o rmfs.o

fsinfo.o: fsinfo.c 
	$(CC) -c fsinfo.c -o fsinfo.o

load.o: load.c 
	$(CC) -c load.c -o load.o

clean:
	rm *.o fs
