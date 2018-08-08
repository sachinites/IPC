#include <stdio.h>
#include <signal.h>

static void
signal_handler(int sig){

    printf("Signal %d recieved\n", sig);
} 

int
main(int argc, char **argv){


    signal(SIGUSR1, signal_handler);
    scanf("\n");
    return 0;
}
