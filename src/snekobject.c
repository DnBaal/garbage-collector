#include "bootmem.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "snekobject.h"

bool snek_array_set(snek_object_t *snek_obj, size_t index,
                    snek_object_t *value)
{
  if (snek_obj == NULL || value == NULL)
  {
    return false;
  }
  if (snek_obj->kind != ARRAY)
  {
    return false;
  }
  if (index >= snek_obj->data.v_array.size || index < 0)
  {
    return false;
  }

  snek_obj->data.v_array.elements[index] = value;
  return true;
}

snek_object_t *snek_array_get(snek_object_t *snek_obj, size_t index)
{
  if (snek_obj == NULL)
  {
    return NULL;
  }
  if (snek_obj->kind != ARRAY)
  {
    return NULL;
  }
  if (index >= snek_obj->data.v_array.size || index < 0)
  {
    return NULL;
  }

  return snek_obj->data.v_array.elements[index];
}

int snek_length(snek_object_t *obj)
{
  if (obj == NULL)
  {
    return -1;
  }

  switch (obj->kind)
  {
  case INTEGER:
  case FLOAT:
    return 1;
  case STRING:
    return strlen(obj->data.v_string);
  case VECTOR3:
    return 3;
  case ARRAY:
    return obj->data.v_array.size;
  default:
    return -1;
  }
}

snek_object_t *snek_add(snek_object_t *a, snek_object_t *b)
{
  if (a == NULL || b == NULL)
  {
    return NULL;
  }

  switch (a->kind)
  {
  case INTEGER:
    switch (b->kind)
    {
    case INTEGER:
      return new_snek_integer(a->data.v_int + b->data.v_int);
    case FLOAT:
      return new_snek_float(a->data.v_int + b->data.v_float);
    default:
      return NULL;
    }
  case FLOAT:
    switch (b->kind)
    {
    case INTEGER:
      return new_snek_float(a->data.v_float + b->data.v_int);
    case FLOAT:
      return new_snek_float(a->data.v_float + b->data.v_float);
    default:
      return NULL;
    }
  case STRING:
    switch (b->kind)
    {
    case STRING:
      size_t new_len = strlen(a->data.v_string) + strlen(b->data.v_string) + 1;
      char *dst = calloc(new_len, sizeof(char));
      strcat(dst, a->data.v_string);
      strcat(dst, b->data.v_string);
      snek_object_t *new_str = new_snek_string(dst);

      free(dst);
      return new_str;

    default:
      return NULL;
    }

  case VECTOR3:
    switch (b->kind)
    {
    case VECTOR3:
      return new_snek_vector3(
          snek_add(a->data.v_vector3.x, b->data.v_vector3.x),
          snek_add(a->data.v_vector3.y, b->data.v_vector3.y),
          snek_add(a->data.v_vector3.z, b->data.v_vector3.z));
    default:
      return NULL;
    }

  case ARRAY:
    switch (b->kind)
    {
    case ARRAY:
      int len_a = snek_length(a);
      int len_b = snek_length(b);

      snek_object_t *new_array = new_snek_array(len_a + len_b);

      for (int i = 0; i < len_a; i++)
      {
        snek_array_set(new_array, i, snek_array_get(a, i));
      }

      for (int i = 0; i < len_b; i++)
      {
        snek_array_set(new_array, i + len_a, snek_array_get(b, i));
      }
      return new_array;
    default:
      return NULL;
    }
  default:
    return NULL;
  }
}

void snek_object_free(snek_object_t *obj)
{
  switch (obj->kind)
  {
  case INTEGER:
  case FLOAT:
    break;
  case STRING:
    free(obj->data.v_string);
    break;
  case VECTOR3:
    break;
  case ARRAY:
    snek_array_t *array = &obj->data.v_array;
    free(array->elements);
    break;
  }
  free(obj);
}

// Moved to sneknew.h and refactored, test_snekobject.c -> test_vm.c

// snek_object_t *_new_snek_object()
// {
//   snek_object_t *obj = calloc(1, sizeof(snek_object_t));
//   if (obj == NULL)
//   {
//     return NULL;
//   }

//   return obj;
// }

// snek_object_t *new_snek_integer(int value)
// {
//   snek_object_t *obj = _new_snek_object();
//   if (obj == NULL)
//   {
//     return NULL;
//   }

//   obj->kind = INTEGER;
//   obj->data.v_int = value;
//   return obj;
// }

// snek_object_t *new_snek_float(float value)
// {
//   snek_object_t *obj = _new_snek_object();
//   if (obj == NULL)
//   {
//     return NULL;
//   }

//   obj->kind = FLOAT;
//   obj->data.v_float = value;
//   return obj;
// }

// snek_object_t *new_snek_string(char *value)
// {
//   snek_object_t *obj = _new_snek_object();
//   if (obj == NULL)
//   {
//     return NULL;
//   }

//   size_t len = strlen(value);
//   char *dst = malloc(len + 1);
//   if (dst == NULL)
//   {
//     free(obj);
//     return NULL;
//   }
//   strcpy(dst, value);

//   obj->kind = STRING;
//   obj->data.v_string = dst;

//   return obj;
// }

// snek_object_t *new_snek_vector3(snek_object_t *x, snek_object_t *y,
//                                 snek_object_t *z)
// {
//   if (x == NULL || y == NULL || z == NULL)
//     return NULL;
//   snek_object_t *obj = _new_snek_object();
//   if (obj == NULL)
//   {
//     return NULL;
//   }

//   obj->kind = VECTOR3;
//   snek_vector_t vector3 = {.x = x, .y = y, .z = z};
//   obj->data.v_vector3 = vector3;

//   return obj;
// }

// snek_object_t *new_snek_array(size_t size)
// {
//   snek_object_t *obj = _new_snek_object();
//   if (obj == NULL)
//   {
//     return NULL;
//   }

//   snek_object_t **dst = calloc(size, sizeof(snek_object_t *));
//   if (dst == NULL)
//   {
//     free(obj);
//     return NULL;
//   }

//   obj->kind = ARRAY;
//   obj->data.v_array = (snek_array_t){.size = size, .elements = dst};
//   return obj;
// }
