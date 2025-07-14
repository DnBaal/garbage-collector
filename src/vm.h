#pragma once

#include "snekobject.h"
#include "stack.h"

typedef struct VirtualMachine
{
    stack_t *frames;
    stack_t *objects;
} vm_t;

typedef struct Frame
{
    stack_t *references;
} frame_t;

/// Main flow for garbage collection
void mark(vm_t *vm);
void trace(vm_t *vm);
void sweep(vm_t *vm);

void vm_collect_garbage(vm_t *vm);

/// Helper funcs for `trace`
void trace_blacken_object(stack_t *gray_objects, snek_object_t *ref);
void trace_mark_object(stack_t *gray_objects, snek_object_t *ref);

vm_t *vm_new();
void vm_free(vm_t *vm);
void vm_track_object(vm_t *vm, snek_object_t *obj);

void vm_frame_push(vm_t *vm, void *frame);
frame_t *vm_frame_pop(vm_t *vm);
frame_t *vm_new_frame(vm_t *vm);

void frame_free(frame_t *frame);
void frame_reference_object(frame_t *frame, snek_object_t *obj);
