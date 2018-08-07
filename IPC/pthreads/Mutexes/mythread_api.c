#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mutex;

typedef struct _pthread_t_{
        pthread_t pthread_handle;
        int selfid;             // Thread id
        int isWaiting;         // False if thread is executing, else true when when Waited
        int blocker_thread_id;  // meaningfull only when isWaiting is true, its  a thread id of the thread which has called the WAIT for this thread.
        int resume_thread_id;   // meaningfull only when isWaiting is false, its a thread id of the thread which has called the signal for this thread.
        pthread_attr_t attr;    // attribute for this thread
        pthread_cond_t cond;     // condition variable for this thread.
} _pthread_t;


void wait(int thid, pthread_cond_t cond_current_thread, int line_no)
{
 pthread_mutex_lock(&mutex);
 printf("wait called at line no : %d : thread %d is waiting.\n",line_no ,thid);
 if(pthread_cond_wait (&cond_current_thread, &mutex)){
        printf("pthread_cond_wait failed, thread id = %d, line_no = %d", thid, line_no);
        pthread_exit(NULL);
}
 pthread_mutex_unlock(&mutex);
}

void signal (int sender_thid, int recvr_thid, pthread_cond_t cond_sig_recver_thread, int line_no)
{
 pthread_mutex_lock(&mutex);
 printf("signal called at line no : %d : thread %d sending signal to thread %d \n",
        line_no, sender_thid, recvr_thid);
 if(pthread_cond_signal(&cond_sig_recver_thread)){
        printf("pthread_cond_signal signalling failed, sender_thread id = %d, recvr_thread id = %d, line_no = %d", sender_thid, recvr_thid, line_no);
        pthread_exit(NULL);
}
 pthread_mutex_unlock(&mutex);
}

void pthread_init(_pthread_t _pthread, 
			 int JOINABLE)
{
 pthread_attr_init(&_pthread.attr);
 JOINABLE ? pthread_attr_setdetachstate(&_pthread.attr, PTHREAD_CREATE_JOINABLE): 
	    pthread_attr_setdetachstate(&_pthread.attr, PTHREAD_CREATE_DETACHED);
 pthread_cond_init(&_pthread.cond, NULL);
 _pthread.isWaiting = 0;
 _pthread.blocker_thread_id = -1;
 _pthread.resume_thread_id = -1;
/* if (pthread_create(&_pthread.pthread_handle, &_pthread.attr, fn_ptr, (void*)thread_id))
 {
	printf("Thread initialization for thread %d failed, exiting ... \n", thread_id);
	exit(0);
 }*/
}

int main(int argc, char **argv)
{
 return 0;
}
