#include "shm_struct.h"
#include <stdio.h>

int shm_write(void *shm_reg, unsigned int data){
    int i = 0;
    shm_region_t *shm_region = shm_reg;
    pthread_mutex_lock(&shm_region->shm_mutex);
    for(i = 0 ; i < N_IPADDRESS ; i++){
        if(shm_region->ip_address[i] != 0)
            continue;
        shm_region->ip_address[i] = data;
        printf("data written at index = %d\n", i);
        pthread_mutex_unlock(&shm_region->shm_mutex);
        return 1;
    }
    printf("Shared memory full\n");
    pthread_mutex_unlock(&shm_region->shm_mutex);
    return 0;
}

void dump_shm_region(void *shm_reg){
    int i = 0;
    shm_region_t *shm_region = shm_reg;
    pthread_mutex_lock(&shm_region->shm_mutex);
    for(i = 0 ; i < N_IPADDRESS ; i++){
        printf("data at index = %d = %d\n", i , shm_region->ip_address[i]);
    }
    pthread_mutex_unlock(&shm_region->shm_mutex);
    return;
}
