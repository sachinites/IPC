#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shm_struct.h"
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>  /* For mode constants */
#include <fcntl.h>  /* For O_* constants */
#include <sys/shm.h>
#include <arpa/inet.h> // for ntop and pton
#include <signal.h>
int 
main(int argc, char **argv){

    char ip_add[16];
    shm_region_t *shm_reg = NULL;
    struct in_addr int_ipv4;
    // create a shared memory object
    int shm_fd, N;  // file descriptor of shared memory
    shm_fd = shm_open(SHM_NAME,   // name of shared memory, named shared memory
    /*
                    create the shared memory in read/write mode and truncate it to zero
                    bytes if shm region already exists wuth this name. These permissions should match to that of mmap
                    call*/
                    O_CREAT | O_RDWR | O_TRUNC,  
                    S_IRWXU | S_IRWXG);

    if (shm_fd < 0) {
        printf("failure on shm_open on shm_fd");
        exit(0);
    }

    /* Allocate the size of the shared memory region. This space will be allocated in physical memory and not in virtual
     * address space of the process */

    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        printf("Error on ftruncate to allocate size of shared memory region\n");
        exit(0);
    }

    /* Map the shared memory region to virtual address space of this process for accessing the SHM region by this
     * process*/

    shm_reg = (shm_region_t *)mmap(NULL, // let the kernel chose to retunr the base address of shm memory
                                   SHM_SIZE, // sizeof the physical memory to map to virtual address space of the process
                                   PROT_READ | PROT_WRITE,// shared memory is Read and Writable 
                                   MAP_SHARED, // shared memory is accessible by different processes
                                   shm_fd, // file descriptor of the memory in physical space to be mapped
                                   0); // offset from the base address of the physical memory to be mapped

     if(shm_reg == MAP_FAILED){
            printf("Error on mapping\n");
            exit(0);
     }

    /* initialising the protection constructs of shared memory*/
    pthread_mutexattr_setpshared(&shm_reg->shm_mutexAttr, PTHREAD_PROCESS_SHARED); // this mutex can be locked/unlocked by diff process in the system
    pthread_mutex_init(&shm_reg->shm_mutex, &shm_reg->shm_mutexAttr);


    printf("Starting IGMP process ...\n");
    while(1){
        printf("Enter roll no \n");
        scanf("%d", &N);
         printf("input value = %d\n", N);
         if(N > 0)
            shm_write((void *)shm_reg, N);
        else
        {
            if(munmap(shm_reg, SHM_SIZE) != 0){
                printf("Memory unmap failed, errno = %d\n", errno);
            }
            else
                printf("unmap succeeds\n");

/* Good practice that it should be called by the process which created the shm. shm_unlink destorys the shm memory from
 * kernel. IF any other process has a mapping on it and shm_unlink is called, SHM_NAME is removed from kernel, however
 * memory will continue to reside. Kernel will cleanup the memory untill last refernece to shm ceases.
 */

            if(shm_unlink(SHM_NAME) != 0){
                printf("Memory unlink failed, errno = %d\n", errno);
                exit(0);
            }
            else
            {
                printf("unlink succeeds\n");
                break;
                }
        }
    }
    return 0;
}
