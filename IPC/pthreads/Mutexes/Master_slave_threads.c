#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mutex;
#define THREAD_NUM 7

typedef struct _pthread_t_{
    pthread_t pthread_handle;
    int selfid;             // Thread id
    int isWaiting;         // False if thread is executing, else true when when Waited
    int blocker_thread_id;  // meaningfull only when isWaiting is true, its  a thread id of the thread which has called the WAIT for this thread.
    int resume_thread_id;   // meaningfull only when isWaiting is false, its a thread id of the thread which has called the signal for this thread.
    pthread_attr_t attr;    // attribute for this thread
    pthread_cond_t cond;     // condition variable for this thread.
    int pass_tkt_count;	// +ve value represents the number of SIGNAL accumulated on this thread, whereas -ve value represents the number of WAITs accumulated on this thread.
} _pthread_t;

typedef struct _pool_blocked_threads_{
    int pool_size;
    int blocked_thread_collection[THREAD_NUM];
    pthread_mutex_t pool_mutex;
} blocked_pool_t;

blocked_pool_t gl_blocked_th_pool;

static void dump_block_pool(blocked_pool_t *block_pool)
{
    printf("pool size = %d\n", block_pool->pool_size);
    int i =0;
    for(;i < block_pool->pool_size; i++)
        printf("pool[%d] = %d\n", i, block_pool->blocked_thread_collection[i]);
}


    static 
int am_i_in_block_pool(int thid, blocked_pool_t block_pool)
{
    int i ;
    for (i = 0; i < THREAD_NUM; i++)
    {
        if(block_pool.blocked_thread_collection[i] == thid){
            return i;
        }
    }
    return -1; 
}


    static 
int am_i_in_block_pool_mutex(int thid, blocked_pool_t block_pool)
{
    int i ;
    pthread_mutex_lock(&block_pool.pool_mutex);
    for (i = 0; i < THREAD_NUM; i++)
    {

        if(block_pool.blocked_thread_collection[i] == thid){
            return i;
        }
    }
    pthread_mutex_unlock(&block_pool.pool_mutex);
    return -1; 
}

    static 
void init_blocked_pool(blocked_pool_t *block_pool)
{
    int i = 0;
    block_pool->pool_size = THREAD_NUM;
    for (i = 0; i < THREAD_NUM; i++)
        block_pool->blocked_thread_collection[i] = -1;
    pthread_mutex_init(&block_pool->pool_mutex, NULL); 
    return;
}

    int 
get_empty_slot_from_pool(blocked_pool_t block_pool)
{
    int i;
    for(i = 0; i < block_pool.pool_size; i++){ 
        if(block_pool.blocked_thread_collection[i] == -1)
            return i;
    }
    return -1;

}

    static 
int _pool_add(blocked_pool_t *block_pool ,int thid)
{
    int loc = -1;
    if(thid > THREAD_NUM -1) return -1;
    loc = get_empty_slot_from_pool(*block_pool);
    if(loc > -1){
        block_pool->blocked_thread_collection[loc] = thid;
        printf("thread %d is added to gl_blocked_th_pool at index %d\n", thid, loc);
        return loc;
    }
    else{
        printf("gl_blocked_th_pool is full, thread %d cannot be added\n", thid);
    }
}

    static 
int unblock_thread(blocked_pool_t *block_pool ,int thid)
{
    int i = 0, loc = -1;
    if(thid > THREAD_NUM -1) return loc;
    pthread_mutex_lock(&block_pool->pool_mutex);
    if((loc = am_i_in_block_pool(thid, *block_pool)) > -1)
    {
        block_pool->blocked_thread_collection[loc] = -1;
        printf("thread %d is removed from gl_blocked_th_pool\n", thid);
    }
    else
    {
        printf("thread %d already does not exist in gl_blocked_th_pool\n", thid);
    }
    pthread_mutex_unlock(&block_pool->pool_mutex);
    return loc;
}

int block_thread(blocked_pool_t *block_pool ,int thid)
{
    if (!block_pool) return -1;

    pthread_mutex_lock(&block_pool->pool_mutex);

    if(am_i_in_block_pool(thid, *block_pool) < 0)
    {
        _pool_add(block_pool, thid);
        pthread_mutex_unlock(&block_pool->pool_mutex);
        return 0;
    }
    else
    {
        printf("Thread %d is already in blocked pool\n", thid);
        pthread_mutex_unlock(&block_pool->pool_mutex);
        return -1;
    }

}


