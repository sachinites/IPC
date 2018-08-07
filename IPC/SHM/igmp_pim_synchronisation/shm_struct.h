#include <pthread.h>
#define SHM_NAME "/ip_addr_list"
#define N_IPADDRESS 10

typedef struct {
        unsigned int ip_address[N_IPADDRESS];
            pthread_mutex_t shm_mutex; // for mutual exclusion
            pthread_mutexattr_t shm_mutexAttr;
} shm_region_t;

#define SHM_SIZE (sizeof(shm_region_t))
int shm_write(void *shm_region, unsigned int data);
void dump_shm_region(void *shm_region);
