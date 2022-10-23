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
#include <memory.h>
#include "app_struct.h"
#include "mem_allocator.h"

#define SERVER_PORT 50000
#define SERVER_IP_ADDR  "127.0.0.1"

int 
main(int argc, char **argv) {

    socklen_t addr_len = sizeof(struct sockaddr);
    const uint32_t page_size = getpagesize();

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if (sockfd < 0) {
        printf ("UDP socket creation failed with error = %d\n", errno);
        return -1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    struct hostent *host = (struct hostent *)gethostbyname(SERVER_IP_ADDR);
    server.sin_addr = *((struct in_addr *)host->h_addr);

    bind(sockfd, (struct sockaddr *)&server, sizeof(server));

    size_t buffer_size = sizeof(vm_page_xmit_data_t) + page_size;

    unsigned char *buffer  = 
        (unsigned char *)calloc (1, sizeof(vm_page_xmit_data_t) + page_size);

    int recv_bytes =  recvfrom(sockfd,
                                                (char *)buffer, buffer_size, 0,
                                                (struct sockaddr *)&server, &addr_len);

    printf ("Bytes recvd = %d\n", recv_bytes);

    vm_page_xmit_data_t *vm_page_xmit_data = (vm_page_xmit_data_t *)buffer;

    printf ("vm_page_xmit_data->page_size = %lu\n", vm_page_xmit_data->page_size);

    void *vm_page_memory = mmap(
            vm_page_xmit_data->page_base_address,
            vm_page_xmit_data->page_size,
            PROT_READ|PROT_WRITE|PROT_EXEC,
            MAP_ANON|MAP_PRIVATE,
            0, 0);

    if (vm_page_memory == MAP_FAILED) {
        printf("Error : VM Page allocation Failed, errorno = %d\n", errno);
        return -1;
    }

    memcpy (vm_page_memory, vm_page_xmit_data->page_memory, page_size);

    student_t *stud1 = (student_t *)vm_page_xmit_data->root_address;
    student_t *stud = stud1;

    while (stud) {

        printf ("Name = %s , Roll No = %u\n", stud->name, stud->roll_no);
        stud = stud->next;
    }
 
    /* Continue to use the student linked list */
    /* no need to invoke allocator_init() because the above memcpy do it */
    stud =  (student_t *)allocator_alloc_mem(vm_page_memory, sizeof(student_t));
    strcpy((char *)stud->name, "Mayank");
    stud->roll_no = 4;
    stud->next = NULL;

    stud->next = stud1;

    /* We are done release the resources */
    close (sockfd);

    /* No need to free linkedlist , we destroy the entire VM page */
    munmap (vm_page_memory, vm_page_xmit_data->page_size);
    free(buffer);
    return 0;
}