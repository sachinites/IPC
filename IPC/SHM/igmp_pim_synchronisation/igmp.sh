rm igmp.o
rm shm_struct.o
rm igmp
rm pim
gcc -g -c -lpthread shm_struct.c -o shm_struct.o
gcc -g -c -lpthread igmp.c -o igmp.o
gcc -g -c -lpthread pim.c -o pim.o
gcc -g igmp.o shm_struct.o -o igmp -lrt
gcc -g pim.o shm_struct.o -o pim -lrt
