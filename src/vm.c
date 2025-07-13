#include "bootmem.h"
#include <stdio.h>
#include "stack.h"
#include "snekobject.h"

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
    for (int i = 0; i < vm->frames->count; i++)
    {
        frame_free((frame_t *)vm->frames->data[i]);
    }
    stack_free(vm->frames);

    for (int i = 0; i < vm->objects->count; i++)
    {
        snek_object_free(vm->objects->data[i]);
    }
    stack_free(vm->objects);

    free(vm);
}

void vm_frame_push(vm_t *vm, void *frame)
{
    if (vm == NULL || frame == NULL)
    {
        return;
    }

    stack_push(vm->frames, frame);
}

frame_t *vm_new_frame(vm_t *vm)
{
    if (vm == NULL)
    {
        return NULL;
    }

    frame_t *frame = malloc(sizeof(frame_t));
    if (frame == NULL)
    {
        return NULL;
    }

    frame->references = stack_new(8);
    vm_frame_push(vm, frame);

    return frame;
}

void frame_free(frame_t *frame)
{
    if (frame == NULL)
    {
        return;
    }

    stack_free(frame->references);
    free(frame);
}

void vm_track_object(vm_t *vm, snek_object_t *obj)
{
    stack_push(vm->objects, obj);
}

void frame_reference_object(frame_t *frame, snek_object_t *obj)
{
    stack_push(frame->references, obj);
}