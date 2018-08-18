#include <stdio.h>
#include <memory.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

int
create_and_write_shared_memory (char *mmap_key, char *value, unsigned int size){
    
    int shm_fd;
    
    /*Create a shared memory object in kernel space. If shared memory already
     * exists it will truncate it to zero bytes again*/
    shm_fd = shm_open(mmap_key, O_CREAT | O_RDWR | O_TRUNC, 0660);
    
    if (shm_fd < 0) {
        printf("failure on shm_open on shm_fd, errcode = %d\n", errno);
        return -1;
    }

    if (ftruncate(shm_fd, size) == -1) {
        printf("Error on ftruncate to allocate size of shared memory region\n");
        return -1;
    }

    /*Now map the shared memory in kernel space into process's Virtual address space*/
    void *shm_reg = mmap(NULL, // let the kernel chose to return the base address of shm memory
            size,              // sizeof the shared memory to map to virtual address space of the process
            PROT_READ | PROT_WRITE, // shared memory is Read and Writable
            MAP_SHARED,        // shared memory is accessible by different processes
            shm_fd,            // file descriptor of the shared memory 
            0);                // offset from the base address of the physical/shared memory to be mapped

    /* shm_reg is the address in process's Virtual address space, just like any other address.
     * The Linux paging mechanism maps this address to starting address of the shared memory region 
     * in kernel space. Any operation performed by process on shm_reg address is actually the operation
     * performed in shared memory which resides in kernel*/
    memset(shm_reg, 0, size);
    memcpy(shm_reg, value, size);
    munmap(shm_reg, size);
    /*Reader process will not be able to read shm if writer unlink
     * the name below*/
    //shm_unlink(mmap_key);
    close(shm_fd);
    return size;
}

int
read_from_shared_memory (char *mmap_key, 
                         char *buffer, 
                         unsigned int buff_size, 
                         unsigned int bytes_to_read) {


    int shm_fd = 0, rc = 0;

    shm_fd = shm_open(mmap_key, O_CREAT | O_RDONLY , 0660);
    
    if (shm_fd < 0) {
        printf("failure on shm_open on shm_fd, error code = %d", errno);
        return -1;
    }

    void *shm_reg = mmap (NULL, bytes_to_read, PROT_READ, MAP_SHARED, shm_fd, 0);

    if(shm_reg == MAP_FAILED){
        printf("Error on mapping\n");
        return -1;
    }
    
    memcpy(buffer, shm_reg, bytes_to_read);
    rc = munmap(shm_reg, bytes_to_read);

    if(rc < 0){
        printf("munmap failed\n");
        return -1;
    }
    close(shm_fd);
    return bytes_to_read; /*Return the number of bytes read */
}
