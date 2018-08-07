#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mutex;

typedef struct _pthread_t{
	pthread_t;
	int selfid;		// Thread id
	bool isWaiting;		// False if thread is executing, else true when when Waited 
	int blocker_thread_id;  // meaningfull only when isWaiting is true, its  a thread id of the thread which has called the WAIT for this thread.
	int resume_thread_id;   // meaningfull only when isWaiting is false, its a thread id of the thread which has called the signal for this thread.
	pthread_attr_t attr;    // attribute for this thread
	pthread_cond_t cond	// condition variable for this thread.
} _pthread_t;


void pthread_init(_pthread_t pthread);
void wait(int thid, pthread_cond_t cond, int line_no);
void signal (int sender_thid, int recvr_thid, pthread_cond_t cond, int line_no);
