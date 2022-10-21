#ifndef __MM_ALLOCATOR__
#define __MM_ALLOCATOR__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#pragma pack (push,1)

typedef struct block_meta_data_{

    bool is_free;
    uint32_t block_size;
    uint32_t offset;    /*offset from the start of the page*/
    struct block_meta_data_ *prev_block_pq;
    struct block_meta_data_ *next_block_pq;
    struct block_meta_data_ *prev_block;
    struct block_meta_data_ *next_block;
} block_meta_data_t;

typedef struct vm_page_hdr_ {

    uint32_t page_size;
    block_meta_data_t free_block_pq_head;
    block_meta_data_t block;  // must be last member
} vm_page_hdr_t;

typedef struct vm_page_xmit_ {

    void *page_base_address;
    uint64_t page_size;
    void *root_address;
    unsigned char page_memory[0];
} vm_page_xmit_data_t;

#pragma pack(pop)

void
allocator_init (void *base_address, uint32_t size);

void *
allocator_alloc_mem (void *base_address, uint32_t req_size);

void
allocator_free_mem (void *addr);

bool
allocator_is_vm_page_empty(void *base_address);

void
allocator_print_vm_page (void *base_address);

/* Helping Macros */
#define MM_GET_PAGE_FROM_META_BLOCK(block_meta_data_ptr)    \
    ((vm_page_hdr_t *)((char *)block_meta_data_ptr - block_meta_data_ptr->offset))

#define NEXT_META_BLOCK(block_meta_data_ptr)    \
    (block_meta_data_ptr->next_block)

#define NEXT_META_BLOCK_BY_SIZE(block_meta_data_ptr)    \
    (block_meta_data_t *)((char *)(block_meta_data_ptr + 1) \
        + block_meta_data_ptr->block_size)

#define PREV_META_BLOCK(block_meta_data_ptr)    \
    (block_meta_data_ptr->prev_block)

#define mm_bind_blocks_for_allocation(allocated_meta_block, free_meta_block)  \
    free_meta_block->prev_block = allocated_meta_block;        \
    free_meta_block->next_block = allocated_meta_block->next_block;    \
    allocated_meta_block->next_block = free_meta_block;                \
    if (free_meta_block->next_block)\
    free_meta_block->next_block->prev_block = free_meta_block

#define mm_bind_blocks_for_deallocation(freed_meta_block_down, freed_meta_block_top)    \
    freed_meta_block_down->next_block = freed_meta_block_top->next_block;               \
    if(freed_meta_block_top->next_block)                                                \
    freed_meta_block_top->next_block->prev_block = freed_meta_block_down

static inline int
mm_get_hard_internal_memory_frag_size(
            block_meta_data_t *first,
            block_meta_data_t *second){

    block_meta_data_t *next_block = NEXT_META_BLOCK_BY_SIZE(first);  
    return (int)((unsigned long)second - (unsigned long)(next_block));
}

static inline bool
allocator_is_vm_page_free(void *base_address);

#endif 