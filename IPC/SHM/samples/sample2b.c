/* Sample 2b */
/* This program reads a number from stdin, inserts it into a shared
 * array, and sends SIGUSR1 to another process which monitors the shared
 * array
 */
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int main (int argc, char* argv[])
{
   int seg_id, k;
   double *data, value;
   struct shmid_ds seg_info;
   int pid;
   printf ("Enter the PID of the shared memory monitor> ");
   scanf ("%d", &pid);
   /* Only the key is needed to access a previously allocated segment */
   seg_id = shmget (0xBEEF, 0, 0);
   if (seg_id == -1) {
       perror ("Fail to create shared memory");
       exit (1);
   }
   printf ("Shared segment id is %d\n", seg_id);
   /* attach the segment */
   data = shmat (seg_id, NULL, 0);
   k = 0;

   printf ("Enter value to insert [negative to quit]: ");
   scanf ("%lf", &value);
   while (value > 0)
   {
       data[k] = value;
       kill (pid, SIGUSR1); /* notify the memory monitor of new data */
       k++;
       if (k == 5) k = 0;
       printf ("Next value [negative to quit]: ");
       scanf ("%lf", &value);
   }

   kill (pid, SIGUSR2); /* notify the memory monitor to terminate */

   /* detach the segment */
   shmdt (data);
   return 0;
}
