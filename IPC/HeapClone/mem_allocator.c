#include <assert.h>
#include <memory.h>
#include <stdlib.h>
#include "mem_allocator.h"

static void
allocator_add_block_before_current_block 
                (block_meta_data_t *curr_block, 
                 block_meta_data_t *new_block){
        
    if(!curr_block->prev_block_pq){
        new_block->prev_block_pq = NULL;
        new_block->next_block_pq = curr_block;
        curr_block->prev_block_pq = new_block;
        return;
    }
    
    block_meta_data_t  *temp = curr_block->prev_block_pq;
    temp->next_block_pq = new_block;
    new_block->prev_block_pq = temp;
    new_block->next_block_pq = curr_block;
    curr_block->prev_block_pq = new_block;
}

static void
allocator_add_block_after_current_block 
                (block_meta_data_t *curr_block, 
                 block_meta_data_t *new_block){
        
    if(!curr_block->next_block_pq){
        curr_block->next_block_pq = new_block;
        new_block->prev_block_pq = curr_block;
        return;
    }

    block_meta_data_t *temp = curr_block->next_block_pq;
    curr_block->next_block_pq = new_block;
    new_block->prev_block_pq = curr_block;
    new_block->next_block_pq = temp;
    temp->prev_block_pq = new_block;
}

static void
allocator_remove_block_from_free_list (block_meta_data_t *curr_block){
    
    if(!curr_block->prev_block_pq){
        if(curr_block->next_block_pq){
            curr_block->next_block_pq->prev_block_pq = NULL;
            curr_block->next_block_pq = 0;
            return;
        }
        return;
    }
    if(!curr_block->next_block_pq){
        curr_block->prev_block_pq->next_block_pq = NULL;
        curr_block->prev_block_pq = NULL;
        return;
    }

    curr_block->prev_block_pq->next_block_pq = curr_block->next_block_pq;
    curr_block->next_block_pq->prev_block_pq = curr_block->prev_block_pq;
    curr_block->prev_block_pq = 0;
    curr_block->next_block_pq = 0;
}

static void
allocator_add_block_to_free_block_list (
        block_meta_data_t *block_list,
        block_meta_data_t *block_to_add) {

    if (!block_list->next_block_pq) {
        allocator_add_block_after_current_block(block_list, block_to_add);
        return;
    }

    block_meta_data_t *curr_block, *prev_block;

    for (curr_block = block_list->next_block_pq; 
            curr_block; 
            curr_block = curr_block->next_block_pq) {

        prev_block = curr_block;
        if (curr_block->block_size >= block_to_add->block_size) continue;
        allocator_add_block_before_current_block(curr_block, block_to_add);
        return;
    }

    allocator_add_block_after_current_block(prev_block, block_to_add);
}

static block_meta_data_t *
allocator_deque_block_from_free_block_list (
        block_meta_data_t *block_list) {
    
    block_meta_data_t *first_block;
    if (!block_list->next_block_pq) return NULL;
    first_block = block_list->next_block_pq;
    allocator_remove_block_from_free_list(first_block);
    return first_block;
}

static void
mm_union_free_blocks(block_meta_data_t *first,
        block_meta_data_t *second){

    assert(first->is_free == true &&
                second->is_free == true);
    allocator_remove_block_from_free_list(first);
    allocator_remove_block_from_free_list(second);
    mm_bind_blocks_for_deallocation(first, second);
}

void
allocator_init (void *base_address, uint32_t size) {

    vm_page_hdr_t *vm_page_hdr = (vm_page_hdr_t *)base_address;
    vm_page_hdr->page_size = size;
    vm_page_hdr->block.is_free = true;
    vm_page_hdr->block.block_size = size - sizeof(vm_page_hdr_t);
    vm_page_hdr->block.offset = offsetof(vm_page_hdr_t, block);
    vm_page_hdr->block.prev_block = NULL;
    vm_page_hdr->block.next_block = NULL;
    vm_page_hdr->block.prev_block_pq = NULL;
    vm_page_hdr->block.next_block_pq = NULL;
    allocator_add_block_to_free_block_list(
                &vm_page_hdr->free_block_pq_head,
                &vm_page_hdr->block);
}

static bool
allocator_split_free_data_block_for_allocation (
            vm_page_hdr_t *vm_page,
            block_meta_data_t *block_meta_data,
            uint32_t size){

    block_meta_data_t *next_block_meta_data = NULL;

    assert(block_meta_data->is_free);

    if (block_meta_data->block_size < size ){
        return false;
    }

    uint32_t remaining_size =
        block_meta_data->block_size - size;

    block_meta_data->is_free = false;
    block_meta_data->block_size = size;

    /* Since this block of memory is going to be allocated to the application, 
     * remove it from priority list of free blocks*/
    allocator_remove_block_from_free_list(block_meta_data);
    
    /*Case 1 : No Split*/
    if(!remaining_size){
        return true;
    }

    if (remaining_size < sizeof(block_meta_data_t)) {
        return true;
    }

    /*New Meta block is to be created*/
    next_block_meta_data = NEXT_META_BLOCK_BY_SIZE(block_meta_data);
    next_block_meta_data->is_free = true;
    next_block_meta_data->block_size =
        remaining_size - sizeof(block_meta_data_t);
    next_block_meta_data->offset = block_meta_data->offset +
                                   sizeof(block_meta_data_t) + block_meta_data->block_size;
    allocator_add_block_to_free_block_list(
        &vm_page->free_block_pq_head,
        next_block_meta_data);
    mm_bind_blocks_for_allocation(block_meta_data, next_block_meta_data);
    return true;
}

