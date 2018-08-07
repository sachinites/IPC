/*This program is a server which register the callbacks from its client, and call them later*/

#include <sys/shm.h>
#include <sys/stat.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
/*Its a key of the shared memory where process keeps its own process id
All pther process know this key*/
#define SERVER_PROCESS_ID_MEMORY_LOCATION_KEY 223 
#define SERVER_PROCESS_CALL_BUFFER_ID 224
typedef void (*fn)(void);
void signal_handler (int); // signal handlers are of this fixed type, argumement is a signal which is recieved by this process

fn *callback_buffer = NULL;

int main(int argc, char **argv)
{
 int process_id_mem_loc_id;
 int callback_segment_id; // this is the id if the segment where funtion pointers will be fethed by the server
 int halt;
 struct sigaction act; // what action to perform when this process recieves a signal

/* Creating a shared memory of size int, this memory will keep the current process id.
Any other client process whoever wants to send signal to this process can fetch process id
from this shared memory location.*/
 process_id_mem_loc_id = shmget (SERVER_PROCESS_ID_MEMORY_LOCATION_KEY, 
				sizeof(int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
 if(process_id_mem_loc_id == -1){
	perror ("Fail to create PROCESS_ID Memory location");
	exit(-1);
}

 /* attach the segment */

 int *pid = shmat (process_id_mem_loc_id, NULL, 0);
 if(pid == (int *)-1){
	perror ("Fail to attach pid memory location");
	exit(-1);
}

printf("process id  buffer address on server side = 0x%x\n", (unsigned int)&pid);

 /* put current process pid in shared memory location*/
 *pid = getpid();

 callback_segment_id = shmget(SERVER_PROCESS_CALL_BUFFER_ID, sizeof(fn), 
				IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

 if(callback_segment_id  == -1){
	perror ("Fail to create PROCESS Callback buffer");
	exit(-1);
}

 callback_buffer = shmat(callback_segment_id, NULL, 0);
 /*initialising the memory segment*/
 memset((void *)callback_buffer, 0, sizeof(fn));
 
 if(callback_buffer == (fn *)-1){
	perror ("Fail to attach process callback buffer");
	exit(-1);
}

printf("Server has created shared memory region, one to kepp its own process id, and other to recieve function callbacks at address 0x%x\n", (unsigned int)callback_buffer);

act.sa_handler = signal_handler; // execute this functin when you recieve a signal
sigemptyset (&act.sa_mask);
act.sa_flags = 0;
sigaction (SIGUSR1, &act, NULL);// registering the fn with type of signal

 /* make server to wait*/
 scanf("%d", &halt);
 
printf("Server removing the process id memory segment\n");
shmdt(pid);
shmctl (process_id_mem_loc_id, IPC_RMID, 0); // removing the segment 


printf("Server removing the process callback memory segment\n");
shmdt(callback_buffer);
shmctl (callback_segment_id, IPC_RMID, 0); // removing the segment 

return 0;
}

void signal_handler(int signaltype)
{
 if(signaltype == SIGUSR1){
	printf("Server has recieved a signal\n");
 printf("Server is fetching the callback from buffer\n");
 printf("Address fetched from the buffer = 0x%x\n", (unsigned int)*callback_buffer);
 printf("Server making a callback now\n");
 (*callback_buffer)();
}
}

