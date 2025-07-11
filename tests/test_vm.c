#include "../munit/munit.h"
#include "../src/bootmem.h"
#include "../src/vm.h"
#include "stdlib.h"

static MunitResult test_vm_new(const MunitParameter params[],
                               void *user_data)
{
    vm_t *vm = vm_new();
    munit_assert_not_null(vm);
    munit_assert_not_null(vm->frames);
    munit_assert_not_null(vm->objects);
    munit_assert_int(vm->frames->capacity, ==, 8);
    munit_assert_int(vm->objects->capacity, ==, 8);
    vm_free(vm);

    return MUNIT_OK;
}

static MunitResult test_vm_free(const MunitParameter params[],
                                void *user_data)
{
    vm_t *vm = vm_new();
    munit_assert_not_null(vm);
    munit_assert_not_null(vm->frames);
    munit_assert_not_null(vm->objects);
    vm_free(vm);
    munit_assert_true(boot_all_freed());

    return MUNIT_OK;
}

static MunitTest vm_tests[] = {
    {"/vm_new", test_vm_new, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/vm_free", test_vm_free, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

MunitSuite vm_suite = {
    "/vm",
    vm_tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE};