/*Alternation among >2 threads, extension of Alternation.c*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define THREAD_NUM 10
#define wait _wait(thid, __LINE__)

pthread_mutex_t mutex;
pthread_cond_t cond[THREAD_NUM];

void _wait(int thid, int line_no)
{
 pthread_mutex_lock(&mutex);
 printf("wait called at line no : %d : thread %d is waiting.\n",line_no ,thid);
 if(pthread_cond_wait (&cond[thid], &mutex)){
	printf("pthread_cond_wait failed, thread id = %d, line_no = %d", thid, line_no);
	pthread_exit(NULL);
}
 pthread_mutex_unlock(&mutex);
}

void signal (int sender_thid, int recvr_thid, int line_no)
{
 pthread_mutex_lock(&mutex);
 printf("signal called at line no : %d : thread %d sending signal to thread %d \n", 
	line_no, sender_thid, recvr_thid);
 if(pthread_cond_signal(&cond[recvr_thid])){
	printf("pthread_cond_signal signalling failed, sender_thread id = %d, recvr_thread id = %d, line_no = %d", sender_thid, recvr_thid, line_no);
	pthread_exit(NULL);
}
 pthread_mutex_unlock(&mutex);
}


 /* ===================================================
	Write your program Logic from Here 
    ================================================== */


void *who_am_i(void *arg)
{
 long thid = (long)arg;
 int i = 0;
 while(++i)
{

		/*if((thid % THREAD_NUM) && execute_once_flag)
		{
			execute_once_flag = 0;
			wait;
		}*/
		printf("i = %d, i am thread %ld\n", i, thid);
		signal(thid, (thid+1) % THREAD_NUM, __LINE__);
		wait;
		//sleep(1);	
	} // while ends

 pthread_exit((void *)thid);
} 


int main(int argc, char *argv[])
{
 int i = 0;
 pthread_t th_id[THREAD_NUM];
 void *ret_val;
/*defining thread attribute*/
 pthread_attr_t attr;
 pthread_attr_init(&attr);
 pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

/*initialising mutexes*/
 for (i = 0;i < THREAD_NUM; i++)
  pthread_mutex_init(&mutex, NULL);	

 /*Initialising condition variables*/
  for (i = 0;i < THREAD_NUM; i++)
   pthread_cond_init(&cond[i], NULL);

/*creating threads*/

   for (i = 0;i < THREAD_NUM; i++){
	pthread_create(&th_id[i], &attr, who_am_i, (void*)i);
}

/* Wait for all threads to complete */
  for (i = 0; i < THREAD_NUM; i++) {
    pthread_join(th_id[i], &ret_val);
    //printf("Main joined by thread %ld\n", long(ret_val));
  }


// clean up

pthread_attr_destroy(&attr);
pthread_mutex_destroy(&mutex);

   for (i = 0; i < THREAD_NUM; i++) {
	pthread_cond_destroy(&cond[i]);
}

 pthread_exit (NULL);

 return 0;
}
