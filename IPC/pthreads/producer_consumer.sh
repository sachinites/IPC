rm threadApi.o
gcc -g -c threadApi.c -o threadApi.o -lpthread
gcc -g -c producer_consumer.c -o producer_consumer.o -lpthread
gcc -g threadApi.o producer_consumer.o -o exe -lpthread
