#include "../munit/munit.h"
#include "../src/bootmem.h"
#include "../src/sneknew.h"
#include "../src/snekobject.h"
#include "../src/vm.h"
#include <stdio.h>
#include "stdlib.h"

static MunitResult test_vm_new(const MunitParameter params[], void *user_data)
{
  vm_t *vm = vm_new();
  munit_assert_ptr_not_null(vm->frames);
  munit_assert_ptr_not_null(vm->objects);
  munit_assert_int(vm->frames->capacity, ==, 8);
  munit_assert_int(vm->objects->capacity, ==, 8);
  vm_free(vm);

  return MUNIT_OK;
}

static MunitResult test_vm_free(const MunitParameter params[],
                                void *user_data)
{
  vm_t *vm = vm_new();
  vm_new_frame(vm);
  vm_free(vm);
  munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

static MunitResult test_new_object(const MunitParameter params[],
                                   void *user_data)
{
  vm_t *vm = vm_new();
  snek_object_t *obj = new_snek_integer(20, vm);
  munit_assert_int(obj->kind, ==, INTEGER);
  munit_assert_ptr_equal(vm->objects->data[0], obj);

  vm_free(vm);
  munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

static MunitResult test_frames_are_freed(const MunitParameter params[],
                                         void *user_data)
{
  vm_t *vm = vm_new();
  vm_new_frame(vm);
  vm_new_frame(vm);

  vm_free(vm);
  munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

static MunitResult test_reference_object(const MunitParameter params[],
                                         void *user_data)
{
  vm_t *vm = vm_new();
  new_snek_integer(23, vm);
  new_snek_string("Hello", vm);

  vm_free(vm);
  munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

static MunitResult test_one_frame_ref(const MunitParameter params[],
                                      void *user_data)
{
  vm_t *vm = vm_new();
  frame_t *frame = vm_new_frame(vm);
  snek_object_t *obj = new_snek_integer(46, vm);
  frame_reference_object(frame, obj);

  munit_assert_int(frame->references->count, ==, 1);
  munit_assert_ptr_equal(frame->references->data[0], obj);

  vm_free(vm);
  munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

static MunitResult test_multi_frame_ref(const MunitParameter params[],
                                        void *user_data)
{
  vm_t *vm = vm_new();
  frame_t *frame1 = vm_new_frame(vm);
  frame_t *frame2 = vm_new_frame(vm);
  snek_object_t *obj1 = new_snek_integer(31, vm);
  snek_object_t *obj2 = new_snek_string("Hello", vm);

  frame_reference_object(frame1, obj1);
  frame_reference_object(frame2, obj2);

  munit_assert_int(frame1->references->count, ==, 1);
  munit_assert_ptr_equal(frame1->references->data[0], obj1);
  munit_assert_ptr_equal(frame2->references->data[0], obj2);

  vm_free(vm);
  munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

static MunitResult test_single_frame(const MunitParameter params[],
                                     void *user_data)
{
  vm_t *vm = vm_new();
  frame_t *frame = vm_new_frame(vm);
  snek_object_t *obj = new_snek_integer(53, vm);
  snek_object_t *obj2 = new_snek_string("World", vm);

  mark(vm);

  // Objects not in frame, should not be marked
  munit_assert_false(obj->is_marked);
  munit_assert_false(obj2->is_marked);

  frame_reference_object(frame, obj);
  frame_reference_object(frame, obj2);

  // after adding and marking , objects should be marked
  mark(vm);
  munit_assert_true(obj->is_marked);
  munit_assert_true(obj2->is_marked);

  vm_free(vm);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_multi_frame(const MunitParameter params[],
                                    void *user_data)
{
  vm_t *vm = vm_new();
  frame_t *frame1 = vm_new_frame(vm);
  frame_t *frame2 = vm_new_frame(vm);
  snek_object_t *obj1 = new_snek_integer(41, vm);
  snek_object_t *obj2 = new_snek_string("Hello", vm);
  snek_object_t *obj3 = new_snek_string("Bro", vm);

  frame_reference_object(frame1, obj1);
  frame_reference_object(frame2, obj2);
  frame_reference_object(frame2, obj3);

  mark(vm);
  munit_assert_true(obj1->is_marked);
  munit_assert_true(obj2->is_marked);
  munit_assert_true(obj3->is_marked);

  vm_free(vm);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_trace_vector(const MunitParameter params[],
                                     void *user_data)
{
  vm_t *vm = vm_new();
  frame_t *frame = vm_new_frame(vm);
  snek_object_t *x = new_snek_integer(1, vm);
  snek_object_t *y = new_snek_integer(2, vm);
  snek_object_t *z = new_snek_integer(3, vm);
  snek_object_t *vec = new_snek_vector3(x, y, z, vm);

  munit_assert_false(x->is_marked);
  munit_assert_false(y->is_marked);
  munit_assert_false(z->is_marked);
  munit_assert_false(vec->is_marked);

  frame_reference_object(frame, vec);
  mark(vm);

  munit_assert_false(x->is_marked);
  munit_assert_false(y->is_marked);
  munit_assert_false(z->is_marked);
  munit_assert_true(vec->is_marked);

  trace(vm);
  munit_assert_true(x->is_marked);
  munit_assert_true(y->is_marked);
  munit_assert_true(z->is_marked);

  vm_free(vm);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_trace_array(MunitParameter params[], void *user_data)
{
  vm_t *vm = vm_new();
  frame_t *frame = vm_new_frame(vm);
  snek_object_t *arr = new_snek_array(2, vm);
  snek_object_t *hello = new_snek_string("Hello", vm);
  snek_object_t *world = new_snek_string("World", vm);

  snek_array_set(arr, 0, world);
  snek_array_set(arr, 1, hello);

  frame_reference_object(frame, arr);
  mark(vm);

  munit_assert_false(hello->is_marked);
  munit_assert_false(world->is_marked);
  munit_assert_true(arr->is_marked);

  trace(vm);
  munit_assert_true(hello->is_marked);
  munit_assert_true(world->is_marked);

  vm_free(vm);

  return MUNIT_OK;
}

static MunitResult test_trace_array_nested(MunitParameter params[],
                                           void *user_data)
{
  vm_t *vm = vm_new();
  frame_t *frame = vm_new_frame(vm);
  snek_object_t *first_strings = new_snek_array(2, vm);
  snek_object_t *hello = new_snek_string("Hello", vm);
  snek_object_t *world = new_snek_string("World", vm);
  snek_array_set(first_strings, 0, world);
  snek_array_set(first_strings, 1, hello);

  snek_object_t *second_strings = new_snek_array(2, vm);
  snek_object_t *foo = new_snek_string("Foo", vm);
  snek_object_t *bar = new_snek_string("Bar", vm);
  snek_array_set(second_strings, 0, foo);
  snek_array_set(second_strings, 1, bar);

  snek_object_t *arr = new_snek_array(2, vm);
  snek_array_set(arr, 0, first_strings);
  snek_array_set(arr, 1, second_strings);

  frame_reference_object(frame, arr);
  mark(vm);
  trace(vm);
  munit_assert_true(first_strings->is_marked);
  munit_assert_true(hello->is_marked);
  munit_assert_true(world->is_marked);
  munit_assert_true(second_strings->is_marked);
  munit_assert_true(foo->is_marked);
  munit_assert_true(bar->is_marked);
  munit_assert_true(arr->is_marked);

  vm_free(vm);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_full_system_simple(const MunitParameter params[],
                                           void *user_data)
{
  vm_t *vm = vm_new();
  frame_t *frame = vm_new_frame(vm);

  snek_object_t *str1 = new_snek_string("I think , therefore I am", vm);
  frame_reference_object(frame, str1);
  vm_collect_garbage(vm);
  // nothing should be freed, because we haven't freed the frame
  printf("Asserting that memory is not freed yet:\n");
  munit_assert_false(boot_all_freed());

  frame_free(vm_frame_pop(vm));
  vm_collect_garbage(vm);

  vm_free(vm);

  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_full_system(const MunitParameter params[],
                                    void *user_data)
{
  vm_t *vm = vm_new();
  frame_t *frame1 = vm_new_frame(vm);
  frame_t *frame2 = vm_new_frame(vm);
  frame_t *frame3 = vm_new_frame(vm);

  snek_object_t *str1 =
      new_snek_string("I think , therefore I am, first frame", vm);
  frame_reference_object(frame1, str1);

  snek_object_t *str2 =
      new_snek_string("I think , therefore I am, second frame", vm);
  frame_reference_object(frame2, str2);

  snek_object_t *str3 =
      new_snek_string("I think , therefore I am, third frame", vm);
  frame_reference_object(frame3, str3);

  snek_object_t *int1 = new_snek_integer(59, vm);
  snek_object_t *int2 = new_snek_integer(520, vm);
  snek_object_t *int3 = new_snek_integer(1228, vm);
  snek_object_t *vec = new_snek_vector3(int1, int2, int3, vm);

  frame_reference_object(frame2, vec);
  frame_reference_object(frame3, vec);

  munit_assert_int(vm->objects->count, ==, 7);

  // free the top frame
  frame_free(vm_frame_pop(vm));
  vm_collect_garbage(vm);
  munit_assert_int(vm->objects->count, ==, 6);
  printf("Asserting that memory is not freed yet:\n");
  munit_assert_false(boot_all_freed());
  // TODO: implement boot_is_freed(*ptr) and refactor boot_all_freed()
  // munit_assert_true(boot_is_freed(str3));
  // munit_assert_false(boot_is_freed(str));
  // munit_assert_false(boot_is_freed(str2));

  // VM frees string , but not the vector, since it's also in second frame

  frame_free(vm_frame_pop(vm));
  frame_free(vm_frame_pop(vm));
  vm_collect_garbage(vm);
  munit_assert_int(vm->objects->count, ==, 0);

  vm_free(vm);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}
static MunitTest vm_tests[] = {
    {"/vm_new", test_vm_new, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vm_free", test_vm_free, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/new_object", test_new_object, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/frames_are_freed", test_frames_are_freed, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/reference_object", test_reference_object, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/one_frame_ref", test_one_frame_ref, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/multi_frame_ref", test_multi_frame_ref, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/single_frame", test_single_frame, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/multi_frame", test_multi_frame, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/trace_vector", test_trace_vector, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/trace_array", test_trace_array, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/trace_array_nested", test_trace_array_nested, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/full_system_simple", test_full_system_simple, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/full_system", test_full_system, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

MunitSuite vm_suite = {"/vm", vm_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE};
