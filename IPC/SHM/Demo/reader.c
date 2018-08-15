#include <stdio.h>
#include <string.h>

extern 
int
read_from_shared_memory (char *mmap_key,
        char *buffer,
        unsigned int buff_size,
        unsigned int bytes_to_read);

int 
main (int argc, char **argv){

    char *key = "/introduction";
    char read_buffer[128];
    memset(read_buffer, 0, 128);
    int rc = read_from_shared_memory(key, read_buffer, 128, 128);
    if(rc < 0){
        printf("Error reading from shared memory\n");
        return 0;
    }
    printf("Data read = %s\n", (char *)read_buffer);
    return 0;
}
