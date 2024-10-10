#pragma once
#define ALLOC_H
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <unistd.h>

typedef struct _alloc_s {
    size_t alloc_size;
    void *ptr;
    struct _alloc_s *next;
} _alloc_t;

static _alloc_t *__root = NULL;

static size_t __heap_allocated_size = 0;

void *__alloc_malloc(size_t size)
{
    _alloc_t *new_alloc = calloc(1, sizeof(_alloc_t));
    if (!new_alloc)
    {
        fprintf(stderr, "ALLOC: Failed to alloc %zu!\n", size);
        exit(EXIT_FAILURE);
    }
    new_alloc->next = __root;
    new_alloc->alloc_size = size;
    __root = new_alloc;
    void *ptr = malloc(size);
    if (!ptr)
    {
        fprintf(stderr, "ALLOC: Failed to alloc %zu!\n", size);
        exit(EXIT_FAILURE);
    }
    __root->ptr = ptr;
    __heap_allocated_size += size;
    return ptr;
}

void __alloc_free(void *ptr)
{
    if (!ptr)
        return;
    _alloc_t *cursor = __root;
    _alloc_t *old_cursor = NULL;
    for (; cursor && cursor->ptr != ptr; old_cursor = cursor, cursor = cursor->next);
    if (!cursor)
        return;
    if (!old_cursor)
        __root = cursor->next;
    else
        old_cursor->next = cursor->next;
    free(ptr);
    __heap_allocated_size -= cursor->alloc_size;
    free(cursor);
}

__attribute__((destructor))
void cleanup(void)
{
    for (; __root;) {
        _alloc_t *next = __root->next;
        free(__root->ptr);
        free(__root);
        __root = next;
    }

}
