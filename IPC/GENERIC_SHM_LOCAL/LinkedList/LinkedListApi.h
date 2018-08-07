#ifndef __LINKEDLIST__
#define __LINKEDLIST__

#define uint8_t char
#define uint32_t unsigned int
#define GET_HEAD_SINGLY_LL(ll) (ll->head)
#define INC_NODE_COUNT_SINGLY_LL(ll) (ll->node_count++)
#define DEC_NODE_COUNT_SINGLY_LL(ll) (ll->node_count--)
#define GET_NODE_COUNT_SINGLY_LL(ll) (ll->node_count)
#define GET_NEXT_NODE_SINGLY_LL(node) (node->next)

typedef enum{
    SUCCESS,
    FAILURE
} rc_t;

typedef enum{
    FALSE,
    TRUE
} bool_t;

typedef struct LL_Node{
    void *data;
    struct LL_Node *next;
    //char mmap_key[32];
} singly_ll_node_t;

typedef struct LL{
    uint32_t node_count;
    uint32_t key_size;
    singly_ll_node_t *head;
} ll_t;

ll_t* init_singly_ll();
singly_ll_node_t* singly_ll_init_node(void *data);
rc_t singly_ll_add_node(ll_t *ll, singly_ll_node_t *node);
rc_t singly_ll_add_node_by_val(ll_t *ll, void *data);
rc_t singly_ll_remove_node(ll_t *ll, singly_ll_node_t *node);
uint32_t singly_ll_remove_node_by_value(ll_t *ll, void* data);
bool_t is_singly_ll_empty(ll_t *ll);
void print_singly_LL(ll_t *ll);
void reverse_singly_ll(ll_t *ll);

/* Generic Functions for polymophism*/

uint32_t ll_get_node_size();
uint32_t ll_get_key_size(void *ds);
void* ll_add_node(void *ds, void *node);
void* ll_init_node(void *key, uint32_t key_size, uint32_t max_key_size);
void* ll_lookup(void *ds, void *node);
int ll_key_compare(void *key1, void *key2, int size);
void ll_node_free(void *node);


/* Registration macros */

#define register_callbacks_for_list(shm_list)                           \
do{                                                                     \
        shm_set_compare_fn(shm_list, ll_key_compare);                   \
        shm_set_lookup_fn(shm_list, ll_lookup);                         \
        shm_set_dump_fn(shm_list, NULL);                                \
        shm_set_node_size(shm_list, ll_get_node_size);                  \
        shm_set_key_size(shm_list, ll_get_key_size);                    \
        shm_set_init_node_fn(shm_list, ll_init_node);                   \
        shm_set_node_free_fn(shm_list, ll_node_free);                   \
        shm_set_add_elem_fn(shm_list, ll_add_node);                     \
}while(0);


#endif
