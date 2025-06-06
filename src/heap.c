#include "heap.h"
#include <stdio.h>
#include <stdbool.h>

uint8_t heap[CAPACITY];
size_t heap_size = 0;

heap_chunk_list allocated_heap_list = {0};
heap_chunk_list freed_chunk_list = {
    .count = 1,
    .chunks = {[0] = {.start = heap, .size = sizeof(heap)}}
};

static bool chunk_equals(const heap_chunk* a, const heap_chunk* b) {
    return (a->start == b->start && a->size == b->size);
}

int find_chunk_for_size(const heap_chunk_list* list, const size_t size) {
    for (size_t i = 0; i < list->count; i++) {
        const heap_chunk chunk = list->chunks[i];
        if (chunk.start != NULL && chunk.size >= size) {
            return i;
        }
    }
    printf("heap chunk not found\n");
    return -1;
}

bool remove_chunk_from_list(heap_chunk_list* chunk_list, const heap_chunk* chunk) {
    for (size_t i = 0; i < chunk_list->count; i++) {
        if (chunk_equals(&chunk_list->chunks[i], chunk)) {
            for (size_t j = i; j < chunk_list->count - 1; j++) {
                chunk_list->chunks[j] = chunk_list->chunks[j + 1];
            }
            chunk_list->count--;
            return true;
        }
    }
    return false;
}

bool insert_chunk_to_list(heap_chunk_list* chunk_list, const heap_chunk* chunk) {
    if (chunk_list->count >= CHUNK_LIST_CAPACITY) {
        printf("Error: chunk list is full\n");
        return false;
    }
    chunk_list->chunks[chunk_list->count] = *chunk;
    chunk_list->count++;
    return true;
}

int find_chunk_for_start(const heap_chunk_list* chunk_list, const void* ptr) {
    for (size_t i = 0; i < chunk_list->count; i++) {
        const heap_chunk chunk = chunk_list->chunks[i];
        if (chunk.start == ptr) {
            return i;
        }
    }
    printf("heap chunk not found\n");
    return -1;
}

void* alloc(const size_t size) {
    if (size == 0) return NULL;

    const int chunk_idx = find_chunk_for_size(&freed_chunk_list, size);
    if (chunk_idx == -1) {
        return NULL;
    }

    const heap_chunk free_chunk = freed_chunk_list.chunks[chunk_idx];
    if (free_chunk.start == NULL) {
        return NULL;
    }

    if (!remove_chunk_from_list(&freed_chunk_list, &free_chunk)) {
        return NULL;
    }

    const heap_chunk allocated = { .start = free_chunk.start, .size = size };
    if (!insert_chunk_to_list(&allocated_heap_list, &allocated)) {
        insert_chunk_to_list(&freed_chunk_list, &free_chunk);
        return NULL;
    }

    const size_t leftover_size = free_chunk.size - size;
    if (leftover_size > 0) {
        const heap_chunk leftover = {
            .start = (uint8_t*)free_chunk.start + size,
            .size = leftover_size
        };
        if (!insert_chunk_to_list(&freed_chunk_list, &leftover)) {
            printf("Warning: failed to add leftover chunk to free list\n");
        }
    }

    return allocated.start;
}

void free_mem(const void* ptr) {
    if (ptr == NULL) return;

    const int idx = find_chunk_for_start(&allocated_heap_list, ptr);
    if (idx == -1) return;

    const heap_chunk chunk_to_free = allocated_heap_list.chunks[idx];

    if (!remove_chunk_from_list(&allocated_heap_list, &chunk_to_free)) return;

    if (!insert_chunk_to_list(&freed_chunk_list, &chunk_to_free)) {
        printf("Error: failed to add chunk to free list\n");
    }
}