#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shm_struct.h"
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>  /* For mode constants */
#include <fcntl.h>  /* For O_* constants */
#include <sys/shm.h>
#include <arpa/inet.h> // for ntop and pton
#include <signal.h>
void signal_handler (int);

shm_region_t *shm_reg = NULL;

void signal_handler (int signal){
    if(signal == SIGINT){
        printf("ctrl-c signal recieved\n");
        if(munmap(shm_reg, SHM_SIZE) != 0){
            printf("Memory unmap failed, errno = %d\n", errno);
        }
        else
            printf("unmap succeeds\n");

        if(shm_unlink(SHM_NAME) != 0){
            printf("Memory unlink failed,errno = %d\n", errno);
        }
        else
        {
            printf("unlink succeeds\n");
        }
    }
}

int
main(int argc, char **argv){
    char response[1];
    int shm_fd;  // file descriptor of shared memory
    struct sigaction act;
    act.sa_handler = signal_handler; 
    sigemptyset (&act.sa_mask);
    act.sa_flags = 0;
    sigaction (SIGINT, &act, NULL);// registering the fn with type of signal

    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR , S_IRWXU | S_IRWXG); // open an existing shared memory for this name
    if (shm_fd < 0) {
        printf("failure on shm_open on shm_fd");
        exit(0);
    }

    shm_reg = (shm_region_t *)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_reg == MAP_FAILED){
        printf("Error on mapping = %d\n", errno);

        exit(0);
    }

    printf("starting pim process...\n");
    while(1){
        printf("Do you want to dump Shared memory region(y/n) ?\n");
        if(fgets(response, 2, stdin) == NULL){  // safer way of using gets is fgets
            printf("error occurred from stdin\n");
            return 0;
        }

        if(response[0] == 'y' || response[0] == 'Y'){
            dump_shm_region((void *)shm_reg);
        }
    }

    return 0;
}

