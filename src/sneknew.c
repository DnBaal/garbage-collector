#include "bootmem.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"

#include "snekobject.h"

snek_object_t *_new_snek_object(vm_t *vm)
{
    snek_object_t *obj = calloc(1, sizeof(snek_object_t));
    if (obj == NULL)
    {
        return NULL;
    }

    vm_track_object(vm, obj);

    return obj;
}

snek_object_t *new_snek_integer(int value, vm_t *vm)
{
    snek_object_t *obj = _new_snek_object(vm);
    if (obj == NULL)
    {
        return NULL;
    }

    obj->kind = INTEGER;
    obj->data.v_int = value;
    return obj;
}

snek_object_t *new_snek_float(float value, vm_t *vm)
{
    snek_object_t *obj = _new_snek_object(vm);
    if (obj == NULL)
    {
        return NULL;
    }

    obj->kind = FLOAT;
    obj->data.v_float = value;
    return obj;
}

snek_object_t *new_snek_string(char *value, vm_t *vm)
{
    snek_object_t *obj = _new_snek_object(vm);
    if (obj == NULL)
    {
        return NULL;
    }

    size_t len = strlen(value);
    char *dst = malloc(len + 1);
    if (dst == NULL)
    {
        free(obj);
        return NULL;
    }
    strcpy(dst, value);

    obj->kind = STRING;
    obj->data.v_string = dst;

    return obj;
}

snek_object_t *new_snek_vector3(snek_object_t *x, snek_object_t *y,
                                snek_object_t *z, vm_t *vm)
{
    if (x == NULL || y == NULL || z == NULL)
        return NULL;
    snek_object_t *obj = _new_snek_object(vm);
    if (obj == NULL)
    {
        return NULL;
    }

    obj->kind = VECTOR3;
    snek_vector_t vector3 = {.x = x, .y = y, .z = z};
    obj->data.v_vector3 = vector3;

    return obj;
}

snek_object_t *new_snek_array(size_t size, vm_t *vm)
{
    snek_object_t *obj = _new_snek_object(vm);
    if (obj == NULL)
    {
        return NULL;
    }

    snek_object_t **dst = calloc(size, sizeof(snek_object_t *));
    if (dst == NULL)
    {
        free(obj);
        return NULL;
    }

    obj->kind = ARRAY;
    obj->data.v_array = (snek_array_t){.size = size, .elements = dst};
    return obj;
}