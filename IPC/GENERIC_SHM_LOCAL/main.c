#include <stdio.h>
#include <stdlib.h>
#include "shm_ds.h"
#include "map.h"
#include "./LinkedList/LinkedListApi.h"
#include "./Trees/tree.h"
#include <string.h>

int main(int argc, char **argv){
	ll_t* list = init_singly_ll(32);
	shm_ds_t *shm_list = init_shm_ds((void *)list);	
	register_callbacks_for_list(shm_list);

	tree_t *tree = init_tree(32);
	shm_ds_t *shm_tree = init_shm_ds((void *)tree);
	register_callbacks_for_tree(shm_tree);

	int key = 10; char val[32] = "ve10";
	SHM_ADD_ELEM(shm_list, &key, sizeof(key), val, CHAR_ARRAY);
	SHM_ADD_ELEM(shm_tree, &key, sizeof(key), val, CHAR_ARRAY);
	key = 11; 
	strncpy(val, "ve11", strlen("ve11"));
	SHM_ADD_ELEM(shm_list, &key, sizeof(key), val, CHAR_ARRAY);
	SHM_ADD_ELEM(shm_tree, &key, sizeof(key), val, CHAR_ARRAY);
	
	void *result = NULL; unsigned int val_size = 0;	
	SHM_LOOKUP(shm_list, &key, result, val_size);	
	if(result)
		printf("key look up = %s, size = %d\n\n", (char *)result, val_size);
	else
		printf("no look up result\n");

	key = 10;
	SHM_LOOKUP(shm_tree, &key, result, val_size);	
	if(result)
		printf("key look up = %s, size = %d\n\n", (char *)result, val_size);
	else
		printf("no look up result\n");

	char ip_key[16] = "225.1.1.1";
	char ip_val[64] = "100.1.1.1,100.1.1.2,100.1.1.3";


	SHM_ADD_ELEM(shm_list, ip_key, 16, ip_val, CHAR_ARRAY);
	if(fork() == 0){
		SHM_LOOKUP(shm_list, ip_key, result, val_size);
		if(result)
			printf("by child : key look up = %s, size = %d\n\n", (char *)result, val_size);
		else
			printf("by child : no look up result\n");
		exit(0);
	}
	else
		wait(NULL);


	SHM_LOOKUP(shm_list, ip_key, result, val_size);
	if(result)
		printf("by parent : key look up = %s, size = %d\n\n", (char *)result, val_size);
	else
		printf("by parent : no look up result\n");

	int person_id = 12345;
	person_t p;
	strcpy(p.name, "Abhishek");
	strcpy(p.address,"Bangalore");
	p.age = 30;
	
	SHM_ADD_ELEM(shm_list, &person_id, sizeof(int), &p, PERSON);
	SHM_LOOKUP(shm_list, &person_id, result, val_size);
	printf("Name    = %s\n", ((person_t *)result)->name);
	printf("Address = %s\n", ((person_t *)result)->address);
	printf("Age     = %d\n", ((person_t *)result)->age);


	return 0;
}
