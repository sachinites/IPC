#include "map.h"
#include <stdio.h>
#include "Utils.h"
#include "memory.h"
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "shm_ds.h"

int
add_value_in_physical_memory(char *mmap_key, void *value, struct_id_t struct_id){
	unsigned int size = sizeofstruct(struct_id);
	int shm_fd;
	shm_fd = shm_open(mmap_key, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG);
	if (shm_fd < 0) {
		printf("failure on shm_open on shm_fd");
		return -1;
	}

	if (ftruncate(shm_fd, size) == -1) {
		printf("Error on ftruncate to allocate size of shared memory region\n");
		return -1;
	}

	void *shm_reg = mmap(NULL, // let the kernel chose to retunr the base address of shm memory
                        size,  // sizeof the physical memory to map to virtual address space of the process
                        PROT_READ | PROT_WRITE,// shared memory is Read and Writable
                        MAP_SHARED, // shared memory is accessible by different processes
                        shm_fd, // file descriptor of the memory in physical space to be mapped
                        0); // offset from the base address of the physical memory to be mapped
	memset(shm_reg, 0, size);
	copy(struct_id, value, shm_reg);
	return 0;
}

void*
get_value_from_physical_memory(shm_ds_t *ds, void *node, unsigned int *value_size){
	char *mmap_key = (char *)node + ds->get_node_size();
	*value_size = *(unsigned int *)(mmap_key + MMAP_KEY_SIZE);	
	int shm_fd = 0;
	void *shm_reg = NULL;
	shm_fd = shm_open(mmap_key, O_CREAT | O_RDWR , S_IRWXU | S_IRWXG);
	if (shm_fd < 0) {
		printf("failure on shm_open\n");
		return NULL;
	}

	shm_reg = mmap (NULL, *value_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if(shm_reg == MAP_FAILED){
                printf("Error on mapping\n");
                return NULL;
        }
	return shm_reg;	
}
