#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include "app_struct.h"
#include "mem_allocator.h"

#define DEST_PORT   50000
#define DEST_IP_ADDRESS "127.0.0.1"

int
main(int argc, char **argv) {

    const uint32_t page_size = getpagesize();

    void *vm_page_memory = mmap(
            NULL,
           page_size,
            PROT_READ|PROT_WRITE|PROT_EXEC,
            MAP_ANON|MAP_PRIVATE,
            0,0);

    if (vm_page_memory == MAP_FAILED) {
        printf("Error : VM Page allocation Failed\n");
        return -1;
    }

    printf ("VM page is allocated of size %u\n", page_size);

    /* Lets use our allocator to further alloc/dealloc memory from this VM page */
    allocator_init(vm_page_memory,  page_size);

    student_t *stud1 = (student_t *)allocator_alloc_mem(vm_page_memory, sizeof(student_t));
    strcpy((char *)stud1->name, "Abhishek");
    stud1->roll_no = 1;
    stud1->next = NULL;

    student_t *stud2 = (student_t *)allocator_alloc_mem(vm_page_memory, sizeof(student_t));
    strcpy((char *)stud2->name, "Shivani");
    stud2->roll_no = 2;
    stud2->next = NULL;

    stud1->next = stud2;

    student_t *stud3 = (student_t *)allocator_alloc_mem(vm_page_memory, sizeof(student_t));
    strcpy((char *)stud3->name, "Twisha");
    stud3->roll_no = 3;
    stud3->next = NULL;

    stud2->next = stud3;

    vm_page_xmit_data_t *vm_page_xmit_data = (vm_page_xmit_data_t *)calloc(1, sizeof(vm_page_xmit_data_t) + getpagesize());

    vm_page_xmit_data->page_base_address = vm_page_memory;
    vm_page_xmit_data->page_size = page_size;
    vm_page_xmit_data->root_address = (void *)stud1;
    memcpy (vm_page_xmit_data->page_memory, vm_page_memory, page_size);
   
    /* Open a UDP socket to remote process and send VM page as it is */
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (sockfd < 0) {
        printf ("UDP socket creation failed with error = %d\n", errno);
        return -1;
    }

    /* Now send data to remote process */
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(DEST_PORT);
    struct hostent *host = (struct hostent *)gethostbyname(DEST_IP_ADDRESS);
    dest.sin_addr = *((struct in_addr *)host->h_addr);

    int sent_bytes = sendto(sockfd,
           vm_page_xmit_data,
           sizeof(vm_page_xmit_data_t) + page_size,
           0,
           (struct sockaddr *)&dest,
           sizeof(struct sockaddr));

    printf ("Bytes send = %d\n", sent_bytes);
    close (sockfd);
    free(vm_page_xmit_data);

    printf ("process halt\n");
   
    scanf("\n");
    return 0;
} 
