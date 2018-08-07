#include "threadApi.h"
#include <stdio.h>
#include <stdlib.h>

#define uint8_t char

_pthread_t*
get_blocked_thread_from_pool(blocked_pool_t *block_pool){
    int i ;
    for (i = block_pool->pool_size -1; i >=0; i--){
        if(block_pool->blocked_thread_collection[i])
            return block_pool->blocked_thread_collection[i];
    }
        return NULL;
}

void
dump_block_pool(blocked_pool_t *block_pool){
    printf("pool size = %d\n", block_pool->pool_size);
    unsigned int i = 0;
    for(;i < block_pool->pool_size; i++){
        if(block_pool->blocked_thread_collection[i]){
            printf("pool[%d] = %d\n", i, block_pool->blocked_thread_collection[i]->selfid);
             dump_thread_DS(block_pool->blocked_thread_collection[i]);
        }
        else 
            printf("block_pool->blocked_thread_collection[%d] = NULL\n", i );
    }
    return;
}

int
is_thread_in_block_pool(unsigned int thid, blocked_pool_t *block_pool){
    int i ;
    for (i = 0; i < block_pool->pool_size; i++)
    {
        if(block_pool->blocked_thread_collection[i] && 
            block_pool->blocked_thread_collection[i]->selfid == thid){
            return i;
        }
    }
    return FAILURE;
}


int
is_thread_in_block_pool_mutex(unsigned int thid, blocked_pool_t *block_pool){
    int i ;
    pthread_mutex_lock(&(block_pool->pool_mutex));
    for (i = 0; i < block_pool->pool_size; i++)
    {
        if(block_pool->blocked_thread_collection[i] && 
            block_pool->blocked_thread_collection[i]->selfid == thid){
            pthread_mutex_unlock(&(block_pool->pool_mutex));
            return i;
        }
    }
    pthread_mutex_unlock(&(block_pool->pool_mutex));
    return FAILURE;
}

void
init_blocked_pool(blocked_pool_t *block_pool, unsigned int pool_size){
    int i = 0;
    block_pool->pool_size = pool_size;
    for (i = 0; i < pool_size; i++)
        block_pool->blocked_thread_collection[i] = NULL;
    pthread_mutex_init(&(block_pool->pool_mutex), NULL);
    return;
}

int
get_empty_slot_from_pool(blocked_pool_t *block_pool){
    int i;
    for(i = block_pool->pool_size -1; i >=0; i--){
        if(block_pool->blocked_thread_collection[i] == NULL)
            return i;
    }
    return FAILURE;
}


int
get_empty_slot_from_pool_mutex(blocked_pool_t *block_pool){
    int i;
    pthread_mutex_lock(&(block_pool->pool_mutex));
    for(i = block_pool->pool_size -1; i >=0; i--){
        if(block_pool->blocked_thread_collection[i] == NULL)
            pthread_mutex_unlock(&(block_pool->pool_mutex));
            return i;
    }
    pthread_mutex_unlock(&(block_pool->pool_mutex));
    return FAILURE;
}

int
remove_thread_from_pool(blocked_pool_t *block_pool, _pthread_t *thread){
    int i = 0, loc = -1;
    if(thread->selfid > (block_pool->pool_size - 1)) return FAILURE;
    pthread_mutex_lock(&(block_pool->pool_mutex));
    if((loc = is_thread_in_block_pool(thread->selfid, block_pool)) > -1)
    {
        block_pool->blocked_thread_collection[loc] = NULL;
        printf("thread %d is removed from gl_blocked_th_pool\n", thread->selfid);
    }
    else
    {
        printf("thread %d already does not exist in gl_blocked_th_pool\n", thread->selfid);
    }
    pthread_mutex_unlock(&(block_pool->pool_mutex));
    return loc;
}

void 
dump_thread_DS(_pthread_t *thread)
{
    printf("===================================\n");
    printf("selfid = %d\n", thread->selfid);
    printf("pthread_handle = %ld\n", thread->pthread_handle);
    printf("isWaiting = %d\n", thread->isWaiting);
    printf("resume_thread_id = %d\n", thread->resume_thread_id);
    printf("attr = 0x%x\n", (unsigned int )&(thread->attr));
    printf("cond = 0x%x\n", (unsigned int )&(thread->cond));
    printf("===================================\n");
    return;
}

static int 
_pool_add(blocked_pool_t *block_pool, _pthread_t *thread)
{
    int loc = -1;
    if(thread->selfid > block_pool->pool_size -1) return -1;
    loc = get_empty_slot_from_pool(block_pool);
    if(loc > -1){
        block_pool->blocked_thread_collection[loc] = thread;
        printf("thread %d is added to gl_blocked_th_pool at index %d\n", thread->selfid, loc);
        return loc;
    }
    else{
        printf("gl_blocked_th_pool is full, thread %d cannot be added\n", thread->selfid);
        return FAILURE;
    }
}

int 
add_thread_to_pool(blocked_pool_t *block_pool , _pthread_t *thread)
{
    if (!block_pool) return -1;
    int rc = SUCCESS;
    pthread_mutex_lock(&block_pool->pool_mutex);

    if(is_thread_in_block_pool(thread->selfid, block_pool) < 0)
    {
        rc = _pool_add(block_pool, thread);
        pthread_mutex_unlock(&block_pool->pool_mutex);
        return rc;
    }
    else
    {
        printf("Thread %d is already in blocked pool\n", thread->selfid);
        pthread_mutex_unlock(&block_pool->pool_mutex);
        return FAILURE;
    }
}

void pthread_init(_pthread_t *_pthread, unsigned int tid, unsigned int JOINABLE)
{
    printf("Initialising thread %d\n", tid);
    _pthread->selfid = tid;
    pthread_attr_init(&_pthread->attr);
    JOINABLE ? pthread_attr_setdetachstate(&_pthread->attr, PTHREAD_CREATE_JOINABLE):
        pthread_attr_setdetachstate(&_pthread->attr, PTHREAD_CREATE_DETACHED);
    pthread_cond_init(&_pthread->cond, NULL);
    _pthread->isWaiting = FALSE;
    _pthread->resume_thread_id = -1;
}

void cleanup_pthread(_pthread_t *thread)
{
    pthread_attr_destroy(&(thread->attr));
    pthread_cond_destroy(&(thread->cond));
    return;
}

void
wait (_pthread_t *thread_to_block, pthread_mutex_t *mutex, unsigned int line_no){
        pthread_mutex_lock(mutex);
        printf("wait called at line no : %d : thread %d is waiting.\n",line_no ,thread_to_block->selfid);
        thread_to_block->isWaiting = TRUE;
        if(pthread_cond_wait (&(thread_to_block->cond), mutex)){
            printf("pthread_cond_wait failed, thread id = %d, line_no = %d", thread_to_block->selfid, line_no);
            thread_to_block->isWaiting = FALSE;
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(mutex);
}

void
signal (_pthread_t *signalling_thread, _pthread_t *signalled_thread, unsigned int line_no){
    printf("signal called at line no : %d : thread %d sending signal to thread %d \n", 
                            line_no, signalling_thread->selfid, signalled_thread->selfid);
    if(pthread_cond_signal(&(signalled_thread->cond))){
        printf("pthread_cond_signal signalling failed, sender_thread id = %d, recvr_thread id = %d, line_no = %d", signalling_thread->selfid, signalled_thread->selfid, line_no);
        pthread_exit(NULL);
    }
    signalled_thread->isWaiting = FALSE;
    signalled_thread->resume_thread_id = signalling_thread->selfid;
}
#if 0
int main(int argc, char **argv){
    return 0;
}
#endif
