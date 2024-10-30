#ifndef functions_worst
#define functions_worst

#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void *nofree_malloc(size_t size);

struct block_meta;

struct block_meta *request_space(struct block_meta* last, size_t size);

struct block_meta *find_worst_fit_block(struct block_meta **last, size_t size);

void *malloc(size_t size);

void *calloc(size_t nelem, size_t elsize);

void free(void *ptr);

void *realloc(void *ptr, size_t size);

struct block_meta *get_block_ptr(void *ptr);

void demonstration();

void check_leaks();

void varied_demonstration();

#endif