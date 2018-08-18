#include <signal.h>
#include <stdio.h>

void signal_catchfunc(int);

int main () {
    int ret;

    ret = signal(SIGINT, signal_catchfunc);

    if( ret == SIG_ERR) {
        printf("Error: unable to set signal handler.\n");
        exit(0);
    }
    printf("Going to raise a signal\n");
    ret = raise(SIGINT);

    if( ret !=0 ) {
        printf("Error: unable to raise SIGINT signal.\n");
        exit(0);
    }

    printf("Exiting...\n");
    return(0);
}

void signal_catchfunc(int signal) {
    printf("!! signal caught !!\n");
}
