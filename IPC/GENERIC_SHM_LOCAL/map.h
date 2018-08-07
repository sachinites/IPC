#ifndef __MAP__
#define __MAP__

#include "struct.h"
typedef struct shm_ds shm_ds_t;

int
add_value_in_physical_memory(char *mmap_key, void *value, struct_id_t struct_id);
void *
get_value_from_physical_memory(shm_ds_t *ds, void *node, unsigned int *value_size);
#endif
