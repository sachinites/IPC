#include <pthread.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef SUCCESS
#define SUCCESS 0
#endif

#ifndef FAILURE
#define FAILURE -1
#endif

#define MAX_POOL_SIZE 100

typedef struct _pthread_t_{
    pthread_t pthread_handle;
    unsigned int selfid;             
    char isWaiting;         
    unsigned int resume_thread_id; 
    pthread_attr_t attr;   
    pthread_cond_t cond;   
} _pthread_t;

typedef struct _pool_blocked_threads_{
     unsigned int pool_size;
     _pthread_t* blocked_thread_collection[MAX_POOL_SIZE];
     pthread_mutex_t pool_mutex;
} blocked_pool_t;

void 
dump_block_pool(blocked_pool_t *block_pool);

int 
is_thread_in_block_pool(unsigned int thid, blocked_pool_t *block_pool);


int 
is_thread_in_block_pool_mutex(unsigned int thid, blocked_pool_t *block_pool);

void 
init_blocked_pool(blocked_pool_t *block_pool, unsigned int pool_size);

int
get_empty_slot_from_pool(blocked_pool_t *block_pool);

int
get_empty_slot_from_pool_mutex(blocked_pool_t *block_pool);

_pthread_t*
get_blocked_thread_from_pool(blocked_pool_t *block_pool);

int 
remove_thread_from_pool(blocked_pool_t *block_pool , _pthread_t *thread);

int
add_thread_to_pool(blocked_pool_t *block_pool, _pthread_t *thread);

void 
dump_thread_DS(_pthread_t *thread);

void 
wait (_pthread_t *thread_to_block, pthread_mutex_t *mutex, unsigned int line_no);

void 
signal (_pthread_t *signalling_thread, _pthread_t *signalled_thread, unsigned int line_no);

void 
pthread_init(_pthread_t *_pthread, unsigned int tid, unsigned int JOINABLE);

void 
cleanup_pthread(_pthread_t *thread);
