#include "stdio.h"
#include "stdlib.h"
#define Nkids 3

void MasterProcess(int *mem)
{
 while (1){
 *mem = 0;
 printf("I am master process, pid = %d, mem = %d, address = 0x%x\n", getpid(), *mem, (unsigned int)mem);
}
sleep(2);
}

void ChildProcess(int *mem)
{
 while(1){
 *mem = -11;
 printf("I am Child Process, pid = %d, mem = %d, address = 0x%x\n", getpid(), *mem, (unsigned)mem);
}
sleep(2);
}

int main()
{
 int ret = -1, i = 0;
/* Shared memory between processes*/
 int *mem = (int *)malloc(sizeof(int));
 printf("shared memory created. address = 0x%x\n", (unsigned int)mem);
 *mem = ret;

 for(i =0; i< Nkids; i++){
		ret = fork();
	switch(ret)
	{
		case 0:
		{
			ChildProcess(mem);
			break;
		}
		default:
		{
			if (i < Nkids -1) 
				continue;
			MasterProcess(mem);
		}
}
} // for ends
 return 0;
}
