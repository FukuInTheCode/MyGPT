#pragma once
#include <cstdlib>
#define ALLOC_H
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>


typedef struct _alloc_s {
    size_t alloc_size;
    void *ptr;
    struct _alloc_s *next;
} _alloc_t;

static _alloc_t *root = NULL;

static void *(*libc_malloc)(size_t) = NULL;
static void (*libc_free)(void *) = NULL;

__attribute__((constructor))
static void initialize()
{
    libc_malloc = (void *(*)(size_t))(dlsym(RTLD_NEXT, "malloc"));
    libc_free = (void (*)(void *))(dlsym(RTLD_NEXT, "free"));

    if (!libc_malloc || !libc_free) {
        fprintf(stderr, "Error in obtaining original malloc and free\n");
        exit(EXIT_FAILURE);
    }
}

void *malloc(size_t size)
{
    if (!libc_malloc)
        initialize();
    _alloc_t *new_root = (_alloc_t *)libc_malloc(sizeof(_alloc_t));
    if (!new_root)
    {
        fprintf(stderr, "ALLOC ERR: failed to malloc size of %zu\n", size);
        exit(EXIT_FAILURE);
    }
    new_root->next = root;
    root = new_root;
    void *ptr = libc_malloc(size);
    if (!ptr)
    {
        fprintf(stderr, "ALLOC ERR: failed to malloc size of %zu\n", size);
        exit(EXIT_FAILURE);
    }
    new_root->alloc_size = size;
    new_root->ptr = ptr;
    return ptr;
}

void free(void *ptr)
{
    if (!ptr)
        return;
    if (!libc_free)
        initialize();
    _alloc_t *container = root;
    _alloc_t *container_prec = NULL;
    for (; container && container->ptr != ptr; container_prec = container, container = container->next);
    if (!container)
        return;
    libc_free(container->ptr);
    _alloc_t *container_next = container->next;
    libc_free(container);
    if (!container_prec)
        root = container_next;
    else
        container_prec->next = container_next;
}
