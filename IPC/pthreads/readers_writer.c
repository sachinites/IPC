#include "threadApi.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#define NREADERS 3
#define NWRITERS 1

#define RESOURCE_UNITS 1
#define JOINABLE 1
#define ITERALS 5

typedef struct resource{
    char *slot[RESOURCE_UNITS];
    pthread_mutex_t resrc_mutex;
    int Nreader_count; // Number of readers
    pthread_mutex_t Nreader_count_mutex; // mutex for Nreader_count variable as it is accessed by reader(s) and writer
} resource_t;

resource_t resource;

_pthread_t reader_threads[NREADERS];
_pthread_t writer_threads[NWRITERS];


void *write(void *t){
    long tid = (long)t;
    long iterals;
    printf("writer thread %ld started\n", tid );
    do{
    //pthread_mutex_lock(&resource.Nreader_count_mutex);
    iterals = (tid * ITERALS) + 1;
    if(resource.Nreader_count > 0){
        printf("writer thread %ld detects that %d Readers are reading\nwriter thread blocking itself\n", tid, resource.Nreader_count);
        //pthread_mutex_unlock(&resource.Nreader_count_mutex);
        wait(&writer_threads[0], &resource.resrc_mutex, __LINE__);
    }
    else
    {
        printf("writer thread %ld detects that %d Readers are reading\nwriter enters the writing mode\n", tid, resource.Nreader_count);
        pthread_mutex_lock(&resource.resrc_mutex);
        printf("mutex locked by writer thread %ld\n", tid);
        while(iterals){
            printf("writer thread %ld writing\n", tid);
            iterals--;
            sleep(1);
            }
        printf("writer thread %ld leaves writing\n", tid);
        pthread_mutex_unlock(&resource.resrc_mutex);
        printf("mutex unlocked by writer thread %ld\n", tid);
        //pthread_mutex_unlock(&resource.Nreader_count_mutex);
    }
        sleep(1);
    } while(FALSE);
    pthread_exit (NULL);
    return NULL;
}

void *read(void *t){
    long tid = (long)t;
    long iterals;
    /* Here reader thread just lock and unlock the mutex to ensure mutual exclusion wrt to writer thread only*/
    do
    {
    iterals = (tid * ITERALS) + 1;
    pthread_mutex_lock(&resource.resrc_mutex);
    printf("reader thread %ld enters reading\n", tid);
    pthread_mutex_unlock(&resource.resrc_mutex);
    pthread_mutex_lock(&resource.Nreader_count_mutex);
    resource.Nreader_count++;
    pthread_mutex_unlock(&resource.Nreader_count_mutex);
    while(iterals){
        printf("reader thread %ld reads %s, Number of Active readers = %d\n", tid, resource.slot[0], resource.Nreader_count);
        sleep(1);
        iterals--;
    }
    pthread_mutex_lock(&resource.Nreader_count_mutex);
    resource.Nreader_count--;
    pthread_mutex_unlock(&resource.Nreader_count_mutex);
    printf("reader thread %ld leaves the reading\n", tid);
    pthread_mutex_lock(&resource.Nreader_count_mutex);
    if(resource.Nreader_count == 0){
        printf("Nreader_count = %d, Signal is being sent to writer thread\n", resource.Nreader_count);
        signal(&reader_threads[tid], &writer_threads[0], __LINE__);
    }
    pthread_mutex_unlock(&resource.Nreader_count_mutex);
    sleep(1);
    }while(FALSE);
    pthread_exit (NULL);
    return NULL;
}

int 
main(int argc, char **argv){
    int i = 0;

    /*Initialise resource*/

    memset(&resource, 0 , sizeof(resource_t));
    for(i = 0 ; i < RESOURCE_UNITS; i++){
        resource.slot[i] = "Initialised state";
    }
    pthread_mutex_init(&resource.resrc_mutex, NULL);
    pthread_mutex_init(&resource.Nreader_count_mutex, NULL);
    resource.Nreader_count = 0;
    /* initialising reader threads*/
    for(i = 0 ; i < NREADERS; i++)
        pthread_init(&reader_threads[i], i, JOINABLE); // JOINABLE Mode
   
    for(i = 0 ; i < NWRITERS; i++)
        pthread_init(&writer_threads[i], NREADERS+i, JOINABLE); // JOINABLE Mode


    /* For portability, explicitly create threads in a joinable state */
    for(i = 0 ; i < NREADERS; i++)
        pthread_create(&(reader_threads[i].pthread_handle), &reader_threads[i].attr, read, (void *)i);


    for(i = 0 ; i < NWRITERS; i++)
        pthread_create(&(writer_threads[i].pthread_handle), &writer_threads[i].attr, write, (void *)NREADERS+i);

    /* Wait for all threads to complete */
    for(i = 0 ; i < NREADERS; i++){
        pthread_join(reader_threads[i].pthread_handle, NULL);
        printf("Reader thread %d Joined\n", reader_threads[i].selfid);
    }
    for(i = 0 ; i < NWRITERS; i++){
        pthread_join(writer_threads[i].pthread_handle, NULL);
        printf("writer thread %d Joined\n", writer_threads[i].selfid);
    }
    /* Clean up and exit */
    pthread_mutex_destroy(&resource.resrc_mutex);
    pthread_mutex_destroy(&resource.Nreader_count_mutex);

    for(i = 0 ; i < NREADERS; i++)
        cleanup_pthread(&reader_threads[i]);

    for(i = 0 ; i < NWRITERS; i++)
        cleanup_pthread(&writer_threads[i]);

    pthread_exit (NULL);
    return 0;
}
