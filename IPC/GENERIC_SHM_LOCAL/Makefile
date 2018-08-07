LL = ./LinkedList/LinkedListApi.o
TREE = ./Trees/tree.o
all:exe
exe:$(LL) $(TREE) main.o shm_ds.o map.o struct.o Utils.o 
	gcc -g $(LL) $(TREE) main.o shm_ds.o map.o struct.o Utils.o -o exe -lrt
$(LL):./LinkedList/LinkedListApi.c
	(cd LinkedList; make)
$(TREE):./Trees/tree.c
	(cd Trees; make)
main.o:main.c
	gcc -g -c -I . main.c -o main.o
shm_ds.o:shm_ds.c
	gcc -g -c -I . shm_ds.c -o shm_ds.o
map.o:map.c
	gcc -g -c -I . map.c -o map.o
struct.o:struct.c
	gcc -g -c -I . struct.c -o struct.o
Utils.o:Utils.c
	gcc -g -c -I . Utils.c -o Utils.o
clean:
	rm *.o
	rm ./LinkedList/*.o
	rm ./Trees/*.o
	rm exe
