#include "bootmem.h"
#include "snekobject.h"
#include "stack.h"
#include <stdio.h>

#include "vm.h"

vm_t *vm_new() {
  vm_t *vm = malloc(sizeof(vm_t));
  if (vm == NULL) {
    return NULL;
  }

  int capacity = 8;
  vm->frames = stack_new(capacity);
  if (vm->frames == NULL) {
    free(vm);
    return NULL;
  }

  vm->objects = stack_new(capacity);
  if (vm->objects == NULL) {
    stack_free(vm->frames);
    free(vm);
    return NULL;
  }

  return vm;
}

void vm_free(vm_t *vm) {
  for (int i = 0; i < vm->frames->count; i++) {
    frame_free((frame_t *)vm->frames->data[i]);
  }
  stack_free(vm->frames);

  for (int i = 0; i < vm->objects->count; i++) {
    snek_object_free(vm->objects->data[i]);
  }
  stack_free(vm->objects);

  free(vm);
}

void vm_frame_push(vm_t *vm, void *frame) {
  if (vm == NULL || frame == NULL) {
    return;
  }

  stack_push(vm->frames, frame);
}

frame_t *vm_frame_pop(vm_t *vm) { return stack_pop(vm->frames); }

frame_t *vm_new_frame(vm_t *vm) {
  if (vm == NULL) {
    return NULL;
  }

  frame_t *frame = malloc(sizeof(frame_t));
  if (frame == NULL) {
    return NULL;
  }

  frame->references = stack_new(8);
  vm_frame_push(vm, frame);

  return frame;
}

void frame_free(frame_t *frame) {
  if (frame == NULL) {
    return;
  }

  stack_free(frame->references);
  free(frame);
}

void vm_track_object(vm_t *vm, snek_object_t *obj) {
  stack_push(vm->objects, obj);
}

void frame_reference_object(frame_t *frame, snek_object_t *obj) {
  stack_push(frame->references, obj);
}

void vm_collect_garbage(vm_t *vm) {
  mark(vm);
  trace(vm);
  sweep(vm);
}

void sweep(vm_t *vm) {
  for (int i = 0; i < vm->objects->count; i++) {
    snek_object_t *obj = vm->objects->data[i];
    if (obj->is_marked) {
      obj->is_marked = false;
    } else {
      snek_object_free(obj);
      vm->objects->data[i] = NULL;
    }
  }
  stack_remove_nulls(vm->objects);
}

void mark(vm_t *vm) {
  for (size_t i = 0; i < vm->frames->count; i++) {
    frame_t *frame = vm->frames->data[i];
    for (size_t j = 0; j < frame->references->count; j++) {
      snek_object_t *obj = frame->references->data[j];
      obj->is_marked = true;
    }
  }
}

void trace(vm_t *vm) {
  stack_t *gray_objects = stack_new(8);
  if (gray_objects == NULL) {
    return;
  }

  for (int i = 0; i < vm->objects->count; i++) {
    snek_object_t *obj = vm->objects->data[i];
    if (obj->is_marked) {
      stack_push(gray_objects, obj);
    }
  }

  while (gray_objects->count > 0) {
    snek_object_t *ref = stack_pop(gray_objects);
    trace_blacken_object(gray_objects, ref);
  }
  stack_free(gray_objects);
}

void trace_blacken_object(stack_t *gray_objects, snek_object_t *ref) {
  switch (ref->kind) {
  case INTEGER:
  case FLOAT:
  case STRING:
    break;
  case VECTOR3:
    trace_mark_object(gray_objects, ref->data.v_vector3.x);
    trace_mark_object(gray_objects, ref->data.v_vector3.y);
    trace_mark_object(gray_objects, ref->data.v_vector3.z);
    break;
  case ARRAY:
    for (int i = 0; i < ref->data.v_array.size; i++) {
      trace_mark_object(gray_objects, ref->data.v_array.elements[i]);
    }
    break;
  }
}

void trace_mark_object(stack_t *gray_objects, snek_object_t *ref) {
  if (ref == NULL || ref->is_marked) {
    return;
  }

  ref->is_marked = true;
  stack_push(gray_objects, ref);
}
