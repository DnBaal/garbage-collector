#include "bootmem.h"
#include <stdio.h>

#include "vm.h"

vm_t *vm_new()
{
    vm_t *vm = malloc(sizeof(vm_t));
    if (vm == NULL)
    {
        return NULL;
    }

    int capacity = 8;
    vm->frames = stack_new(capacity);
    if (vm->frames == NULL)
    {
        free(vm);
        return NULL;
    }

    vm->objects = stack_new(capacity);
    if (vm->objects == NULL)
    {
        stack_free(vm->frames);
        free(vm);
        return NULL;
    }

    return vm;
}

void vm_free(vm_t *vm)
{
    if (vm == NULL)
    {
        return;
    }

    stack_free(vm->frames);
    stack_free(vm->objects);
    free(vm);
}