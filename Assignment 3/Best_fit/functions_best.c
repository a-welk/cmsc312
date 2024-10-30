#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "functions_best.h"
// Don't include stdlb since the names will conflict?

// TODO: align

// sbrk some extra space every time we need it.
// This does no bookkeeping and therefore has no ability to free, realloc, etc.
void *nofree_malloc(size_t size) {
  void *p = sbrk(0);
  void *request = sbrk(size);
  if (request == (void*) -1) { 
    return NULL; // sbrk failed
  } else {
    assert(p == request); // Not thread safe.
    return p;
  }
}

struct block_meta {
  size_t size;
  struct block_meta *next;
  struct block_meta *prev;
  int free;
  int magic;    // For debugging only. TODO: remove this in non-debug mode.
};

#define META_SIZE sizeof(struct block_meta)

void *global_base = NULL;

// Iterate through blocks until we find one that's large enough.
// TODO: split block up if it's larger than necessary
struct block_meta *find_best_fit_block(struct block_meta **last, size_t size) {
    struct block_meta *current = global_base;
    struct block_meta *best_fit = NULL;
    size_t min_size = current->size + 1; // Initialize to a value larger than any possible block size


    while (current != NULL) {
        if (current->free && current->size >= size && current->size < min_size) {
            best_fit = current;
            min_size = current->size;
        }
        *last = current;
        current = current->next;
    }
    return best_fit;
}

struct block_meta *request_space(struct block_meta* last, size_t size) {
  struct block_meta *block;
  block = sbrk(0);
  void *request = sbrk(size + META_SIZE);
  assert((void*)block == request); // Not thread safe.
  if (request == (void*) -1) {
    return NULL; // sbrk failed.
  }
  
  if (last) { // NULL on first request.
    last->next = block;
  }
  block->size = size;
  block->next = NULL;
  block->prev = last;
  block->free = 0;
  block->magic = 0x12345678;
  return block;
}

// If it's the first ever call, i.e., global_base == NULL, request_space and set global_base.
// Otherwise, if we can find a free block, use it.
// If not, request_space.
void *malloc(size_t size) {
  struct block_meta *block;
  // TODO: align size?

  if (size <= 0) {
    return NULL;
  }

  if (!global_base) { // First call.
    block = request_space(NULL, size);
    if (!block) {
      return NULL;
    }
    global_base = block;
  } else {
    struct block_meta *last = global_base;
    block = find_best_fit_block(&last, size);
    if (!block) { // Failed to find free block.
      block = request_space(last, size);
      if (!block) {
	return NULL;
      }
    } else {      // Found free block
      // TODO: consider splitting block here.
      block->free = 0;
      block->magic = 0x77777777;
    }
  }
  
  return(block+1);
}

void *calloc(size_t nelem, size_t elsize) {
  size_t size = nelem * elsize;
  void *ptr = malloc(size);
  memset(ptr, 0, size);
  return ptr;
}

// TODO: maybe do some validation here.
struct block_meta *get_block_ptr(void *ptr) {
  return (struct block_meta*)ptr - 1;
}

void free(void *ptr) {
  if (!ptr) {
    return;
  }

  // TODO: consider merging blocks once splitting blocks is implemented.
  struct block_meta* block_ptr = get_block_ptr(ptr);
  //assert(block_ptr->free == 0);
  //assert(block_ptr->magic == 0x77777777 || block_ptr->magic == 0x12345678);
  block_ptr->free = 1;
  block_ptr->magic = 0x55555555;

  if (block_ptr->prev)
      block_ptr->prev->next = block_ptr->next;
  if (block_ptr->next)
      block_ptr->next->prev = block_ptr->prev;  
}

void *realloc(void *ptr, size_t size) {
  if (!ptr) { 
    // NULL ptr. realloc should act like malloc.
    return malloc(size);
  }

  struct block_meta* block_ptr = get_block_ptr(ptr);
  if (block_ptr->size >= size) {
    // We have enough space. Could free some once we implement split.
    return ptr;
  }

  // Need to really realloc. Malloc new space and free old space.
  // Then copy old data to new space.
  void *new_ptr;
  new_ptr = malloc(size);
  if (!new_ptr) {
    return NULL; // TODO: set errno on failure.
  }
  memcpy(new_ptr, ptr, block_ptr->size);
  free(ptr);  
  return new_ptr;
}


