#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include <stdint.h>

#define CAPACITY 320000
#define CHUNK_LIST_CAPACITY 1024

typedef struct {
    size_t size;
    uint8_t* start;
} heap_chunk;

typedef struct {
    heap_chunk chunks [CHUNK_LIST_CAPACITY];
    size_t count;
} heap_chunk_list;

void* alloc(size_t size);
void free_mem(void* ptr);

#endif