void dump_thread_DS(_pthread_t thread)
{
    printf("===================================\n");
    printf("selfid = %d\n", thread.selfid);
    printf("pthread_handle = %ld\n", thread.pthread_handle);
    printf("isWaiting = %d\n", thread.isWaiting);
    printf("blocker_thread_id = %d\n", thread.blocker_thread_id);
    printf("resume_thread_id = %d\n", thread.resume_thread_id);
    printf("attr = 0x%x\n", (unsigned int )&thread.attr);
    printf("cond = 0x%x\n", (unsigned int )&thread.cond);
    printf("pass_tkt_count = %d\n", thread.pass_tkt_count);
    printf("===================================\n");
    return;
}


void wait (_pthread_t *thread_to_block, _pthread_t *blocking_thread, int line_no)
{
    pthread_mutex_lock(&mutex);
    printf("wait called at line no : %d : thread %d is waiting.\n",line_no ,thread_to_block->selfid);
    thread_to_block->isWaiting = 1;
    thread_to_block->blocker_thread_id = blocking_thread->selfid;
    thread_to_block->pass_tkt_count--; 
    if(pthread_cond_wait (&(thread_to_block->cond), &mutex)){
        printf("pthread_cond_wait failed, thread id = %d, line_no = %d", thread_to_block->selfid, line_no);
        thread_to_block->isWaiting = 0;
        thread_to_block->blocker_thread_id = -1;
        pthread_exit(NULL);
    }
    pthread_mutex_unlock(&mutex);
}



void signal (_pthread_t *sender_thread, _pthread_t *recvr_thread, int line_no)
{
    pthread_mutex_lock(&mutex);
    printf("signal called at line no : %d : thread %d sending signal to thread %d \n",
            line_no, sender_thread->selfid, recvr_thread->selfid);
    if(pthread_cond_signal(&(recvr_thread->cond))){
        printf("pthread_cond_signal signalling failed, sender_thread id = %d, recvr_thread id = %d, line_no = %d", sender_thread->selfid, recvr_thread->selfid, line_no);
        pthread_exit(NULL);
    }
    recvr_thread->isWaiting = 0;
    recvr_thread->blocker_thread_id = -1;
    recvr_thread->resume_thread_id = sender_thread->selfid;
    recvr_thread->pass_tkt_count++;
    if (recvr_thread->pass_tkt_count >1)
        printf("Signals accumulated on thread %d\n", recvr_thread->selfid);
    pthread_mutex_unlock(&mutex);
}

void pthread_init(_pthread_t *_pthread, 
        int tid,
        int JOINABLE)
{
    printf("Initialising thread %d\n", tid);
    _pthread->selfid = tid;
    pthread_attr_init(&_pthread->attr);
    JOINABLE ? pthread_attr_setdetachstate(&_pthread->attr, PTHREAD_CREATE_JOINABLE): 
        pthread_attr_setdetachstate(&_pthread->attr, PTHREAD_CREATE_DETACHED);
    pthread_cond_init(&_pthread->cond, NULL);
    _pthread->isWaiting = 0;
    _pthread->blocker_thread_id = -1;
    _pthread->resume_thread_id = -1;
    _pthread->pass_tkt_count = 0;
}

void cleanup_pthread(_pthread_t *thread)
{
    pthread_attr_destroy(&(thread->attr));
    pthread_cond_destroy(&(thread->cond));
    return;
}

/*
   Do Not change anythig above this Line, except setting the value of THREAD_NUM
   ================ Write your program logic here ========================== 
 */


_pthread_t thread[THREAD_NUM];

