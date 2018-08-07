
#include <stdlib.h>
#include <stdio.h>  /* for printf */
#include <string.h> /* for strlen */

    int
main(int argc, char **argv)
{
    int n;
    int fd[2];   // create two file descriptors
    char buf[1025];
    char *data = "hello... this is sample data";

    pipe(fd); // open a pipe , fd[0] for reading, and fd[1] for writing the data
    write(fd[1], data, strlen(data)); // write the data to the pipe
    if ((n = read(fd[0], buf, 1024)) >= 0) {   // read the data from the pipe, if data is not available , it will wait or block here
        buf[n] = 0; /* terminate the string */
        printf("read %d bytes from the pipe: \"%s\"\n", n, buf);
    }   
    else
        perror("read");
    exit(0);
}
