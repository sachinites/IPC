#include "threadApi.h"
#include "./../Trees/tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define ITERALS 5
#define LIMIT_BARRIER 4
#define N_THREADS 10
#define JOINABLE 1

#ifdef MAX_POOL_SIZE
#undef MAX_POOL_SIZE
#define MAX_POOL_SIZE N_THREADS
#endif

typedef struct cs_param{
    int n_concurrent_threads;
    pthread_mutex_t n_concurrent_threads_mutex;
    pthread_mutex_t CS_mutex;
} cs_param_t;

cs_param_t C_S;

_pthread_t threads[N_THREADS];
blocked_pool_t gl_block_pool;

void *CS(void *t){
    long tid = (long)t;
    printf("thread %ld is ready to enter CS\n", tid);
    long iterals;
    _pthread_t *next_thread = NULL;

    do{
#if 0
        printf("thread %ld is going for long sleep\n", tid);
        sleep(10);
        printf("thread %ld wakes up\n", tid);
#endif
        pthread_mutex_lock(&C_S.n_concurrent_threads_mutex);
        if(C_S.n_concurrent_threads < LIMIT_BARRIER)
        { 
            C_S.n_concurrent_threads++;
            printf("thread %ld entering into CS, concurrent_threads = %d\n", tid, C_S.n_concurrent_threads);
            pthread_mutex_unlock(&C_S.n_concurrent_threads_mutex);
            
            /*------------------CS STARTS HERE----------------------------*/
            iterals = (tid * ITERALS) + 1;
            while(iterals){
                printf("thread %ld is in CS, Total thread count in CS = %d\n", tid, C_S.n_concurrent_threads);
                iterals--;
                sleep(1);
            }
            /*------------------CS ENDS HERE----------------------------*/
            pthread_mutex_lock(&C_S.n_concurrent_threads_mutex);
            C_S.n_concurrent_threads--;
            printf("thread %ld is going out of CS, n_concurrent_threads = %d\n", tid, C_S.n_concurrent_threads);
            if(C_S.n_concurrent_threads < LIMIT_BARRIER){
                //printf("printing block pool before thread removal\n");
                //dump_block_pool(&gl_block_pool);
                next_thread = get_blocked_thread_from_pool(&gl_block_pool);
                if(next_thread) printf("thread returned from block pool = %d\n", next_thread->selfid);
                if(next_thread)
                {
                    printf("thread %ld sending signal to the blocked thread %d\n", tid, next_thread->selfid);
                    signal(&threads[tid], next_thread, __LINE__);
                    printf("thread %ld is going for a long sleep\n", tid);
                    //sleep(60);
                }
                 else
                    printf("There is no blobking thread, no signal sent\n");
            }
            pthread_mutex_unlock(&C_S.n_concurrent_threads_mutex);
        }
        else
        {
            pthread_mutex_unlock(&C_S.n_concurrent_threads_mutex);
            printf("thread %ld is blocking itself as number of concurrent threads = %d\n", tid, C_S.n_concurrent_threads);
            if(add_thread_to_pool(&gl_block_pool, &threads[tid]) < 0){
                printf("adding thread to pool failed\n");
                assert(0);
            }

            printf("pool contents\n");
            //dump_block_pool(&gl_block_pool);
            wait(&threads[tid], &C_S.CS_mutex, __LINE__);
            printf("thread %ld is resuming on recieving the signal from thread %d\n", tid, threads[tid].resume_thread_id);
            pthread_mutex_unlock(&C_S.CS_mutex);
            remove_thread_from_pool(&gl_block_pool, &threads[tid]);
            //printf("pool contents\n");
            //dump_block_pool(&gl_block_pool);
          }
    }while(TRUE);
}


int 
main(int argc, char **argv){
    int i = 0;

    /*Initialise CS paramsresource*/
    memset(&C_S, 0, sizeof(cs_param_t));
    pthread_mutex_init(&C_S.n_concurrent_threads_mutex, NULL);
    pthread_mutex_init(&C_S.CS_mutex, NULL);
   
   /*initialise block pool*/

   init_blocked_pool(&gl_block_pool, MAX_POOL_SIZE);

    /* initialising reader threads*/
    for(i = 0 ; i < N_THREADS; i++)
        pthread_init(&threads[i], i, JOINABLE); // JOINABLE Mode
   
    for(i = 0 ; i < N_THREADS; i++)
        pthread_create(&(threads[i].pthread_handle), &threads[i].attr, CS, (void *)i);

    /* Wait for all threads to complete */
    for(i = 0 ; i < N_THREADS; i++){
        pthread_join(threads[i].pthread_handle, NULL);
        printf("thread %d Joined\n", threads[i].selfid);
    }
    /* Clean up and exit */
    pthread_mutex_destroy(&C_S.n_concurrent_threads_mutex);
    pthread_mutex_destroy(&C_S.CS_mutex);

    for(i = 0 ; i < N_THREADS; i++)
        cleanup_pthread(&threads[i]);

    pthread_exit (NULL);
    return 0;
}
