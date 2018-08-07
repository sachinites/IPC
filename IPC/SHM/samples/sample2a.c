/* Sample 2a */
/* This program creates an array in a shared memory and (in a loop) prints
 * the array upon receiving SIGUSR1. It finally exits the loop on SIGUSR2.
 * The shared array is updated by another process
 */

#include <sys/shm.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define LEN 5

/* prototype */
void sig_handler (int);

/* global var */
int keep_running = 1;

int main (int argc, char* argv[])
{
   int seg_id, k;
   double *data;
   struct shmid_ds seg_info;
   struct sigaction act;

   act.sa_handler = sig_handler;
   sigemptyset (&act.sa_mask);
   act.sa_flags = 0;
   sigaction (SIGUSR1, &act, NULL);
   sigaction (SIGUSR2, &act, NULL);

   printf ("Shared memory monitor is process %d\n", getpid());

   /* allocate an ARRAY of doubles in a shared segment */
   seg_id = shmget (0xBEEF, LEN * sizeof(double), 
         IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
   if (seg_id == -1) {
       perror ("Fail to create shared memory");
       exit (1);
   }

   /* attach the segment */
   data = shmat (seg_id, NULL, 0);
   if (data == (double *)-1) {
       perror ("Fail to attach");
       exit (1);
   }

   while (keep_running)
   {
       pause();
       printf ("Waken up by signal\n\t");
       for (k = 0; k < LEN; k++)
           printf ("%7.2f ", data[k]);
       printf ("\n");
   }
   printf ("Detaching....\n");

   /* detach the segment */
   shmdt (data);

   /* normally we want to remove the segment, but for this experiment we
    * don't */
   shmctl (seg_id, IPC_RMID, 0);
   return 0;
}
void sig_handler (int signum)
{
    if (signum == SIGUSR2)
        keep_running = 0;
}
