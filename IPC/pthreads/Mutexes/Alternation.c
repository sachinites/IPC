#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define THREAD_NUM 2
#define wait _wait(thid)

pthread_mutex_t mutex;;
pthread_cond_t cond[THREAD_NUM];

void _wait(int thid)
{
 pthread_mutex_lock(&mutex);
 pthread_cond_wait (&cond[thid], &mutex);
 pthread_mutex_unlock(&mutex);
}

void signal (int thid)
{
 pthread_mutex_lock(&mutex);
 pthread_cond_signal(&cond[thid]);
 pthread_mutex_unlock(&mutex);
}

void *who_am_i(void *arg)
{
 long thid = (long)arg;
 while(1){
 switch (thid)
 {
  case 0:
	{
		printf("I am thread 0\n");
		signal(1);
		wait;	
		break;
	}
 case 1:
	{
		wait;
		printf("I am thread 1\n");
		signal(0);
		break;
	}
	
 }
}
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
