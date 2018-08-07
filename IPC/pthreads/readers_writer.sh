rm threadApi.o
gcc -g -c threadApi.c -o threadApi.o -lpthread
gcc -g -c readers_writer.c -o readers_writer.o -lpthread
gcc -g threadApi.o readers_writer.o -o exe -lpthread