void demonstration() {
  void *ptr = malloc(7);
  void *ptr2 = malloc(4);
  void *ptr3 = malloc(11);

  printf("Allocated memory address with malloc: %p\n", ptr);
  printf("Allocated memory address with malloc: %p\n", ptr2);
  printf("Allocated memory address with malloc: %p\n", ptr3);
  printf("Freeing all memory locations\n");

  free(ptr);
  free(ptr2);
  free(ptr3);

  void *ptr4 = malloc(3);
  void *ptr5 = malloc(1);
  printf("Allocated memory address with malloc: %p\n", ptr4);
  printf("Allocated memory address with malloc: %p\n", ptr5);
  printf("Freeing all memory locations\n");
  free(ptr4);
  free(ptr5);


  ptr = calloc(1, 7);
  ptr2 = calloc(1, 4);
  ptr3 = calloc(1, 11);
  printf("Allocated memory address with calloc: %p\n", ptr);
  printf("Allocated memory address with calloc: %p\n", ptr2);
  printf("Allocated memory address with calloc: %p\n", ptr3);
  printf("Freeing all memory locations\n");
  free(ptr);
  free(ptr2);
  free(ptr3);

  ptr4 = calloc(1, 3);
  ptr5 = calloc(1, 1);

  printf("Allocated memory address with calloc: %p\n", ptr4);
  printf("Allocated memory address with calloc: %p\n", ptr5);
  printf("Freeing all memory locations\n");
  free(ptr4);
  free(ptr5);

  ptr = malloc(14);
  ptr2 = malloc(19);
  ptr3 = malloc(8);
  ptr4 = malloc(6);
  ptr5 = malloc(11);
  printf("Allocated memory address with malloc: %p\n", ptr);
  printf("Allocated memory address with malloc: %p\n", ptr2);
  printf("Allocated memory address with malloc: %p\n", ptr3);
  printf("Allocated memory address with malloc: %p\n", ptr4);
  printf("Allocated memory address with malloc: %p\n", ptr5);
  ptr = realloc(ptr, 20);
  ptr2 = realloc(ptr2, 18);
  ptr3 = realloc(ptr3, 26);
  ptr4 = realloc(ptr4, 22);
  ptr5 = realloc(ptr5, 24);
  printf("Reallocated to memory address: %p\n", ptr);
  printf("Reallocated to memory address: %p\n", ptr2);
  printf("Reallocated to memory address: %p\n", ptr3);
  printf("Reallocated to memory address: %p\n", ptr4);
  printf("Reallocated to memory address: %p\n", ptr5);
  printf("Freeing all memory locations\n");
  free(ptr);
  free(ptr2);
  free(ptr3);
  free(ptr4);
  free(ptr5);

  ptr = calloc(1, 8);
  ptr2 = calloc(1, 4);
  ptr3 = calloc(1, 13);
  ptr4 = calloc(1, 2);
  ptr5 = calloc(1, 1);
  printf("Allocated memory address with calloc: %p\n", ptr);
  printf("Allocated memory address with calloc: %p\n", ptr2);
  printf("Allocated memory address with calloc: %p\n", ptr3);
  printf("Allocated memory address with calloc: %p\n", ptr4);
  printf("Allocated memory address with calloc: %p\n", ptr5);
  ptr = realloc(ptr, 27);
  ptr2 = realloc(ptr2, 23);
  ptr3 = realloc(ptr3, 20);
  ptr4 = realloc(ptr4, 17);
  ptr5 = realloc(ptr5, 15);
  printf("Reallocated to memory address: %p\n", ptr);
  printf("Reallocated to memory address: %p\n", ptr2);
  printf("Reallocated to memory address: %p\n", ptr3);
  printf("Reallocated to memory address: %p\n", ptr4);
  printf("Reallocated to memory address: %p\n", ptr5);
  printf("Freeing all memory locations\n");
  free(ptr);
  free(ptr2);
  free(ptr3);
  free(ptr4);
  free(ptr5);
}


void check_leaks() {
    struct block_meta *current = global_base;
    while (current) {
        if (!current->free) {
            printf("Memory leak detected at address: %p\n", (void *)(current + 1));
        }
        current = current->next;
    }
}

void varied_demonstration() {
  void *ptr = malloc(8);
  void *ptr2 = malloc(5);
  void *ptr3 = malloc(12);

  free(ptr);
  free(ptr2);
  free(ptr3);

  ptr = calloc(1, 9);
  ptr2 = calloc(1, 4);
  ptr3 = calloc(1, 13);
  void *ptr4 = calloc(1, 10);
  void *ptr5 = calloc(1, 8);

  ptr = realloc(ptr, 18);
  ptr2 = realloc(ptr2, 20);
  ptr3 = realloc(ptr3, 24);
  ptr4 = realloc(ptr4, 7);
  ptr5 = realloc(ptr5, 17);
  ptr = realloc(ptr, 14);
  ptr2 = realloc(ptr2, 29);

  free(ptr);
  free(ptr2);
  free(ptr3);
  free(ptr4);
  free(ptr5);
}
