#include "shm_ds.h"
#include "stdlib.h"
#include "memory.h"

static int shm_unit_key_index = 1;

shm_ds_t*
init_shm_ds(void *ds){
	shm_ds_t *shm_ds 	= (shm_ds_t *)calloc(1, sizeof(shm_ds_t));
	shm_ds->ds 	     	= ds;
	shm_ds->add_elem     	= NULL;
	shm_ds->lookup       	= NULL;
	shm_ds->dump 	     	= NULL;
	shm_ds->init_node    	= NULL;
	shm_ds->compare      	= NULL;
	shm_ds->get_node_size	= NULL;
	shm_ds->get_key_size 	= NULL;
	shm_ds->node_free    	= NULL;
	return shm_ds;
}


void
shm_set_init_node_fn(shm_ds_t *shm_ds, _init_node init_node){
	shm_ds->init_node = init_node;
}

void
shm_set_node_free_fn(shm_ds_t *shm_ds, _node_free node_free){
	shm_ds->node_free = node_free;
}

void
shm_set_add_elem_fn(shm_ds_t *shm_ds, _add_elem shm_add_elem){
	shm_ds->add_elem = shm_add_elem;
}

void
shm_set_lookup_fn(shm_ds_t *shm_ds, _lookup lookup){
	shm_ds->lookup = lookup;
}

void
shm_set_dump_fn(shm_ds_t *shm_ds, _dump dump){
	shm_ds->dump = dump;
}

void
shm_set_compare_fn(shm_ds_t *shm_ds, _compare compare){
	shm_ds->compare = compare;
}

void
shm_set_node_size(shm_ds_t *shm_ds, _get_node_size get_node_size){
	shm_ds->get_node_size = get_node_size;
}


void
shm_set_key_size(shm_ds_t *shm_ds, _get_key_size get_key_size){
	shm_ds->get_key_size = get_key_size;
}

static void*
shm_augment_node(void *node, int sizeofnode, struct_id_t struct_id){
	void *aug_node = calloc(1, sizeofnode + MMAP_KEY_SIZE + sizeof(unsigned int));
	memcpy(aug_node, node, sizeofnode);
	char *end = (char *)aug_node + sizeofnode;
	char buff[MMAP_KEY_SIZE];
	memset(buff, 0, MMAP_KEY_SIZE);
	itoa(shm_unit_key_index, buff, 10);
	shm_unit_key_index++;
	memcpy((void *)end, buff, MMAP_KEY_SIZE);
	end = (char *)end + MMAP_KEY_SIZE;
	*(unsigned int *)end = sizeofstruct(struct_id);
	return aug_node;
}

int
shm_add_elem(shm_ds_t *ds, void *key, unsigned int key_size, void *value, struct_id_t struct_id){
	void *node = ds->init_node (key, key_size, ds->get_key_size(ds->ds));
	void *aug_node = shm_augment_node (node, ds->get_node_size(),  struct_id);
	free(node);
	ds->add_elem((void *)ds->ds, aug_node);
	add_value_in_physical_memory((char *)aug_node + ds->get_node_size(), value, struct_id);
	return 0;
}
