rm threadApi.o
rm ./../Stack/stack.o
rm ./../Trees/tree.o
rm barrier.o
rm exe
gcc -g -c ./../Stack/stack.c -o ./../Stack/stack.o
gcc -g -c ./../Trees/tree.c -o ./../Trees/tree.o
gcc -g -c threadApi.c -o threadApi.o -lpthread
gcc -g -c barrier.c -o barrier.o -lpthread
gcc -g threadApi.o barrier.o ./../Trees/tree.o ./../Stack/stack.o -o exe -lpthread
