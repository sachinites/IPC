#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>

/* mmap() has multiple purpose, this is the example where program simply
do dynamic memory allocation using mmap() instead of malloc() call. There
is no external data source or memory mapping involved. This example simply
shows how mmap() can be used as substitution for malloc()/free().
*/

#if 0
void *
mmap(void *addr, size_t length, int prot, int flags,
                  int fd, off_t offset);

int munmap(void *addr, size_t length);
#endif

int 
main(int argc, char **argv) {

int N=5;
int *ptr = mmap ( NULL,     /*   Le the OS choose the starting virtual address of the memory, just
                                                    like you do not have a control as to what address malloc() will return */
                            N*sizeof(int),      /* Size of memory in bytes being requested for allocation */ 
                             PROT_READ |  /* Memory is Readable */
                             PROT_WRITE, /* Memory is Writable */
                             MAP_PRIVATE |  /* This memory is not sharable with any other process, use MAP_SHARED instead */
                             MAP_ANONYMOUS,   /* This memory is not mapped to extern data source, but only RAM by default */
                             0,     /* FD 0, since no external data source is specified */
                             0 );  /* offset value as zero, since no extern data source is specified */

if(ptr == MAP_FAILED){
    printf("Mapping Failed, errno = %d\n", errno);
    return 1;
}

for(int i=0; i<N; i++)
    ptr[i] = i*10;

for(int i=0; i<N; i++)
    printf("[%d] ",ptr[i]);

printf("\n");
int err = munmap(ptr, 10*sizeof(int));
if(err != 0){
    printf("UnMapping Failed\n");
    return 1;
}

return 0;
}