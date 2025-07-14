#define BOOTMEM_IMPLEMENTATION
#include <stdlib.h>
#include <stdio.h>
#include "bootmem.h"

static int allocation_count = 0;

void *boot_malloc(size_t size)
{
  allocation_count++;
  return malloc(size);
}

void *boot_calloc(size_t num, size_t size)
{
  allocation_count++;
  return calloc(num, size);
}

void *boot_realloc(void *ptr, size_t size)
{
  if (ptr == NULL)
  {
    allocation_count++;
  }
  return realloc(ptr, size);
}

void boot_free(void *ptr)
{
  if (ptr != NULL)
  {
    allocation_count--;
    free(ptr);
  }
}

int boot_all_freed(void)
{
  if (allocation_count != 0)
  {
    printf("[bootmem]: %d unfreed allocations\n",
           allocation_count);
  }
  return allocation_count == 0;
}
