#ifndef APP_STRUCT
#define APP_STRUCT

#include <stdint.h>

typedef struct student_ {

    unsigned char name[32];
    uint32_t roll_no;
    struct student_ *next;
} student_t;

#endif