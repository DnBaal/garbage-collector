#pragma once

#include "snekobject.h"
#include "vm.h"

snek_object_t *_new_snek_object(vm_t *vm);
snek_object_t *new_snek_integer(int value, vm_t *vm);
snek_object_t *new_snek_float(float value, vm_t *vm);
snek_object_t *new_snek_string(char *value, vm_t *vm);
snek_object_t *new_snek_array(size_t size, vm_t *vm);
snek_object_t *new_snek_vector3(snek_object_t *x, snek_object_t *y,
                                snek_object_t *z, vm_t *vm);