void *
allocator_alloc_mem (void *base_address, uint32_t req_size) {

    vm_page_hdr_t *vm_page_hdr = (vm_page_hdr_t *)base_address;
    block_meta_data_t *block_meta_data = allocator_deque_block_from_free_block_list(
                &vm_page_hdr->free_block_pq_head);
    if (!block_meta_data) return NULL;
    assert(block_meta_data->is_free);
    if (block_meta_data->block_size < req_size) return NULL;
    if (!allocator_split_free_data_block_for_allocation(
            vm_page_hdr, block_meta_data, req_size)) {
        return NULL;
    }

    memset((char *)(block_meta_data + 1), 0, block_meta_data->block_size);
    return (void *)(block_meta_data + 1);
}

static void 
allocator_free_block (block_meta_data_t *to_be_free_block) {

    block_meta_data_t *return_block;

    return_block = to_be_free_block;

    assert(!to_be_free_block->is_free);
    
    vm_page_hdr_t *vm_page = 
        MM_GET_PAGE_FROM_META_BLOCK(to_be_free_block);
   
    to_be_free_block->is_free = true;
    
    block_meta_data_t *next_block = NEXT_META_BLOCK(to_be_free_block);

    /*Handling Hard IF memory*/
    if(next_block){
        /*Scenario 1 : When data block to be freed is not the last 
         * upper most meta block in a VM data page*/
        to_be_free_block->block_size += 
            mm_get_hard_internal_memory_frag_size (to_be_free_block, next_block);
    }
    else {
        /* Scenario 2: Page Boundary condition*/
        /* Block being freed is the upper most free data block
         * in a VM data page, check of hard internal fragmented 
         * memory and merge*/
        char *end_address_of_vm_page = (char *)((char *)vm_page + vm_page->page_size);
        char *end_address_of_free_data_block = 
            (char *)(to_be_free_block + 1) + to_be_free_block->block_size;
        int internal_mem_fragmentation = (int)((unsigned long)end_address_of_vm_page - 
                (unsigned long)end_address_of_free_data_block);
        to_be_free_block->block_size += internal_mem_fragmentation;
    }
    
    /*Now perform Merging*/
    if(next_block && next_block->is_free){

        to_be_free_block->block_size += 
            next_block->block_size + sizeof(block_meta_data_t);
        /*Union two free blocks*/
        mm_union_free_blocks(to_be_free_block, next_block);
        return_block = to_be_free_block;
    }

    /*Check the previous block if it was free*/
    block_meta_data_t *prev_block = PREV_META_BLOCK(to_be_free_block);
    
    if(prev_block && prev_block->is_free){
        
        prev_block->block_size += 
            to_be_free_block->block_size + sizeof(block_meta_data_t);
        mm_union_free_blocks(prev_block, to_be_free_block);
        return_block = prev_block;
    }
   
    allocator_add_block_to_free_block_list(
                &vm_page->free_block_pq_head,
                return_block);
}

void
allocator_free_mem (void *addr) {

    block_meta_data_t *block_meta_data = 
        (block_meta_data_t *)((char *)addr - sizeof(block_meta_data_t));
    
    assert(!block_meta_data->is_free);
    allocator_free_block(block_meta_data);
}

bool
allocator_is_vm_page_empty (void *base_address) {

    vm_page_hdr_t *vm_page = (vm_page_hdr_t *)base_address;

    if ((vm_page->page_size !=
         (vm_page->block.block_size +
          sizeof(block_meta_data_t) +
          sizeof(vm_page->free_block_pq_head) +
          sizeof(vm_page->page_size)))
        ||
        !vm_page->block.is_free) {

        return false;
    }

    block_meta_data_t *block_meta_data = vm_page->free_block_pq_head.next_block_pq;
    
    assert(block_meta_data);

    if (block_meta_data != &vm_page->block) return false;

    if (block_meta_data->next_block_pq || 
            (block_meta_data->prev_block_pq != &vm_page->free_block_pq_head)) {

        return false;
    }

    return true;
}

void
allocator_print_vm_page (void *base_address) {


}

#if 1
int
main(int argc, char **argv) {

    void *base_address = (void *)calloc (1, 1024);
    allocator_init(base_address, 1024);

    void *ptr1 = allocator_alloc_mem(base_address, 48);
    void *ptr2 = allocator_alloc_mem(base_address, 98);
    allocator_free_mem(ptr1);
    void *ptr3 = allocator_alloc_mem(base_address, 198);
    allocator_free_mem(ptr2);
    allocator_free_mem(ptr3);
    assert(allocator_is_vm_page_empty(base_address));
    free(base_address);
    return 0;
}
#endif
