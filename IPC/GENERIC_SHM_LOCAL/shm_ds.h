/* forward declaration */

#ifndef __SHM_DS__
#define __SHM_DS__
#include "struct.h"
#define MMAP_KEY_SIZE	32

typedef struct shm_ds shm_ds_t;

typedef void* (*_add_elem)(void *ds, void *node);
typedef void* (*_lookup)(void *ds, void *key);
typedef void  (*_dump)(void *ds);
typedef int   (*_compare)(void *key1, void *key2, int key_size);
typedef void* (*_init_node)(void *key, unsigned int key_size, unsigned int max_key_size);
typedef unsigned int (*_get_node_size)();
typedef unsigned int (*_get_key_size)(void *ds);
typedef void (*_node_free)(void *node);

struct shm_ds{
	void *ds;
	_add_elem add_elem;
	_lookup  lookup;
	_dump dump;
	_init_node init_node;
	_compare compare;
	_get_node_size get_node_size;
	_get_key_size get_key_size;
	_node_free node_free;
};

shm_ds_t*
init_shm_ds(void *ds);


void
shm_set_node_free_fn(shm_ds_t *shm_ds, _node_free node_free);

void
shm_set_init_node_fn(shm_ds_t *shm_ds, _init_node init_node);

void
shm_set_add_elem_fn(shm_ds_t *shm_ds, _add_elem shm_add_elem);

void
shm_set_lookup_fn(shm_ds_t *shm_ds, _lookup lookup);
 
void 
shm_set_dump_fn(shm_ds_t *shm_ds, _dump dump);

void shm_set_compare_fn(shm_ds_t *shm_ds, _compare compare);

void
shm_set_node_size(shm_ds_t *shm_ds, _get_node_size get_node_size);

void
shm_set_key_size(shm_ds_t *shm_ds, _get_key_size get_key_size);


/* Shared Memory Absolute Functions*/

int
shm_add_elem(shm_ds_t *ds, void *key, unsigned int key_size, void *value, struct_id_t struct_id);

/* Macros */

#define SHM_ADD_ELEM(ds, key, key_size, elem, struct_type)	shm_add_elem(ds, (void *)key, key_size, (void *)elem, struct_type)

#define SHM_LOOKUP(_ds, key, result, result_size)			\
do{									\
result = (_ds->lookup((void *)(_ds->ds), (void *)key));  	  	\
if(result)								\
    result=(void *)get_value_from_physical_memory(_ds, result,		\
		&result_size); 						\
}while(0);


#endif /* __SHM_DS__ */
