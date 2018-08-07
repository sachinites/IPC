#include "stdio.h"
#include "stdlib.h"


void MasterProcess()
{
 while (1){
 printf("I am master process, pid = %d\n", getpid());
 sleep(1);
}
}

void ChildProcess()
{
 while(1){
 printf("I am Child Process, pid = %d\n", getpid());
 sleep(1);
}
}

int main()
{
 printf("parent process pid = %d\n", getpid());
	switch(fork())
	{
		case 0:
		{
			ChildProcess();
			break;
		}
		default:
			MasterProcess();
}
 return 0;
}
