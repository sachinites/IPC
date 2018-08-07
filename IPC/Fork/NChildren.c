#include "stdio.h"
#include "stdlib.h"
#define Nkids 3

void MasterProcess()
{
 while (1){
 printf("I am master process, pid = %d\n", getpid());
 sleep(2);
}
}

void ChildProcess()
{
 while(1){
 printf("I am Child Process, pid = %d\n", getpid());
 sleep(2);
}
}

int main()
{
 int ret = -1, i = 0;
 printf("parent process pid = %d\n", getpid());
 for(i =0; i< Nkids; i++){
	ret = fork();
	switch(ret)
	{
		case 0:
		{
			printf("Forking process %d\n",i);
			ChildProcess();
			break;
		}
		default:
		{
			printf("inside default with i = %d\n", i);
			if (i < Nkids -1) 
				continue;
			MasterProcess();
		}
}
} // for ends
 return 0;
}
