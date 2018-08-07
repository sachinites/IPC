#ifndef __TREE__
#define __TREE__

#define SUCCESS 0
#define FAILURE -1


typedef struct tree_node{
  struct tree_node *left;
  struct tree_node *right;
  void *data;
} tree_node_t;

typedef struct tree{
    tree_node_t *root;
    unsigned int key_size;
} tree_t;


/* Generic Functions for polymophism*/

unsigned int tree_get_node_size();
unsigned int tree_get_key_size(void *ds);
void* tree_add_node(void *ds, void *node);
void* tree_init_node(void *key, unsigned int key_size, unsigned int max_key_size);
void* tree_lookup(void *ds, void *node);
int tree_key_compare(void *key1, void *key2, int size);
tree_t *init_tree(int key_size);
void tree_node_free(void *node);

/* Registration macros */

#define register_callbacks_for_tree(shm_tree)                             \
do{                                                                       \
        shm_set_compare_fn(shm_tree, tree_key_compare);                   \
        shm_set_lookup_fn(shm_tree, tree_lookup);                         \
        shm_set_dump_fn(shm_tree, NULL);                                  \
        shm_set_node_size(shm_tree, tree_get_node_size);                  \
        shm_set_key_size(shm_tree, tree_get_key_size);                    \
        shm_set_init_node_fn(shm_tree, tree_init_node);                   \
        shm_set_node_free_fn(shm_tree, tree_node_free);                   \
        shm_set_add_elem_fn(shm_tree, tree_add_node);                     \
}while(0);



#endif
