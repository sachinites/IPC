#include "threadApi.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define PRODUCER 0
#define CONSUMER 1
#define MONITOR 2
#define AVAILABLE 0

#ifdef MAX_POOL_SIZE
#undef MAX_POOL_SIZE
#define MAX_POOL_SIZE 2
#endif

#define RESOURCE_UNITS 3
#define JOINABLE 1

typedef struct resource{
    int slot[RESOURCE_UNITS];
    pthread_mutex_t resrc_mutex;
} resource_t;

resource_t resource;

_pthread_t threads[3];

void *monitor(void *t3){
    int tid;
    printf("Monitor thread started\n");
    while(1){
        printf("Enter the thread number\n");
        scanf("%d\n", &tid);
        dump_thread_DS(&threads[tid]);
        printf("--------------------\n");
    }
}

void *produce(void *t){
    printf("producer started\n");
    int i = 0;
    while(1){
        //sleep(1);
        printf("producer %ld is waiting to lock the resource mutex\n", (long)t);
        pthread_mutex_lock(&resource.resrc_mutex);
        printf("producer locked the resource mutex\n");
        for(i = 0; i < RESOURCE_UNITS; i++){
            if(resource.slot[i] == AVAILABLE){
                printf("filling the slot %d\n", i);
                resource.slot[i] = 1;
                printf("producer sending the signal to consumer\n");
                signal(&threads[PRODUCER], &threads[CONSUMER], __LINE__);
                printf("producer unlocked the resource mutex\n");
                pthread_mutex_unlock(&resource.resrc_mutex);
                break;
            }
            else 
            {
                continue;
            }
        }
        if(i == RESOURCE_UNITS){
            printf("resource is full, producer unlocked the resource mutex\n");
            printf("producer %ld is getting blocked as resource is full\n", (long)t);
            pthread_mutex_unlock(&resource.resrc_mutex);
            wait(&threads[PRODUCER], &resource.resrc_mutex, __LINE__);
            printf("producer thread resumes as it recieves the signal\n");
        }
    } // while ends
    pthread_exit (NULL);
    return NULL;
}

void *consume(void *t){
    printf("consumer started\n");
    int i = 0;
    while(1){
        //sleep(2);
        printf("consumer %ld is waiting to lock the resource mutex\n", (long)t);
        pthread_mutex_lock(&resource.resrc_mutex);
        printf("consumer locked the resource mutex\n");
        for(i = 0; i < RESOURCE_UNITS; i++){
            if(resource.slot[i] != AVAILABLE){
                printf("consuming the slot %d\n", i);
                resource.slot[i] = AVAILABLE;
                printf("consumer sending the signal to producer\n");
                signal(&threads[CONSUMER], &threads[PRODUCER], __LINE__);
                printf("consumer unlocked the resource mutex\n");
                pthread_mutex_unlock(&resource.resrc_mutex);
                break;
        }
        else
        {
            continue;
        }
    }
    if(i == RESOURCE_UNITS){
        printf("resource is empty, consumer unlocked the resource mutex\n");
        printf("consumer %ld is getting blocked as resource is empty\n", (long)t);
        pthread_mutex_unlock(&resource.resrc_mutex);
        wait(&threads[CONSUMER], &resource.resrc_mutex, __LINE__);
        printf("consumer thread resumes as it recieves the signal\n");
    }
    } // while ends
    
    pthread_exit (NULL);
    return NULL;
}

int 
main(int argc, char **argv){
    long t1 = PRODUCER, t2 = CONSUMER,
    t3 = MONITOR;
    int i = 0;

    /*Initialise resource*/

    memset(&resource, 0 , sizeof(resource_t));
    for(i = 0 ; i < RESOURCE_UNITS; i++){
        resource.slot[i] = 0;
    }
    pthread_mutex_init(&resource.resrc_mutex, NULL);

    pthread_init(&threads[PRODUCER], PRODUCER, JOINABLE); // Detached Mode
    pthread_init(&threads[CONSUMER], CONSUMER, JOINABLE); // Detached Mode
    pthread_init(&threads[MONITOR], MONITOR, JOINABLE); // Detached Mode

    /* For portability, explicitly create threads in a joinable state */
    pthread_create(&(threads[PRODUCER].pthread_handle), &threads[PRODUCER].attr, produce, (void *)t1);
    pthread_create(&(threads[CONSUMER].pthread_handle), &threads[CONSUMER].attr, consume, (void *)t2);
    pthread_create(&(threads[MONITOR].pthread_handle), &threads[MONITOR].attr, monitor, (void *)t3);

    /* Wait for all threads to complete */
        pthread_join(threads[PRODUCER].pthread_handle, NULL);
        printf("Producer thread Joined\n");
        pthread_join(threads[CONSUMER].pthread_handle, NULL);
        printf("Consumer thread Joined\n");
        pthread_join(threads[MONITOR].pthread_handle, NULL);
        printf("Monitor thread Joined\n");
    /* Clean up and exit */
    pthread_mutex_destroy(&resource.resrc_mutex);

    cleanup_pthread(&threads[PRODUCER]);
    cleanup_pthread(&threads[CONSUMER]);
    cleanup_pthread(&threads[MONITOR]);
    pthread_exit (NULL);
    return 0;
}
