#ifndef BOOTMEM_H
#define BOOTMEM_H

#include <stdlib.h>
#include <stddef.h>

/* Tracking allocation functions */
void *boot_malloc(size_t size);
void *boot_calloc(size_t num, size_t size);
void *boot_realloc(void *ptr, size_t size);
void boot_free(void *ptr);

/* Memory tracking check */
int boot_all_freed(void);

/* Macros to replace malloc/free in your code */
#ifndef BOOTMEM_IMPLEMENTATION
#define malloc boot_malloc
#define calloc boot_calloc
#define realloc boot_realloc
#define free boot_free
#endif

#endif
