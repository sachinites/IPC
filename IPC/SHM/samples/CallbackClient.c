/*This is the client program which will register its callbacks with the server*/

#include <sys/shm.h>
#include <sys/stat.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define PROCESS_ID_MEMORY_LOCATION_KEY 223 // client will fetch process from shared memory whose key is 223
#define SERVER_PROCESS_CALL_BUFFER_ID 224 // Client will place its callbacks at the memory location whose key is 224

typedef void (*fn)(void);
void sum()
{
 int a = 10, b = 20;
 printf("sum of %d and %d is %d\n", a,b,a+b);
 for (a = -5;a<5;a++){
 printf("a = %d\n",a);
	b/a;
}
// assert(0);
 return;
}

int main(int argc, char **argv)
{
 int seg_id_of_server_process_pid, callback_segment_id, halt;
 int *server_process_id = NULL;
 fn *callback_buffer  = NULL  , fn_addr = NULL;
  /* Only the key is needed to access a previously allocated segment */
 seg_id_of_server_process_pid = shmget (PROCESS_ID_MEMORY_LOCATION_KEY, 0, 0); 

   if (seg_id_of_server_process_pid == -1) {
       perror ("fail to get access to  shared memory containing process id");
       exit (1);
   }

 /* attach the segment */
   server_process_id = shmat (seg_id_of_server_process_pid, NULL, 0);
   printf("process id  buffer address on client side = 0x%x\n", (unsigned int)&server_process_id);
   printf("Server process id fetched by the client is %d\n", *server_process_id);

   callback_segment_id = shmget (SERVER_PROCESS_CALL_BUFFER_ID, 0,0);
 
   if (callback_segment_id == -1) {
       perror ("fail to get access to  shared memory buffer");
       exit (1);
   }
  
   callback_buffer = shmat(callback_segment_id, NULL,0);
   printf("callback buffer address on client side = 0x%x\n", (unsigned int)callback_buffer); 
   fn_addr = &sum;

   //memcpy(&callback_buffer, &fn_addr, sizeof(fn));
   *callback_buffer = &sum;
   printf("contents of the callback buffer after memcpy 0x%x\n", (unsigned int)*callback_buffer);
   (*callback_buffer)();

   printf("Client has put the function pointer address = 0x%x to the buffer, sending signal to Server\n", (unsigned int)*callback_buffer);
    kill (*server_process_id, SIGUSR1);
   
 printf("halting the client \n");
 scanf("\n");
 return 0;
}
