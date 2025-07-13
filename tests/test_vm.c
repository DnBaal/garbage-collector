#include "../munit/munit.h"
#include "../src/bootmem.h"
#include "../src/vm.h"
#include "stdlib.h"
#include "../src/sneknew.h"
#include "../src/snekobject.h"

static MunitResult test_vm_new(const MunitParameter params[],
                               void *user_data)
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
    snek_object_t *obj = new_snek_integer(42, vm);
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
    new_snek_integer(42, vm);
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
    snek_object_t *obj = new_snek_integer(42, vm);
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
    snek_object_t *obj1 = new_snek_integer(42, vm);
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

static MunitTest vm_tests[] = {
    {"/vm_new", test_vm_new, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vm_free", test_vm_free, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/new_object", test_new_object, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/frames_are_freed", test_frames_are_freed, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/reference_object", test_reference_object, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/one_frame_ref", test_one_frame_ref, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/multi_frame_ref", test_multi_frame_ref, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

MunitSuite vm_suite = {
    "/vm",
    vm_tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE};