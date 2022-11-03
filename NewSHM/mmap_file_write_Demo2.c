#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <memory.h>

/* This program shows how mmap() can be used to memory map the text
file present on disk into process's Virtual address space */
static void  breakpoint () {}

typedef struct student_ {

    int roll_no;
    int marks;
    char name[128];
    char city[128];

} student_t;

int 
main(int argc, char *argv[]){

    if(argc < 2){
        printf("File path not mentioned\n");
        exit(0);
    }
   
    const char *filepath = argv[1];

    /* OPen the file in Read-Write OMode */
    int fd = open(filepath, O_RDWR );

    if(fd < 0){
        printf("\n\"%s \" could not open\n",
               filepath);
        exit(1);
    }

    /* Extract the size of the file */
    struct stat statbuf;
    int err = fstat(fd, &statbuf);
    if(err < 0){
        printf("\n\"%s \" could not open\n",
                       filepath);
        exit(2);
    }

    char *ptr = mmap(NULL, /*Let Kernel decide the starting address of mapping in Virtual Memory */
                                  statbuf.st_size,  /* Memory size to Map */
                                  PROT_READ | PROT_WRITE,  /* Map the memory in Read-Write mode, meaning the Virtual Memory can be read 
                                                                and written by the process. Note that, this permission must be compatible
                                                                with permissions with which the file is open using open () */
                                MAP_SHARED,  /* To see the changes in the file, should be always MAP_SHARED.   */
                                fd,                         /* FD represents the identifier of the external data source, in this case it is a file */
                                0);                         /* Offset into the text file from which the mapping has to begin, pass zero to map from the 
                                                                    beginning of the file */ 

    if(ptr == MAP_FAILED){
        printf("Mapping Failed, errorno = %d\n", errno);
        return 1;
    }
    
     close(fd);      /* We will not going to read/write into file using file-handling routines, hence close the fd. mmap can still
                                use this file even if its fd is closed */

    student_t stud = {123, 90, "Abhishek", "Bangalore"};

    memcpy (ptr, &stud, sizeof(stud));

    msync(ptr, sizeof(stud), MS_SYNC);

    err = munmap(ptr, statbuf.st_size);     /* Destroy the mapping once done */
   
    if(err != 0){
        printf("UnMapping Failed\n");
        return 1;
    }
    
    return 0;
}