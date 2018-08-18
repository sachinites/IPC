gcc -g -c shm_demo.c -o shm_demo.o
gcc -g -c reader.c -o reader.o
gcc -g -c writer.c -o writer.o 
gcc -g reader.o shm_demo.o -o reader -lrt
gcc -g writer.o shm_demo.o -o writer -lrt


