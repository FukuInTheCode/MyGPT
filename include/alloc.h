#pragma once
#define ALLOC_H
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <cstddef>


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
