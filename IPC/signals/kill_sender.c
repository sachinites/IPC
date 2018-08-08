#include <stdio.h>
#include <signal.h>

int
main(int argc, char **argv){


    kill(5939, SIGUSR1);
    scanf("\n");
    return 0;
}
