#ifndef __STRUCT__
#define __STRUCT__

typedef enum{
	INTEGER = 0,
	DOUBLE,
	CHAR_ARRAY,
	PERSON
} struct_id_t;

void
copy_struct(struct_id_t struct_id, void *src, void *dst);

unsigned int 
sizeofstruct(struct_id_t struct_id);

typedef struct _person{
	char name[32];
	char address[64];
	int age;
} person_t;
#endif