void *Master_thread(void *arg)
{

    long thid = (long)arg;
    int i = 0, inp_th_id = -1, choice = -1;

    printf("Entering Master thread...\n");
    while(++i)
    {
        printf("Enter thread id which you want to perform operation on (0 - %d)\n", THREAD_NUM -1);
        scanf("%d", &inp_th_id);
        printf("Do you want to \n");
        printf("1. Block thread %d, Or\n", inp_th_id);
        printf("2. Resume thread %d\n", inp_th_id);
        printf("3. Exit\n");
        printf("4. Dump Data Structure of thread %d\n", inp_th_id);
        printf("5. Dump block pool\n");
        printf("Enter Choice 1 - 5\n");
        scanf ("%d", &choice);
        switch (choice)
        {
            case 1:
                {
                    printf("Master thread sending BLOCK/WAIT signal to thread %d\n", inp_th_id);
                    if(thid == inp_th_id){
                        printf("thread %ld is Master thread, it cannot block itself\n", thid);
                        break;
                    }
                    if(block_thread(&gl_blocked_th_pool, inp_th_id)==0){
                        printf("Master thread added thread %d to gl_blocked_th_pool, at index = %d\n",
                                inp_th_id , am_i_in_block_pool_mutex(inp_th_id, gl_blocked_th_pool));
                    }
                    break;
                }
            case 2:
                {
                    printf("Master thread sending RESUME signal to thread %d\n", inp_th_id);
                    if(thid == inp_th_id){
                        printf("thread %ld is Master thread, it cannot Resume itself\n", thid);
                        break;
                    }	
                    unblock_thread(&gl_blocked_th_pool, inp_th_id);
                    signal(&thread[thid], &thread[inp_th_id], __LINE__);
                    break;
                }
            case 3:
                {
                    printf("Exiting program...\n");
                    pthread_exit((void *)thid);
                    break;
                }
            case 4:
                {
                    printf("Dumping DS of thread %d\n", inp_th_id);
                    dump_thread_DS(thread[inp_th_id]); 
                    break;
                }

            case 5:
                {	
                    dump_block_pool(&gl_blocked_th_pool);
                    break;
                }

        } // switch ends


    } // while ends

    pthread_exit((void *)thid);
}

void *Slave_thread(void *arg)
{

    long thid = (long)arg;
    int i = 0;

    FILE *fptr;
    char filename[10], data[30];
    sprintf(filename, "slave_%ld", thid);
    fptr=fopen(filename,"w");
    if(fptr==NULL){
        printf("Error in opening a file!");
        exit(1);
    }

    while(++i)
    {
        if(am_i_in_block_pool_mutex(thid, gl_blocked_th_pool) > -1){
            printf("Thread %ld realised that is has been black listed, hence blocking itself\n", thid);
            wait(&thread[thid], &thread[thid], __LINE__);
        }  
        sprintf(data, "%d: I am thread %d\n",i, (int)thid);
        fprintf(fptr,"%s",data);
        fflush(fptr);
        sleep(1);
    } // while ends

    pthread_exit((void *)thid);
}


int main(int argc, char **argv)
{
    int i = 0;
    void *ret_val;

    /*initialising mutexes*/
    pthread_mutex_init(&mutex, NULL);

    /*Initialising global block thread pool*/
    printf("Initializing gl_blocked_th_pool\n");
    init_blocked_pool(&gl_blocked_th_pool);

    /*initialising Master Thread*/

    pthread_init(&thread[0], 0, 1); // Joinable Mode

    /*initialising Slave Thread*/

    for (i = 1; i < THREAD_NUM; i++) {
        pthread_init(&thread[i], i, 0); // Detached Mode
    }

    /*Creating Master thread*/
    if (pthread_create(&thread[0].pthread_handle, &thread[0].attr, Master_thread, (void*)0))
    {
        printf("Thread initialization for Master thread %d failed, exiting ... \n", i);
        exit(0);
    }
    /*creating Slave threads threads*/
    for (i = 1; i < THREAD_NUM; i++) {
        if (pthread_create(&thread[i].pthread_handle, &thread[i].attr, Slave_thread, (void*)i))
        {
            printf("Thread initialization for Slave thread %d failed, exiting ... \n", i);
            exit(0);
        }
    }

    // Waiting for Master thread to Join
    pthread_join(thread[0].pthread_handle, &ret_val);

    /*IF master thread is finished, cancel all slave threads also*/
    /* Wait for all threads to complete */
    for (i = 1; i < THREAD_NUM; i++) {
        if(pthread_cancel(thread[i].pthread_handle) == 0){
            printf("Thread %d is cancelled\n", thread[i].selfid);
        }
        else
        {
            printf("Cancellation request for thread %d is failed\n", thread[i].selfid);
        }  
    }

    // clean up

    for (i = 0; i < THREAD_NUM; i++) {
        cleanup_pthread(&thread[i]);
    }

    pthread_mutex_destroy(&mutex);
    pthread_exit (NULL);

    return 0;
}
