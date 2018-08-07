#include "struct.h"
#include "memory.h"
#include <stdio.h>

unsigned int 
sizeofstruct(struct_id_t struct_id){
	switch(struct_id){
		case INTEGER:
			return sizeof(int);
		case DOUBLE:
			return sizeof(double);
		case CHAR_ARRAY:
			return 64;
		case PERSON:
			return sizeof(person_t);
	}
}

void
copy(struct_id_t struct_id, void *src, void *dst){
	switch(struct_id){
		case INTEGER:
		case DOUBLE:
			memcpy(dst, src, sizeofstruct(struct_id));
			return;
		case CHAR_ARRAY:
			if(strlen((char *)src) < 64)
				memcpy(dst, src, strlen((char *)src));
			else
				memcpy(dst, src, 64);
			return;
		case PERSON:
			memcpy(dst, src, sizeofstruct(struct_id));
			return;
	}
}

